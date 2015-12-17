(function ($) {

"use strict";

/**
 * Provide the summary information for the tracking settings vertical tabs.
 */
Backdrop.behaviors.trackingSettingsSummary = {
  attach: function (context) {
    // Make sure this behavior is processed only if backdropSetSummary is defined.
    if (typeof jQuery.fn.backdropSetSummary == 'undefined') {
      return;
    }

    $('fieldset#edit-page-vis-settings', context).backdropSetSummary(function (context) {
      var $radio = $('input[name="googleanalytics_visibility_pages"]:checked', context);
      if ($radio.val() == 0) {
        if (!$('textarea[name="googleanalytics_pages"]', context).val()) {
          return Backdrop.t('Not restricted');
        }
        else {
          return Backdrop.t('All pages with exceptions');
        }
      }
      else {
        return Backdrop.t('Restricted to certain pages');
      }
    });

    $('fieldset#edit-role-vis-settings', context).backdropSetSummary(function (context) {
      var vals = [];
      $('input[type="checkbox"]:checked', context).each(function () {
        vals.push($.trim($(this).next('label').text()));
      });
      if (!vals.length) {
        return Backdrop.t('Not restricted');
      }
      else if ($('input[name="googleanalytics_visibility_roles"]:checked', context).val() == 1) {
        return Backdrop.t('Excepted: @roles', {'@roles' : vals.join(', ')});
      }
      else {
        return vals.join(', ');
      }
    });

    $('fieldset#edit-user-vis-settings', context).backdropSetSummary(function (context) {
      var $radio = $('input[name="googleanalytics_custom"]:checked', context);
      if ($radio.val() == 0) {
        return Backdrop.t('Not customizable');
      }
      else if ($radio.val() == 1) {
        return Backdrop.t('On by default with opt out');
      }
      else {
        return Backdrop.t('Off by default with opt in');
      }
    });

    $('fieldset#edit-linktracking', context).backdropSetSummary(function (context) {
      var vals = [];
      if ($('input#edit-googleanalytics-trackoutbound', context).is(':checked')) {
        vals.push(Backdrop.t('Outbound links'));
      }
      if ($('input#edit-googleanalytics-trackmailto', context).is(':checked')) {
        vals.push(Backdrop.t('Mailto links'));
      }
      if ($('input#edit-googleanalytics-trackfiles', context).is(':checked')) {
        vals.push(Backdrop.t('Downloads'));
      }
      if (!vals.length) {
        return Backdrop.t('Not tracked');
      }
      return Backdrop.t('@items enabled', {'@items' : vals.join(', ')});
    });

    $('fieldset#edit-messagetracking', context).backdropSetSummary(function (context) {
      var vals = [];
      $('input[type="checkbox"]:checked', context).each(function () {
        vals.push($.trim($(this).next('label').text()));
      });
      if (!vals.length) {
        return Backdrop.t('Not tracked');
      }
      return Backdrop.t('@items enabled', {'@items' : vals.join(', ')});
    });

    $('fieldset#edit-search-and-advertising', context).backdropSetSummary(function (context) {
      var vals = [];
      if ($('input#edit-googleanalytics-site-search', context).is(':checked')) {
        vals.push(Backdrop.t('Site search'));
      }
      if ($('input#edit-googleanalytics-trackadsense', context).is(':checked')) {
        vals.push(Backdrop.t('AdSense ads'));
      }
      if ($('input#edit-googleanalytics-trackdoubleclick', context).is(':checked')) {
        vals.push(Backdrop.t('Display features'));
      }
      if (!vals.length) {
        return Backdrop.t('Not tracked');
      }
      return Backdrop.t('@items enabled', {'@items' : vals.join(', ')});
    });

    $('fieldset#edit-domain-tracking', context).backdropSetSummary(function (context) {
      var $radio = $('input[name="googleanalytics_domain_mode"]:checked', context);
      if ($radio.val() == 0) {
        return Backdrop.t('A single domain');
      }
      else if ($radio.val() == 1) {
        return Backdrop.t('One domain with multiple subdomains');
      }
      else {
        return Backdrop.t('Multiple top-level domains');
      }
    });

    $('fieldset#edit-privacy', context).backdropSetSummary(function (context) {
      var vals = [];
      if ($('input#edit-googleanalytics-tracker-anonymizeip', context).is(':checked')) {
        vals.push(Backdrop.t('Anonymize IP'));
      }
      if ($('input#edit-googleanalytics-privacy-donottrack', context).is(':checked')) {
        vals.push(Backdrop.t('Universal web tracking opt-out'));
      }
      if (!vals.length) {
        return Backdrop.t('No privacy');
      }
      return Backdrop.t('@items enabled', {'@items' : vals.join(', ')});
    });
  }
};

})(jQuery);
