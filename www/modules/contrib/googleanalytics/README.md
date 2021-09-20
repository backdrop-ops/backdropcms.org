Google Analytics
======================

Adds a Google Analytics JavaScript tracking code to every page.

Requirements
------------

* Google Analytics user account

Installation
------------

- Install this module using the official Backdrop CMS instructions at
  https://backdropcms.org/guide/modules

- Visit the configuration page under Administration > Configuration > System >
  Google Analytics (admin/config/system/googleanalytics) and enter your Google
  Analytics account number.

- All pages will now have the required JavaScript added to the HTML footer and
  you can confirm this by viewing the page source from your browser.


Advanced Settings
-----------------

You can include additional JavaScript snippets in the custom javascript code
text area. These can be found on the official Google Analytics pages and a few
examples at http://drupal.org/node/248699. Support is not provided for any
customizations you include.

To speed up page loading you may also cache the Google Analytics "analytics.js"
file locally.

Manual JS debugging
-------------------

For manual debugging of the JS code you are able to create a test node. This
is the example HTML code for this test node. You need to enable debugging mode
in your Drupal configuration of Google Analytics settings to see verbose
messages in your browsers JS console.

Title: Google Analytics test page

Body:
<ul>
  <li><a href="mailto:foo@example.com">Mailto</a></li>
  <li><a href="/files/test.txt">Download file</a></li>
  <li><a class="colorbox" href="#">Open colorbox</a></li>
  <li><a href="http://example.com/">External link</a></li>
  <li><a href="/go/test">Go link</a></li>
</ul>

Text format: Full HTML

Current Maintainers
-------------------

- Jen Lampton (https://github.com/jenlampton)
- Seeking additional maintainers

Credits
-------

- This module was originally written for Drupal by [Mike Carter](https://www.drupal.org/u/budda).
- The Drupal module has been maintained for many years by [Alexander Hass](https://www.drupal.org/u/hass).
- The port to Backdrop CMS was done by [Jen Lampton](https://github.com/jenlampton) and [Nate Haug](https://github.com/quicksketch).

License
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.
