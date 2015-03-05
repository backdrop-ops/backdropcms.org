Honeypot
--------

Honeypot uses both the honeypot and timestamp methods of deterring spam bots from completing forms on your Backdrop site. These methods are effective against many spam bots, and are not as intrusive as CAPTCHAs or other methods which punish the user.

The module currently supports enabling for all forms on the site, or particular forms like user registration or password reset forms, webforms, contact forms, node forms, and comment forms.

Installation
------------

To install this module, place it in your modules folder and enable it on the modules page.


Configuration
-------------

All settings for this module are on the Honeypot configuration page, under the Configuration section, in the Content authoring settings. You can visit the configuration page directly at admin/config/content/honeypot.

Note that, when testing Honeypot on your website, make sure you're not logged in as an administrative user or user 1; Honeypot allows administrative users to bypass Honeypot protection, so by default, Honeypot will not be added to forms accessed by site administrators.

Use in Your Own Forms
---------------------

If you want to add honeypot to your own forms, or to any form through your own module's hook_form_alter's, you can simply place the following function call inside your form builder function (or inside a hook_form_alter):

```
honeypot_add_form_protection($form, $form_state, array('honeypot', 'time_restriction'));
```

Note that you can enable or disable either the honeypot field, or the time restriction on the form by including or not including the option in the array.

License
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for complete text.

Current Maintainers
-------------------

This module is currently seeking maintainers.

Credits
-------

Ported to Backdrop by Herb v/d Dool (https://github.com/herbdool/)

The Honeypot module was originally developed by [Jeff Geerling](https://www.drupal.org/u/geerlingguy) of Midwestern Mac,
LLC (midwesternmac.com) for Drupal (https://drupal.org/project/honeypot), and sponsored by flockNote (flocknote.com).
