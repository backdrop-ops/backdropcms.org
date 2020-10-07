Image Maximum Crop
==================

Image Maximum Crop provides an image effect that crops images only when they
exceed a given dimension.

If a large image is uploaded, it will be cropped to the specified dimensions. If
a small image is uploaded, it won't be affected. If an image is uploaded with
one side larger and one side smaller than the specified dimensions, the larger
side will be cropped while the smaller side will be unaffected.

Installation
------------

- Install this module using the official Backdrop CMS instructions at
  https://backdropcms.org/guide/modules.

- Visit the 'Image styles' page under Administration > Configuration > Media >
  Image styles (admin/config/media/image-styles), then add the 'Maximum crop'
  effect to the image style(s) of your choice.

Issues
------

Bugs and Feature requests should be reported in the Issue Queue:
https://github.com/backdrop-contrib/image_max_size_crop/issues.

Current Maintainers
-------------------

- [Peter Anderson](https://github.com/BWPanda)

Credits
-------

- Ported to Backdrop CMS by [Peter Anderson](https://github.com/BWPanda).
- Originally written for Drupal by
  [Tim Mallezie](https://www.drupal.org/u/mallezie).

License
-------

This project is GPL v2 software.
See the LICENSE.txt file in this directory for complete text.
