
(function ($) {

Backdrop.behaviors.xmlsitemapFieldsetSummaries = {
  attach: function (context) {
    $('fieldset#edit-xmlsitemap', context).backdropSetSummary(function (context) {
      var vals = [];

      // Inclusion select field.
      var status = $('#edit-xmlsitemap-status option:selected').text();
      vals.push(Backdrop.t('Inclusion: @value', { '@value': status }));

      // Priority select field.
      var priority = $('#edit-xmlsitemap-priority option:selected').text();
      vals.push(Backdrop.t('Priority: @value', { '@value': priority }));

      return vals.join('<br />');
    });
  }
};

})(jQuery);
