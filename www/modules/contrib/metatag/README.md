Metatag
=======

Provide structured metadata, aka "meta tags", for pages on your site.

In the context of search engine optimization, providing an extensive set of
meta tags may help improve your search ranking. A better search ranking will
result in a more prominent display of your content within search results.
Additionally, meta tags can be used to control the display of content as it is
shared across social networks. The Open Graph submodule in particular, will help
with Facebook, Pinterest, LinkedIn, etc (see below).


Pre-Release Information
-----------------------

This is a pre-release of the meta-tag module. We are currently seeking testers,
feedback, and direction on which submodules to work on next.

The following sub-modules have been ported to Backdrop CMS in this release:
* metatag
* metatag devel
* metatag verification

All 1.x-0.17.x tags will be used to denote pre-release status. The first
official release will be numbered 1.x-1.17.0.


Installation
------------

- Install this module using the official Backdrop CMS instructions at
  https://backdropcms.org/guide/modules

- On the Permissions administration page ("Administer >> Configuration
  >> User accounts >> Permissions" or admin/config/people/permissions)
  you need to assign:

   - The "Administer meta tags" permission to the roles that are allowed to
     access the meta tags admin pages to control the site defaults.

   - The "Edit meta tags" permission to the roles that are allowed to change
     meta tags on each individual page (node, term, etc).

- The main administrative page ("Administer >> Configuration >> Search and
  metadata >> Metatag" or admin/config/metadata/metatags) controls the site-wide
  defaults. Global settings, Home page settings, and specific defaults for
  different kinds of content are defined here.

- The list of configured sets of metatags may be controlled from the Settings
  page ("Administer >> Configuration >> Search and metadata >> Settings" or
  admin/config/metadata/metatags/settings)

- In order to provide a specific configuration per content type, vocabulary,
  etc, click "Add default meta tags".

- Each supported item will have a set of meta tag fields available for
  customization on the respective edit page. These fields will inherit their
  default values from the defaults assigned in #2 above. Any values that are
  not overridden will automatically update should the defaults be updated.

- Meta tags are often generated using Tokens. It may be necessary to customize
  the display of tokens for this purpose. To customize the display of tokens:

  - Go to a "Mange display" defualt interface, for example:
   admin/structure/types/manage/article/display,

  - In the "Custom Display Settings" section, ensure that "Tokens" is checked
    (save the form if necessary).

  - To customize the tokens, go to the "Mange display" interface for tokens,
    for example: admin/structure/types/manage/article/display/token


Documentation
-------------

Additional documentation is located in the Wiki:
https://github.com/backdrop-contrib/metatag/wiki/Documentation


Issues
------

Bugs and Feature requests should be reported in the Issue Queue:
https://github.com/backdrop-contrib/metatag/issues


API
---

Full API documentation is available in metatag.api.php.

It is not necessary to control Metatag via the entity API, any entity that has
view modes defined and is not a configuration entity is automatically suitable
for use.

The meta tags for a given entity object (node, etc) can be obtained as follows:
  $metatags = metatags_get_entity_metatags($entity_id, $entity_type, $langcode);
The result will be a nested array of meta tag structures ready for either output
via drupal_render(), or examining to identify the actual text values.


Current Maintainers
-------------------

- Jen Lampton (https://github.com/jenlampton).
- Seeking additional maintainers.


Credits
-------

- Ported to Backdrop CMS by Jen Lampton (https://github.com/jenlampton).
- Maintained for Drupal by Damien McKenna (https://www.drupal.org/u/damienmckenna).
- Maintained for Drupal by Dave Reid (https://github.com/davereid).
- Initial Drupal development by Dave Reid (https://github.com/davereid).
- Ongoing Drupal development sponsored by Mediacurrent
- Ongoing Drupal development sponsored by Lullabot
- Initial Drupal development sponsored by Acquia
- Initial Drupal development sponsored by Palantir.net


License
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.
