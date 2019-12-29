var NeoPixel = require("./NeoPixel.js");

var pixel = new NeoPixel('/dev/ttyACM0');

function set_pink()
{
    pixel.set_state("blue");
}

setTimeout(set_pink, 5000);