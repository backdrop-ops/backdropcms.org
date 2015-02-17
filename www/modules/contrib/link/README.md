Link
====

Link module allows links to be added easily to any content types and profiles and include advanced validating and different ways of storing internal or external links and URLs. It also supports additional link text title, site wide tokens for titles and title attributes, target attributes, css class attribution, static repeating values, input conversion, and many more.

Note: Since some misleading user reports we need to clarify here - Link module is NOT about to add links to any menus or the navigation nor primary/secondary menu. This can be done with default menu module (part of Backdrop core). The Link module provides an additional custom field for storing and validating links to be added with any content type, which means another input block additional to your text-body, title, image and any other input you can make on new content creation.

Installation
------------

1. Install this module using the official Backdrop CMS instructions at
  https://backdropcms.org/guide/modules
2. Create or Edit a content-type and add a new field of type link: `admin/structure/types`.

Configuration
-------------

Configuration is only slightly more complicated than a text field. Link text titles for URLs can be made required, set as instead of URL, optional (default), or left out entirely. If no link text title is provided, the trimmed version of the complete URL will be displayed. The target attribute should be set to "_blank", "top", or left out completely (checkboxes provide info). The rel=nofollow attribute prevents the link from being followed by certain search engines. More info at Wikipedia.

Example
-------

If you were to create a field named 'My New Link', the default display of the link would be: <em><div class="field_my_new_link" target="[target_value]"><a href="[URL]">[Title]</a></div></em> where items between [] characters would be customized based on the user input.

The link module supports both, internal and external URLs. URLs are validated on input. Here are some examples of data input and the default view of a link:  http://backdropcms.org results in http://backdropcms.org, but backdropcms.org results in http://backdropcms.org, while <front> will convert into http://backdropcms.org and node/1 into http://backdropcms.org/handbook

Anchors and query strings may also be used in any of these cases, including: node/74971/edit?destination=node/74972<front>#pager

Theming and Output
------------------

Since link module is mainly a data storage field in a modular framework, the theming and output is up to the site builder and other additional modules such as views, etc for such needs.

License
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.

Current Maintainers
-------------------

This module is currently seeking maintainers.

Credits
-------

Ported to Backdrop by Herb v/d Dool (https://github.com/herbdool/)

This module was originally written for Drupal (https://drupal.org/project/link). Drupal maintainers are: [jcfiala](https://www.drupal.org/u/jcfiala), [diqidoq](https://www.drupal.org/u/diqidoq), [sun](https://www.drupal.org/u/sun), [dropcube](https://www.drupal.org/u/dropcube), [mrfelton](https://www.drupal.org/u/mrfelton).
