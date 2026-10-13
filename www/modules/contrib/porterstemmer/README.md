Porter-Stemmer
==============

This module implements the Porter-Stemmer algorithm, version 2, to improve
English-language searching with the Drupal built-in Search module. Information
about the algorithm can be found at
http://snowball.tartarus.org/algorithms/english/stemmer.html

Stemming reduces a word to its basic root or stem (e.g. 'blogging' to 'blog')
so that variations on a word ('blogs', 'blogger', 'blogging', 'blog') are
considered equivalent when searching. This generally results in more relevant
results.

Note that a few parts of the Porter Stemmer algorithm work better for American
English than British English, so some British spellings will not be stemmed
correctly.

This module will use the PECL "stem" library's implementation of the Porter
Stemmer algorithm, if it is installed on your server. If the PECL "stem" library
is not available, the module uses its own PHP implementation of the
algorithm. The output is the same in either case. More information about the
PECL "stem" library: http://pecl.php.net/package/stem


Installation
------------

- Install this module using the official Backdrop CMS instructions at
  https://backdropcms.org/guide/modules.

- Visit the configuration page under Administration > Configuration > Search >
  Search settings (admin/config/search/search) and click the button for
  'Invalidate Search Index'.

  Note: You should do this step whenever you upgrade to a new version of the
  Porter Stemmer module, so that the search index is rebuilt with any changes
  to the stemming algorithm.

- Ensure that cron has run at least once, to build the search index. On larger
  sites, it may take several cron runs to complete the search index. You can
  check progress on the Search settings page, and you can run cron manually by
  visiting 'Administer > Reports > Status report', and clicking on
  'Run cron manually'.

- This module will use the PECL "stem" library for PHP, if it is installed
  on your server. If you have full administrative/root access to your server,
  and are comfortable with Apache/PHP configuration and system administration,
  you can install this library by following the instructions at:
  http://us3.php.net/manual/en/install.pecl.php

  Once the library is installed, you will need to add a line to your php.ini
  file to enable the module:
    extension=stem.so
  If the PECL "stem" library is not available, the module uses a PHP
  implementation of the stemming algorithm. The output is identical.

  More information about the PECL "stem" library:
  http://pecl.php.net/package/stem


Documentation
-------------

Additional documentation is located in the Wiki:
https://github.com/backdrop-contrib/porterstemmer/wiki/Documentation.


Issues
------

Bugs and Feature requests should be reported in the Issue Queue:
https://github.com/backdrop-contrib/porterstemmer/issues.


Current Maintainers
-------------------

- Jen Lampton (https://github.com/jenlampton).
- Seeking additional maintainers.


Credits
-------

- Ported to Backdrop CMS by [Jennifer Hodgdon](https://www.drupal.org/u/jhodgdon).
- Originally written for Drupal by [Jennifer Hodgdon](https://www.drupal.org/u/jhodgdon).


License
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.
