Entity Plus
=================

This module wraps in a variety of entity-related functionality from various
sources, including:

 - The `Entity Metadata Wrapper` module from Drupal 7.
 - Various bits from the `Entity API` module from Drupal 7 which have not (yet)
   been merged into core. Note that this module renames several functions from
   the `entity_xxx()` format to `entity_plus_xxx()` format to prevent conflict
   in case some of these functions are eventually merged into core.

This is an API module. You only need to enable it if a module depends on it or
you are interested in using it for development.

Installation and Usage:
---------------
- Install this module using the [official Backdrop CMS instructions](https://backdropcms.org/guide/modules)
- Usage instructions can be [viewed and edited in the Wiki](https://github.com/backdrop-contrib/entity_plus/wiki).
- The [Basic Entity Plus Example](https://github.com/backdrop-contrib/basic_entity_plus_example)
  module provides an example of a custom entity using Entity Plus and Entity UI.

Current Maintainers
---------------

- [Laryn Kragt Bakker](https://github.com/laryn)
- [Joseph Flatt](https://github.com/hosef)
- [Alejandro Cremaschi](https://github.com/argiepiano)
- Seeking co-maintainers

Credits
---------------

- Ported to Backdrop by [docwilmot](https://github.com/docwilmot)
- Original Drupal version by [Wolfgang Ziegler](https://www.drupal.org/user/16747)

License
---------------

This project is GPL v2 software. See the LICENSE.txt file in this directory
for complete text.
