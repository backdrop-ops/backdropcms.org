Restrict Abusive Words
======================

The Restrict Abusive Words module restricts the use of specific words and
phrases when submitting various forms. The restriction can be applied on the
content creation form, the comment form, the user profile form, the user
registration form, and webforms.

Restrictions can be applied for specific roles. Restrictions can be applied only
to fields (text, textarea and long text type) and other basic fields like title,
body for node form, name, email for user registration form etc.

There are two types of restrictions: one will prevent the from being submitted,
and the other will allow the form to be saved but will unpublish the node, or
deactivate the user account.

Requirements
------------

None.

Installation
------------

- Install this module using the official Backdrop CMS instructions at
  https://backdropcms.org/guide/modules.

- Visit the configuration page under Administration > Configuration >
  Content authoring > Restrict Abusive Words
  (admin/config/content/restrict_abusive_words) and adjust the settings.

- Add abusive words under Administration > Configuration > Content authoring >
  Restrict Abusive Words > Add abusive words
  (admin/config/content/restrict_abusive_words/add) and enter abusive words.

- View the list of abusive words at Administration > Configuration >
  Content authoring > Restrict Abusive Words > List abusive words
  (admin/config/content/restrict_abusive_words/list).

Issues
------

Bugs and Feature requests should be reported in the Issue Queue:
https://github.com/backdrop-contrib/restrict_abusive_words/issues.

Current Maintainers
-------------------

- [Jen Lampton](https://github.com/jenlampton).
- [Robert J. Lang](https://github.com/bugfolder).

Credits
-------

- Ported to Backdrop CMS by [Jen Lampton](https://github.com/jenlampton).
- Originally written for Drupal by [Biswajit Mondal](https://www.drupal.org/user/2698531).

License
-------

This project is GPL v2 software.
See the LICENSE.txt file in this directory for complete text.
