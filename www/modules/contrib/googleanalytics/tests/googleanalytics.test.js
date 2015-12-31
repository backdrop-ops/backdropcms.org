(function ($) {

/**
 *  This file is for developers only.
 *
 *  This tests are made for the javascript functions used in GA module.
 *  These tests verify if the return values are properly working.
 *
 *  Hopefully this can be added somewhere else once Backdrop core has JavaScript
 *  unit testing integrated.
 */

"use strict";

Backdrop.googleanalytics.test = {};

Backdrop.googleanalytics.test.assertSame = function (value1, value2, message) {
  if (value1 === value2) {
    console.info(message);
  }
  else {
    console.error(message);
  }
};

Backdrop.googleanalytics.test.assertNotSame = function (value1, value2, message) {
  if (value1 !== value2) {
    console.info(message);
  }
  else {
    console.error(message);
  }
};

Backdrop.googleanalytics.test.assertTrue = function (value1, message) {
  if (value1 === true) {
    console.info(message);
  }
  else {
    console.error(message);
  }
};

Backdrop.googleanalytics.test.assertFalse = function (value1, message) {
  if (value1 === false) {
    console.info(message);
  }
  else {
    console.error(message);
  }
};

// Run after the documented is ready or Backdrop.settings is undefined.
$(document).ready(function() {

  /**
   *  Run javascript tests against the GA module.
   */

  // JavaScript debugging
  var base_url = window.location.protocol + '//' + window.location.host;
  var base_path = window.location.pathname;
  console.dir(Backdrop);

  console.group("Test 'isDownload':");
  Backdrop.googleanalytics.test.assertFalse(Backdrop.googleanalytics.isDownload(base_url + Backdrop.settings.basePath + 'node/8'), "Verify that '/node/8' url is not detected as file download.");
  Backdrop.googleanalytics.test.assertTrue(Backdrop.googleanalytics.isDownload(base_url + Backdrop.settings.basePath + 'files/foo1.zip'), "Verify that '/files/foo1.zip' url is detected as a file download.");
  Backdrop.googleanalytics.test.assertTrue(Backdrop.googleanalytics.isDownload(base_url + Backdrop.settings.basePath + 'files/foo1.zip#foo'), "Verify that '/files/foo1.zip#foo' url is detected as a file download.");
  Backdrop.googleanalytics.test.assertTrue(Backdrop.googleanalytics.isDownload(base_url + Backdrop.settings.basePath + 'files/foo1.zip?foo=bar'), "Verify that '/files/foo1.zip?foo=bar' url is detected as a file download.");
  Backdrop.googleanalytics.test.assertTrue(Backdrop.googleanalytics.isDownload(base_url + Backdrop.settings.basePath + 'files/foo1.zip?foo=bar#foo'), "Verify that '/files/foo1.zip?foo=bar#foo' url is detected as a file download.");
  Backdrop.googleanalytics.test.assertFalse(Backdrop.googleanalytics.isDownload(base_url + Backdrop.settings.basePath + 'files/foo2.ddd'), "Verify that '/files/foo2.ddd' url is not detected as file download.");
  console.groupEnd();

  console.group("Test 'isInternal':");
  Backdrop.googleanalytics.test.assertTrue(Backdrop.googleanalytics.isInternal(base_url + Backdrop.settings.basePath + 'node/1'), "Link '" + base_url + Backdrop.settings.basePath + "node/2' has been detected as internal link.");
  Backdrop.googleanalytics.test.assertTrue(Backdrop.googleanalytics.isInternal(base_url + Backdrop.settings.basePath + 'node/1#foo'), "Link '" + base_url + Backdrop.settings.basePath + "node/1#foo' has been detected as internal link.");
  Backdrop.googleanalytics.test.assertTrue(Backdrop.googleanalytics.isInternal(base_url + Backdrop.settings.basePath + 'node/1?foo=bar'), "Link '" + base_url + Backdrop.settings.basePath + "node/1?foo=bar' has been detected as internal link.");
  Backdrop.googleanalytics.test.assertTrue(Backdrop.googleanalytics.isInternal(base_url + Backdrop.settings.basePath + 'node/1?foo=bar#foo'), "Link '" + base_url + Backdrop.settings.basePath + "node/1?foo=bar#foo' has been detected as internal link.");
  Backdrop.googleanalytics.test.assertTrue(Backdrop.googleanalytics.isInternal(base_url + Backdrop.settings.basePath + 'go/foo'), "Link '" + base_url + Backdrop.settings.basePath + "go/foo' has been detected as internal link.");
  Backdrop.googleanalytics.test.assertFalse(Backdrop.googleanalytics.isInternal('http://example.com/node/3'), "Link 'http://example.com/node/3' has been detected as external link.");
  console.groupEnd();

  console.group("Test 'isInternalSpecial':");
  Backdrop.googleanalytics.test.assertTrue(Backdrop.googleanalytics.isInternalSpecial(base_url + Backdrop.settings.basePath + 'go/foo'), "Link '" + base_url + Backdrop.settings.basePath + "go/foo' has been detected as special internal link.");
  Backdrop.googleanalytics.test.assertFalse(Backdrop.googleanalytics.isInternalSpecial(base_url + Backdrop.settings.basePath + 'node/1'), "Link '" + base_url + Backdrop.settings.basePath + "node/1' has been detected as special internal link.");
  console.groupEnd();

  console.group("Test 'getPageUrl':");
  Backdrop.googleanalytics.test.assertSame(base_path, Backdrop.googleanalytics.getPageUrl(base_url + Backdrop.settings.basePath + 'node/1'), "Absolute internal URL '" +  Backdrop.settings.basePath + "node/1' has been extracted from full qualified url '" + base_url + base_path + "'.");
  Backdrop.googleanalytics.test.assertSame(base_path, Backdrop.googleanalytics.getPageUrl(Backdrop.settings.basePath + 'node/1'), "Absolute internal URL '" +  Backdrop.settings.basePath + "node/1' has been extracted from absolute url '" +  base_path + "'.");
  Backdrop.googleanalytics.test.assertSame('http://example.com/node/2', Backdrop.googleanalytics.getPageUrl('http://example.com/node/2'), "Full qualified external url 'http://example.com/node/2' has been extracted.");
  Backdrop.googleanalytics.test.assertSame('//example.com/node/2', Backdrop.googleanalytics.getPageUrl('//example.com/node/2'), "Full qualified external url '//example.com/node/2' has been extracted.");
  console.groupEnd();

  console.group("Test 'getDownloadExtension':");
  Backdrop.googleanalytics.test.assertSame('zip', Backdrop.googleanalytics.getDownloadExtension(base_url + Backdrop.settings.basePath + '/files/foo1.zip'), "Download extension 'zip' has been found in '" + base_url + Backdrop.settings.basePath + "files/foo1.zip'.");
  Backdrop.googleanalytics.test.assertSame('zip', Backdrop.googleanalytics.getDownloadExtension(base_url + Backdrop.settings.basePath + '/files/foo1.zip#foo'), "Download extension 'zip' has been found in '" + base_url + Backdrop.settings.basePath + "files/foo1.zip#foo'.");
  Backdrop.googleanalytics.test.assertSame('zip', Backdrop.googleanalytics.getDownloadExtension(base_url + Backdrop.settings.basePath + '/files/foo1.zip?foo=bar'), "Download extension 'zip' has been found in '" + base_url + Backdrop.settings.basePath + "files/foo1.zip?foo=bar'.");
  Backdrop.googleanalytics.test.assertSame('zip', Backdrop.googleanalytics.getDownloadExtension(base_url + Backdrop.settings.basePath + '/files/foo1.zip?foo=bar#foo'), "Download extension 'zip' has been found in '" + base_url + Backdrop.settings.basePath + "files/foo1.zip?foo=bar'.");
  Backdrop.googleanalytics.test.assertSame('', Backdrop.googleanalytics.getDownloadExtension(base_url + Backdrop.settings.basePath + '/files/foo2.dddd'), "No download extension found in '" + base_url + Backdrop.settings.basePath + "files/foo2.dddd'.");
  console.groupEnd();

  // List of top-level domains: example.com, example.net
  console.group("Test 'isCrossDomain' (requires cross domain configuration with 'example.com' and 'example.net'):");
  if (Backdrop.settings.googleanalytics.trackCrossDomains) {
    console.dir(Backdrop.settings.googleanalytics.trackCrossDomains);
    Backdrop.googleanalytics.test.assertTrue(Backdrop.googleanalytics.isCrossDomain('example.com', Backdrop.settings.googleanalytics.trackCrossDomains), "URL 'example.com' has been found in cross domain list.");
    Backdrop.googleanalytics.test.assertTrue(Backdrop.googleanalytics.isCrossDomain('example.net', Backdrop.settings.googleanalytics.trackCrossDomains), "URL 'example.com' has been found in cross domain list.");
    Backdrop.googleanalytics.test.assertFalse(Backdrop.googleanalytics.isCrossDomain('www.example.com', Backdrop.settings.googleanalytics.trackCrossDomains), "URL 'www.example.com' not found in cross domain list.");
    Backdrop.googleanalytics.test.assertFalse(Backdrop.googleanalytics.isCrossDomain('www.example.net', Backdrop.settings.googleanalytics.trackCrossDomains), "URL 'www.example.com' not found in cross domain list.");
  }
  else {
    console.warn('Cross domain tracking is not enabled. Tests skipped.');
  }
  console.groupEnd();

});

})(jQuery);
