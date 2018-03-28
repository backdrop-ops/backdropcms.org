SUMMARY - YouTube Field
========================
The YouTube field module provides a simple field that allows you to add a
YouTube video to a content type, user, or any entity.

Display types include:

 * YouTube videos of various sizes and options.
 * YouTube thumbnails with image styles.


Requirements
-------------
All dependencies of this module are enabled by default in Backdrop 1.x.


Installation
------------
Install this module using the official Backdrop CMS instructions at
https://backdropcms.org/guide/modules


Usage
-------
To use this module create a new field of type 'YouTube video'. This field will
accept YouTube URLs of the following formats:

 * http://youtube.com/watch?v=[video_id]
 * http://youtu.be/[video_id]
 * http://youtube.com/v/[video_id]
 * http://youtube.com/embed/[video_id]
 * http://youtube.com/?v=[video_id]

All formats listed above can also be provided without 'http://', with 'www.',
or with 'https://' rather than 'http://'. The last format can be provided with
additional parameters (ignored) and v does not have to be the first parameter.

To enable Colorbox support, enable the YouTube Field Colorbox module included in
this directory and consult its README file.


Configuration
--------------
Global module settings can be found at admin/config/media/youtube.

The video output of a YouTube field can be manipulated in three ways:
 * global parameters found on the configuration page mentioned above
 * field-specific parameters found in that particular field's display settings
 * Views settings for the specific field

The thumbnail of the YouTube image can also be used and can link to either the
content, the video on YouTube, or nothing at all.

To configure the field settings:

 1. click 'manage display' on the listing of entity types
 2. click the configuration gear to the right of the YouTube field


License
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.


Current Maintainers
-------------------

- Paul Davis (https://github.com/hellomrcat)
- Jen Lampton (https://github.com/jenlampton)
- Daniel Rose (https://github.com/danielrose28)


Credits
-------

This module was originally written for Drupal by Jen Lampton
(https://github.com/jenlampton).
