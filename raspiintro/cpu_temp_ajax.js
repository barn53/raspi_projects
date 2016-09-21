var execute = function(query) {

    var $ = require('jquery-deferred');
    var sqlite3 = require('sqlite3').verbose();

    var sql = "SELECT * FROM (SELECT TIME, TEMP_CELCIUS FROM LOG ORDER BY TIME DESC LIMIT 24) ORDER BY 1 ASC";

    if (query.all === 'true') {
        sql = "SELECT CAST(AVG(TIME) AS INTEGER) AS TIME, AVG(TEMP_CELCIUS) AS TEMP_CELCIUS FROM LOG " +
            "GROUP BY CAST (ROWID / ((SELECT COUNT(*) FROM LOG) / 24) AS INTEGER) ORDER BY 1 ASC";
    }

    var deferred = $.Deferred();
    var db = new sqlite3.Database('log_cpu_temp.db');
    db.all(sql, function(err, rows) {
        var data = [];
        for (var rr = 0; rr < rows.length; rr++) {
            data.push([rows[rr].TIME, parseFloat(rows[rr].TEMP_CELCIUS)]);
        }
        deferred.resolve(data);
    });
    db.close();
    return deferred.promise();
};

module.exports.execute = exports.execute = execute;
