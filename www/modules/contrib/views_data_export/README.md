Views Data Export
======================

This module is designed to provide a way to export large amounts of data from
views. It provides a display plugin that can rendered progressively in a batch.
Style plugins are include that support exporting in the following types:

* CSV
* Microsoft XLS
* Microsoft Doc
* Basic txt
* Simple xml


Installation
------------

- Install this module using the official Backdrop CMS instructions at
  https://backdropcms.org/guide/modules

- Add a new "Data export" display to your view.

- Change its "Style" to the desired export type. e.g. "CSV file".

- Configure the options (such as name, quote, etc.). You can go back and do
   this at any time by clicking the gear icon next to the style plugin you just
   selected.

- Give it a path in the Feed settings such as "path/to/view/csv".

- Optionally, you can choose to attach this to another of your displays by
   updating the "Attach to:" option in feed settings.

Documentation
-------------

Additional documentation is located in the Wiki:

`https://github.com/backdrop-contrib/views_data_export/wiki/Documentation`

Issues
------

Bugs and Feature requests should be reported in the Issue Queue:
https://github.com/backdrop-contrib/views_data_export/issues

Current Maintainers
-------------------

- John Franklin (https://github.com/jlfranklin)

Credits
-------

- Ported to Backdrop CMS by John Franklin (https://github.com/jlfranklin).
- Originally written for Drupal by Steven Jones (https://www.drupal.org/u/steven-jones).


License
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.
