(function ($) {

"use strict";

Backdrop.googleanalytics = {};

$(document).ready(function() {

  // Attach mousedown, keyup, touchstart events to document only and catch
  // clicks on all elements.
  $(document.body).bind("mousedown keyup touchstart", function(event) {

    // Catch the closest surrounding link of a clicked element.
    $(event.target).closest("a,area").each(function() {

      // Is the clicked URL internal?
      if (Backdrop.googleanalytics.isInternal(this.href)) {
        // Skip 'click' tracking, if custom tracking events are bound.
        if ($(this).is('.colorbox') && (Backdrop.settings.googleanalytics.trackColorbox)) {
          // Do nothing here. The custom event will handle all tracking.
          //console.info("Click on .colorbox item has been detected.");
        }
        // Is download tracking activated and the file extension configured for download tracking?
        else if (Backdrop.settings.googleanalytics.trackDownload && Backdrop.googleanalytics.isDownload(this.href)) {
          // Download link clicked.
          gtag('event', Backdrop.googleanalytics.getDownloadExtension(this.href).toUpperCase(), {
            event_category: 'Downloads',
            event_label: Backdrop.googleanalytics.getPageUrl(this.href),
            transport_type: 'beacon'
          });
        }
        else if (Backdrop.googleanalytics.isInternalSpecial(this.href)) {
          // Keep the internal URL for Google Analytics website overlay intact.
          // @todo: May require tracking ID
          var target = this;
          $.each(backdropSettings.google_analytics.account, function () {
            gtag('config', this, {
              page_path: Backdrop.googleanalytics.getPageUrl(target.href),
              transport_type: 'beacon'
            });
          });
        }
      }
      else {
        if (Backdrop.settings.googleanalytics.trackMailto && $(this).is("a[href^='mailto:'],area[href^='mailto:']")) {
          // Mailto link clicked.
          gtag('event', 'Click', {
            event_category: 'Mails',
            event_label: this.href.substring(7),
            transport_type: 'beacon'
          });
        }
        else if (Backdrop.settings.googleanalytics.trackOutbound && this.href.match(/^\w+:\/\//i)) {
          if (Backdrop.settings.googleanalytics.trackDomainMode !== 2 || (Backdrop.settings.googleanalytics.trackDomainMode === 2 && !Backdrop.googleanalytics.isCrossDomain(this.hostname, Backdrop.settings.googleanalytics.trackCrossDomains))) {
            // External link clicked / No top-level cross domain clicked.
            gtag('event', 'Click', {
              event_category: 'Outbound links',
              event_label: this.href,
              transport_type: 'beacon'
            });
          }
        }
      }
    });
  });

  // Track hash changes as unique pageviews, if this option has been enabled.
  if (Backdrop.settings.googleanalytics.trackUrlFragments) {
    window.onhashchange = function() {
      $.each(backdropSettings.google_analytics.account, function () {
        gtag('config', this, {
          page_path: location.pathname + location.search + location.hash
        });
      });
    };
  }

  // Colorbox: This event triggers when the transition has completed and the
  // newly loaded content has been revealed.
  if (Backdrop.settings.googleanalytics.trackColorbox) {
    $(document).bind("cbox_complete", function () {
      var href = $.colorbox.element().attr("href");
      if (href) {
        $.each(backdropSettings.google_analytics.account, function () {
          gtag('config', this, {
            page_path: Backdrop.googleanalytics.getPageUrl(href)
          });
        });
      }
    });
  }

});

/**
 * Check whether the hostname is part of the cross domains or not.
 *
 * @param string hostname
 *   The hostname of the clicked URL.
 * @param array crossDomains
 *   All cross domain hostnames as JS array.
 *
 * @return boolean
 */
Backdrop.googleanalytics.isCrossDomain = function (hostname, crossDomains) {
  /**
   * jQuery < 1.6.3 bug: $.inArray crushes IE6 and Chrome if second argument is
   * `null` or `undefined`, https://bugs.jquery.com/ticket/10076,
   * https://github.com/jquery/jquery/commit/a839af034db2bd934e4d4fa6758a3fed8de74174
   *
   * @todo: Remove/Refactor in D8
   */
  if (!crossDomains) {
    return false;
  }
  else {
    return $.inArray(hostname, crossDomains) > -1 ? true : false;
  }
};

/**
 * Check whether this is a download URL or not.
 *
 * @param string url
 *   The web url to check.
 *
 * @return boolean
 */
Backdrop.googleanalytics.isDownload = function (url) {
  var isDownload = new RegExp("\\.(" + Backdrop.settings.googleanalytics.trackDownloadExtensions + ")([\?#].*)?$", "i");
  return isDownload.test(url);
};

/**
 * Check whether this is an absolute internal URL or not.
 *
 * @param string url
 *   The web url to check.
 *
 * @return boolean
 */
Backdrop.googleanalytics.isInternal = function (url) {
  var isInternal = new RegExp("^(https?):\/\/" + window.location.host, "i");
  return isInternal.test(url);
};

/**
 * Check whether this is a special URL or not.
 *
 * URL types:
 *  - gotwo.module /go/* links.
 *
 * @param string url
 *   The web url to check.
 *
 * @return boolean
 */
Backdrop.googleanalytics.isInternalSpecial = function (url) {
  var isInternalSpecial = new RegExp("(\/go\/.*)$", "i");
  return isInternalSpecial.test(url);
};

/**
 * Extract the relative internal URL from an absolute internal URL.
 *
 * Examples:
 * - https://mydomain.com/node/1 -> /node/1
 * - https://example.com/foo/bar -> https://example.com/foo/bar
 *
 * @param string url
 *   The web url to check.
 *
 * @return string
 *   Internal website URL
 */
Backdrop.googleanalytics.getPageUrl = function (url) {
  var extractInternalUrl = new RegExp("^(https?):\/\/" + window.location.host, "i");
  return url.replace(extractInternalUrl, '');
};

/**
 * Extract the download file extension from the URL.
 *
 * @param string url
 *   The web url to check.
 *
 * @return string
 *   The file extension of the passed url. e.g. "zip", "txt"
 */
Backdrop.googleanalytics.getDownloadExtension = function (url) {
  var extractDownloadextension = new RegExp("\\.(" + Backdrop.settings.googleanalytics.trackDownloadExtensions + ")([\?#].*)?$", "i");
  var extension = extractDownloadextension.exec(url);
  return (extension === null) ? '' : extension[1];
};

})(jQuery);
