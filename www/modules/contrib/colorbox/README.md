COLORBOX
========

A light-weight, customizable lightbox plugin for jQuery 1.4.3+. Iframed or
inline images or content can be displayed in a popup or modal "lightbox" above
the current page.

The Colorbox module:

* Excellent integration with Image fields and Image styles
* Choose between a default style and 5 example styles that are included.
* Style the Colorbox with a custom colorbox.css file in your theme.

The Colorbox plugin:

* Supports photos, grouping, slideshow, ajax, inline, and iframed content.
* Appearance is controlled through CSS so it can be restyled.
* Preloads upcoming images in a photo group.
* Completely unobtrusive, options are set in the JS and require no changes to existing HTML.
* Released under the MIT License.

Colorbox - http://www.jacklmoore.com/colorbox


Installation
------------

- Install this module using the official Backdrop CMS instructions at
  https://backdropcms.org/user-guide/modules

- Configure your image fields or views to display images in colorboxes.

- (Optional) Visit the configuration page under Administration > Configuration >
  Media > Colorbox (admin/config/media/colorbox) and configure as necessary.


Differences from Drupal
-----------------------

- If you would like to use HTML tags in colorbox captions, you will need to enable
  the new setting `Allow HTML in Colorbox captions`. This configuration will use
  the DOMpurify library for sanitization. Please note: **The DOMpurify library
  has several dependencies with security issues.** It is unknown whether the
  colorbox use of DOMpurify could introduce a vulnerablity to your site. For
  this reason it is recommend to leave this setting disabled.
- The Backdrop version of Colorbox module does not require that you download or
  install the Color library separately.
- The Backdrop version of Colorbox module does not require that you download or
  install the DomPurify library separately.


Current Maintainers
-------------------

- jenlampton (https://github.com/jenlampton)
- Robert J. Lang (bugfolder) (https://github.com/bugfolder)
- Seeking additional maintainers


Credits
-------

- Ported to Backdrop by [Andy Martha](https://github.com/biolithic)
- Drupal module maintainer [Paul McKibben](https://www.drupal.org/u/paulmckibben)
- Drupal module maintainer [Fredrik Jonsson](https://drupal.org/user/5546)
- Drupal module maintainer [Sam Becker](https://www.drupal.org/u/sam152)
- Drupal module maintainer [Andrii Podanenko](https://www.drupal.org/u/podarok)
- Drupal module maintainer [Francisco José Cruz Romanos](https://www.drupal.org/u/grisendo)
- Drupal module maintainer [Joe Wheaton](https://www.drupal.org/u/jdwfly)
- The [Colorbox jQuery library](http://www.jacklmoore.com)


Licenses
--------

* This project is GPL v2 software. See the LICENSE.txt file in this directory
  for complete text.
* The Colorbox jQuery library is released under the [MIT License](https://opensource.org/licenses/mit-license.php).
* The DOMpurify library is released under [a dual license of Apache-2.0 and MPL-2.0](https://github.com/cure53/DOMPurify/blob/main/LICENSE).
