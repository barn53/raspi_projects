var pfio = require('piface-node');
pfio.init();
pfio.digital_write(1,1); // (pin, state)
pfio.digital_write(0,1); // (pin, state)
pfio.digital_write(7,1); // (pin, state)
var foo = pfio.digital_read(0); // (pin; returns state)
console.log(foo);
pfio.deinit();
