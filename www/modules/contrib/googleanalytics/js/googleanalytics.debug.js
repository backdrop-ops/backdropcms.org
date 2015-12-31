(function ($) {

"use strict";

Backdrop.googleanalytics = {};

$(document).ready(function() {

  // Attach mousedown, keyup, touchstart events to document only and catch
  // clicks on all elements.
  $(document.body).bind("mousedown keyup touchstart", function(event) {
    console.group("Running Google Analytics for Backdrop.");
    console.info(event);

    // Catch the closest surrounding link of a clicked element.
    $(event.target).closest("a,area").each(function() {
      console.info("Element '%o' has been detected. Link '%s' found.", this, this.href);

      // Is the clicked URL internal?
      if (Backdrop.googleanalytics.isInternal(this.href)) {
        // Skip 'click' tracking, if custom tracking events are bound.
        if ($(this).is('.colorbox')) {
          // Do nothing here. The custom event will handle all tracking.
          console.info("Click on .colorbox item has been detected.");
        }
        // Is download tracking activated and the file extension configured for download tracking?
        else if (Backdrop.settings.googleanalytics.trackDownload && Backdrop.googleanalytics.isDownload(this.href)) {
          // Download link clicked.
          console.info("Download url '%s' has been found. Tracked download as extension '%s'.", Backdrop.googleanalytics.getPageUrl(this.href), Backdrop.googleanalytics.getDownloadExtension(this.href).toUpperCase());
          ga("send", "event", "Downloads", Backdrop.googleanalytics.getDownloadExtension(this.href).toUpperCase(), Backdrop.googleanalytics.getPageUrl(this.href));
        }
        else if (Backdrop.googleanalytics.isInternalSpecial(this.href)) {
          // Keep the internal URL for Google Analytics website overlay intact.
          console.info("Click on internal special link '%s' has been tracked.", Backdrop.googleanalytics.getPageUrl(this.href));
          ga("send", "pageview", { "page": Backdrop.googleanalytics.getPageUrl(this.href) });
        }
        else {
          // e.g. anchor in same page or other internal page link
          console.info("Click on internal link '%s' detected, but not tracked by click.", this.href);
        }
      }
      else {
        if (Backdrop.settings.googleanalytics.trackMailto && $(this).is("a[href^='mailto:'],area[href^='mailto:']")) {
          // Mailto link clicked.
          console.info("Click on e-mail '%s' has been tracked.", this.href.substring(7));
          ga("send", "event", "Mails", "Click", this.href.substring(7));
        }
        else if (Backdrop.settings.googleanalytics.trackOutbound && this.href.match(/^\w+:\/\//i)) {
          if (Backdrop.settings.googleanalytics.trackDomainMode != 2 || (Backdrop.settings.googleanalytics.trackDomainMode == 2 && !Backdrop.googleanalytics.isCrossDomain(this.hostname, Backdrop.settings.googleanalytics.trackCrossDomains))) {
            // External link clicked / No top-level cross domain clicked.
            console.info("Outbound link '%s' has been tracked.", this.href);
            ga("send", "event", "Outbound links", "Click", this.href);
          }
          else {
            console.info("Internal link '%s' clicked, not tracked.", this.href);
          }
        }
      }
    });

    console.groupEnd();
  });

  // Track hash changes as unique pageviews, if this option has been enabled.
  if (Backdrop.settings.googleanalytics.trackUrlFragments) {
    window.onhashchange = function() {
      console.info("Track URL '%s' as pageview. Hash '%s' has changed.", location.pathname + location.search + location.hash, location.hash);
      ga('send', 'pageview', location.pathname + location.search + location.hash);
    }
  }

  // Colorbox: This event triggers when the transition has completed and the
  // newly loaded content has been revealed.
  $(document).bind("cbox_complete", function () {
    var href = $.colorbox.element().attr("href");
    if (href) {
      console.info("Colorbox transition to url '%s' has been tracked.", Backdrop.googleanalytics.getPageUrl(href));
      ga("send", "pageview", { "page": Backdrop.googleanalytics.getPageUrl(href) });
    }
  });

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
   * `null` or `undefined`, http://bugs.jquery.com/ticket/10076,
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
 * - http://mydomain.com/node/1 -> /node/1
 * - http://example.com/foo/bar -> http://example.com/foo/bar
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
