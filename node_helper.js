var NeoPixel = require("./NeoPixel.js");
var NodeHelper = require("node_helper");

module.exports = NodeHelper.create({

    socketNotificationReceived: function(notification, payload) {
        this.pixel.set_state(payload)
    },

    start: function() {
        this.pixel = new NeoPixel('/dev/ttyACM0')
    }
});