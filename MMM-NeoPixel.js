/* global Module */

/* Magic Mirror
 * Module: MMM-NeoPixel
 *
 * By Philip Karlsson Gisslow
 * MIT Licensed.
 */

Module.register("MMM-NeoPixel",{

	// Default module config.
	defaults: {
		text: "NeoPixel"
	},

	notificationReceived: function(notification, payload, sender) {
		var self = this;
		if (notification === "NeoPixel") {
			self.sendSocketNotification('set_state', payload.state);
		}
		else if (notification === "SHOW_ALERT") {
			self.sendSocketNotification('set_state', 'iss_alarm');
		}
	},
});