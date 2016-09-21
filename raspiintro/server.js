var http = require('http');
var path = require('path');
var fs = require('fs');
var url = require('url');

var _handler = function(request, response) {
    if (request.method === 'GET') {
        _handleGET(request, response);
    }
    else if (request.method === 'POST') {
        _handlePOST(request, response);
    }
};


var _handleGET = function(request, response) {

    var parsed = url.parse(request.url, true, true);

    console.log('GET url: ' + request.url);
    //console.dir(parsed);

    if (parsed.pathname === '/ajax') {
        var mod = './' + parsed.query.module + '_ajax.js';
        var ajaxHandler = require(mod);
        console.log('Load module: ' + mod + '  ' + JSON.stringify(parsed.query));

        ajaxHandler.execute(parsed.query).done(function(data) {
            //console.dir(data);
            response.setHeader('Content-Type', 'application/json');
            response.writeHead(200);
            response.end(JSON.stringify(data));
        });
    }
    else {
        var file = '.'  + parsed.pathname;

        if (fs.existsSync(file) && fs.statSync(file).isFile()) {
            var stream = fs.createReadStream(file);

            stream.on('error', function(error) {
                response.writeHead(500);
                response.end();
                return;
            });

            var mime = require('mime');
            response.setHeader('Content-Type', mime.lookup(file));
            response.writeHead(200);

            stream.pipe(response);
            stream.on('end', function() {
                response.end();
            });
        }
        else {
            console.log(' - not found!');
            response.writeHead(404); 
            response.end(); 
        }
    }
};



var _handlePOST = function(request, response) {
    response.writeHead(405); 
    response.end(); 
};


http.createServer(_handler).listen(8888);
