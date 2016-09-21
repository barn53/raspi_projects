var sqlite = require('sqlite3').verbose();
var db = new sqlite.Database('/home/pi/raspiintro/log_cpu_temp.db');

db.serialize(function() {
    db.run('CREATE TABLE IF NOT EXISTS LOG(TIME, TEMP_CELCIUS)');
    var fs = require('fs');
    var temperature = fs.readFileSync("/sys/class/thermal/thermal_zone0/temp");
    temperature = (temperature / 1000).toPrecision(3);
    var stmt = db.prepare('INSERT INTO LOG VALUES (?, ?)');
    var d = new Date;
    stmt.run(d.getTime(), temperature);
    stmt.finalize();
});
db.close();
