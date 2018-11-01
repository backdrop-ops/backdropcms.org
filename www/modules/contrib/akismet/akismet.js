(function ($) {

Backdrop.akismet = Backdrop.akismet || {};

/**
 * Open links to Akismet.com in a new window.
 *
 * Required for valid XHTML Strict markup.
 */
Backdrop.behaviors.akismetTarget = {
  attach: function (context) {
    $(context).find('.akismet-target').click(function () {
      this.target = '_blank';
    });
  }
};

/**
 * Retrieve and attach the form behavior analysis tracking image if it has not
 * yet been added for the form.
 */
Backdrop.behaviors.akismetFBA = {
  attach: function (context, settings) {
    $(':input[name="akismet[fba]"][value=""]', context).once().each(function() {
      $input = $(this);
      $.ajax({
        url: Backdrop.settings.basePath + Backdrop.settings.pathPrefix + 'akismet/fba',
        type: 'POST',
        dataType: 'json',
        success: function(data) {
          if (!data.tracking_id || !data.tracking_url) {
            return;
          }
          // Save the tracking id in the hidden field.
          $input.val(data.tracking_id);
          // Attach the tracking image.
          $('<img src="' + data.tracking_url + '" width="1" height="1" alt="" />').appendTo('body');
        }
      })
    });
  }
};

})(jQuery);
