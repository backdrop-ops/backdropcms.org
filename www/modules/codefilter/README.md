Code Filter
===========

This is a simple filter module. It handles `<code></code>` and `<?php ?>` tags
so that users can post code without having to worry about escaping with `&lt;`
and `&gt;`


INSTALLATION
------------

- Install this module using the official Backdrop CMS instructions at
  https://backdropcms.org/guide/modules

- Go to Configuration > Content authoring > Text formats
  (admin/config/content/formats). For each format on which you wish to add Code
  Filter:

  1. Click the "Configure" link.

  2. Under "Enabled filters", check the codefilter checkbox.

  3. Under "Filter processing order", rearrange the filtering chain to resolve
     any conflicts. For example, to prevent invalid XHTML in the form of
     `<p><div class="codefilter">` make sure "Code filter" comes before the
     "Convert line breaks into HTML" filter. "Code filter" should also come
     after "Limit allowed HTML tags", to ensure it can output the needed tags
     and inline styles for syntax highlighting.

  4. Click the "Save configuration" button.

- (optionally) Edit your theme to provide a div.codeblock style for blocks of
  code.

License
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.

Current Maintainers
-------------------

This module is currently seeking maintainers.

Credits
-------

Ported to Backdrop by Nate Haug (https://github.com/quicksketch/)

This module was originally written for Drupal by Steven Wittens
(http://acko.net/), based on the PHP filter in project.module by Kjartan Mannes
(https://www.drupal.org/u/kjartan).
