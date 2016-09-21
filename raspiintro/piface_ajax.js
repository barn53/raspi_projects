var pfio = require('piface-node');
pfio.init();

var execute = function(query) {

    var $ = require('jquery-deferred');
    var deferred = $.Deferred();

    pfio.digital_write(query.port, 1); // (pin, state)

    setTimeout(function() {
        pfio.digital_write(query.port, 0); // (pin, state)
        deferred.resolve({status: 'ok'});
    }, 1000);

    return deferred.promise();
};

module.exports.execute = exports.execute = execute;
