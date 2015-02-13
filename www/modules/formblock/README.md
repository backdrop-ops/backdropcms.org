Form Block
======================

Enables the presentation of user registration, site wide contact, or node creation forms in blocks.

Installation
------------

- Install this module using the official Backdrop CMS instructions at
  https://backdropcms.org/guide/modules

**To enable a node add form for a specific content type:**

- Go to the administration page for the content type under Administration >
  Structure > Content Types > Foo (admin/structure/types/foo).

- Go to the 'Form block' section (vertical tab) of the page.

- Check the box "[] Enable data entry from a block". This will 'make the entry
form for this content type available as a block.'

- Choose whether you want to '[] Show submission guidelines'.

- Save the settings by clicking the 'Save content type' button.

- Visit the layout administration page under Administration > Structure >
  Layouts > Foo (admin/structure/layouts/manage/default) to enable and place the
  block containing the form. Forms are named: "TYPE form block".

- Note that only users with permission to create the given content type will see
  the form. Adjust user permissions accordingly.

**To enable a block for user registration or change of password:**

- Visit the layout administration page under Administration > Structure >
  Layouts > Foo (admin/structure/layouts/manage/default) to enable and place the
  block containing the form. Forms are named: "User registration form" and
  "Request new password form".

- Note that the user registration form will appear only to non-logged in users
  and only if the site is configured to allow user registration.

- Note that the change password form will appear only to logged in users and
  only if the user has the permission to change password.

- Adjust settings accordingly.

**To enable a site-wide contact block:**

- Enable the Contact module.

- Visit the layout administration page under Administration > Structure >
  Layouts > Foo (admin/structure/layouts/manage/default) to enable and place the
  block containing the "Site-wide contact form".

- Note that the form will appear only to users with the permission to use the
  site-wide contact form. Adjust permissions accordingly.

License
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.

Current Maintainers
-------------------

- Jen Lampton (https://github.com/jenlampton)
- (seeking additional maintainers)

Credits
-------

This module was originally written for Drupal, previous maintainers include:
* Michael Prasuhn (https://www.drupal.org/u/mikey_p)
* Derek Wright (https://www.drupal.org/u/dww).
* Nedjo Rogers (https://www.drupal.org/u/nedjo)
