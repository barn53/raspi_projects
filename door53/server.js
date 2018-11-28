var secrets = require('./secrets')
var http = require('http')
var https = require('https')
var url = require('url')
var moment = require('moment');
var gpio = require('onoff').Gpio;
var ffi = require('ffi');
var crypto = require('crypto');
var fs = require('fs');

var led = new gpio(27, 'high');

var libsend = ffi.Library('libsend433', {
    'setup': ['void', ['int']],
    'sendElro': ['void', ['int', 'int', 'bool', 'int']],
    'sendInterTechno': ['void', ['int', 'int', 'bool', 'int']],
})

libsend.setup(0)

http.createServer(_handler).listen(secrets.server_port)
console.log('Server running on port ' + secrets.server_port)

function _handler(request, response) {
    console.log("\n===============================================")
    console.log(" " + moment().format())
    console.log(" " + request.url)

    if (request.method !== 'GET') {
        responseError(response)
        return
    }

    var get = url.parse(request.url, true)
    if (get.query === undefined
        || get.query.o === undefined) {
        responseError(response)
        return
    }

    try {
        var o = JSON.parse(get.query.o)
    }
    catch (e) {
        console.log(e)
        responseError(response)
        return
    }

    var oString = JSON.stringify(o, null, 2)

    const cipher = crypto.createCipher('aes192', secrets.crypto_password);
    var encrypted = cipher.update(oString, 'utf8', 'base64');
    encrypted += cipher.final('base64');

    console.log("o: " + oString)
    console.log("crypto: " + encrypted)

    var command = o.command || ''
    var ticket = o.ticket || ''
    var latitude = o.latitude || 0
    var longitude = o.longitude || 0
    var user = o.user || ''

    if (command === 'ticket') {
        createTicket(response)
        return
    }
    else if (command === 'test') {
        if (validTicket(ticket)) {
            responseOk(response, '1')
            triggerIFTTT(command, user, latitude + ', ' + longitude)
        }
        else {
            responseError(response)
        }
        return
    }
    /*********************************************/
    else if (command === 'gaga') {
        responseOk(response, '1')
        led.write(0, (err) => {
            setTimeout(() => {
                led.write(1, (err) => { })
            }, 500)
        })
        return
    }
    else if (command === 't') {
        responseOk(response, '1')
        libsend.sendElro(secrets.elro_id, 1, true, 10)
        libsend.sendInterTechno(secrets.intertechno_id, 2, true, 10)
    }
    else if (command === 'f') {
        responseOk(response, '1')
        libsend.sendElro(secrets.elro_id, 1, false, 10)
        libsend.sendInterTechno(secrets.intertechno_id, 2, false, 10)
    }
    /*********************************************/

    if (!checkPosition(latitude, longitude)) {
        console.log("wrong position: latitude=" + latitude + ", longitude=" + longitude)
        responseError(response)
        return
    }

    if (command === 'door') {
        if (validTicket(ticket)) {
            responseOk(response, '1')
            led.write(0, (err) => {
                setTimeout(() => {
                    led.write(1)
                }, 500)
            })
            triggerIFTTT(command, user, latitude + ', ' + longitude)
        }
        else {
            responseError(response)
        }
    }
    else if (command === 't') {
        if (validTicket(ticket)) {
            responseOk(response, '1')
            libsend.sendElro(secrets.elro_id, 1, true, 10)
            libsend.sendInterTechno(secrets.intertechno_id, 2, true, 10)
        }
        else {
            responseError(response)
        }
    }
    else if (command === 'f') {
        if (validTicket(ticket)) {
            responseOk(response, '1')
            libsend.sendElro(secrets.elro_id, 1, false, 10)
            libsend.sendInterTechno(secrets.intertechno_id, 2, false, 10)
        }
        else {
            responseError(response)
        }
    }
    else {
        validTicket(ticket)
        responseError(response)
    }
}

function responseOk(response, value) {
    response.writeHead(200)
    response.end(value, 'utf8')
}
function responseError(response) {
    response.writeHead(200)
    response.end('0', 'utf8')
}

function triggerIFTTT(value1, value2, value3) {
    var options = {
        host: 'maker.ifttt.com',
        path: '/trigger/relais/with/key/'
            + secrets.ifttt_key
            + '?value1=' + encodeURI(value1)
            + '&value2=' + encodeURI(value2)
            + '&value3=' + encodeURI(value3)
    }
    https.request(options, () => {
        console.log('request callback');
    })
        .on('end', () => {
            console.log('request end');
        })
        .on('error', (e) => {
            console.log(`problem with request: ${e.message}`);
        })
        .end();
}

function fileNameFromTicket(ticket) {
    return "/ram/" + ticket
}

function checkPosition(latitude, longitude) {
    if (parseFloat(latitude) === 0.0 && parseFloat(longitude) === 0.0) {
        return true;
    } else if (parseFloat(latitude) >= secrets.min_latitude
        && parseFloat(latitude) <= secrets.max_latitude
        && parseFloat(longitude) >= secrets.min_longitude
        && parseFloat(longitude) <= secrets.max_longitude) {
        return true;
    }
    return false;
}

function validTicket(ticket) {
    const file = fileNameFromTicket(ticket)
    try {
        fs.closeSync(fs.openSync(file, "r"))
        fs.unlinkSync(file)
        console.log("valid ticket: " + ticket);
        return true
    }
    catch (e) {
        console.log("invalid ticket: " + ticket);
        return false
    }
}

function createTicket(response) {
    const buf = crypto.randomBytes(16);
    const ticket = buf.toString('hex') //('base64')
    const file = fileNameFromTicket(ticket)

    fs.open(file, "w", (err, fd) => {
        if (!err) {
            setTimeout(() => {
                fs.unlink(file, () => { })
                console.log('' + fd + ': invalidate ticket: ' + ticket);
            }, 30000)
            console.log('' + fd + ': create ticket: ' + ticket);
            responseOk(response, ticket)
        }
        else {
            console.log("error creating ticket: " + err);
            responseError(response)
        }
    })
}
