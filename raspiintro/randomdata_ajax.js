var execute = function() {

    var $ = require('jquery-deferred');
    var deferred = $.Deferred();

    var series = [];
    for (var ss = 0; ss < 3; ss++) {
        series.push({data: []});
        for (var ii = 0; ii < 15; ii++) {
            series[ss].data[ii] = Math.floor((Math.random() * 100) -49);
        }
    }

    deferred.resolve(series);

    return deferred.promise();
};

module.exports.execute = exports.execute = execute;
