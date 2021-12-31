Token Filter
============

This is a simple module to make Backdrop native token values available in
text areas via an input filter.

Installation
------------

- Install this module using [the official Backdrop CMS instructions](https://backdropcms.org/guide/modules).

- Visit the relevant text format configuration pages under Administration >
  Configuration > Content authoring > Text editors and formats and enable the
  "Replace tokens" filter.

Usage
-----

You can use tokens in any text field for which the format uses the "Replace tokens" filter. All global tokens (tokens that need no context) should just work. In addition, the following token types that require context should also work:

* Content (e.g., `[node:?]`)
* Users (e.g., `[user:?]`)

If you encounter a usage where a token doesn't work but you think it should, please file a feature request in [the Issue Queue](https://github.com/backdrop-contrib/token_filter/issues).

Differences from Drupal 7
-------------------------

In Drupal 7, the Token module provided a function `token_entity_mapping()`, which supported an undocumented hook `hook_token_entity_mapping_alter()` that could be implemented by other modules. In Backdrop, Token support is in core and there is no `hook_token_entity_mapping_alter()`.

Issues
------

Bugs and feature requests should be reported in the [Issue Queue](https://github.com/backdrop-contrib/token_filter/issues).

Current Maintainer
------------------

- [Robert J. Lang](https://github.com/bugfolder/)

Credits
-------

- Ported to Backdrop CMS by [Jerry Hudgins](https://github.com/jerry-hudgins/).
- Originally written for Drupal by [Anton de Wet](https://github.com/asciikewl/).

License
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.
