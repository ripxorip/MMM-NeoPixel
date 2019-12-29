const SerialPort = require('serialport')

function NeoPixel(port) {
    this.port = new SerialPort(port, {
      baudRate: 9600
    });
}

NeoPixel.prototype.set_state = function(state) {
    this.port.write(state+'\r\n');
}

module.exports = NeoPixel;