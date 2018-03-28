(function ($) {

  Backdrop.honeypot = {};
  Backdrop.honeypot.timestampJS = new Date();

  Backdrop.behaviors.honeypotJS = {
    attach: function (context, settings) {
      $('form.honeypot-timestamp-js').once('honeypot-timestamp').bind('submit', function() {
        var $honeypotTime = $(this).find('input[name="honeypot_time"]');
        $honeypotTime.attr('value', Backdrop.behaviors.honeypotJS.getIntervalTimestamp());
      });
    },
    getIntervalTimestamp: function() {
      var now = new Date();
      var interval = Math.floor((now - Backdrop.honeypot.timestampJS) / 1000);
      return Backdrop.settings.honeypot.jsToken + '|' + interval;
    }
  };

  if (Backdrop.ajax && Backdrop.ajax.prototype && Backdrop.ajax.prototype.beforeSubmit) {
    Backdrop.ajax.prototype.honeypotOriginalBeforeSubmit = Backdrop.ajax.prototype.beforeSubmit;
    Backdrop.ajax.prototype.beforeSubmit = function (form_values, element, options) {
      if (this.form && $(this.form).hasClass('honeypot-timestamp-js')) {
        for (key in form_values) {
          // Inject the right interval timestamp.
          if (form_values[key].name == 'honeypot_time' && form_values[key].value == 'no_js_available') {
            form_values[key].value = Backdrop.behaviors.honeypotJS.getIntervalTimestamp();
          }
        }
      }

      // Call the original function in case someone else has overridden it.
      return Backdrop.ajax.prototype.honeypotOriginalBeforeSubmit(form_values, element, options);
    }
  }

}(jQuery));
