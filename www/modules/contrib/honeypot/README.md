Honeypot
========

Honeypot uses both the honeypot and timestamp methods of deterring spam bots
from completing forms on your Backdrop site. These methods are effective against
many spam bots, and are not as intrusive as CAPTCHAs or other methods which
punish the user.

The module currently supports enabling for all forms on the site, or particular
forms like user registration or password reset forms, webforms, contact forms,
node forms, and comment forms.

Installation
------------

- Install this module using the official Backdrop CMS instructions at
  https://backdropcms.org/guide/modules

- Visit the configuration page under Administration > Configuration >
  Content authoring (admin/config/content/honeypot) and enter the required
  information.

When testing Honeypot on your website, make sure you're not logged in as an
administrative user or user 1. Honeypot allows administrative users to bypass
Honeypot protection.  By default, Honeypot will not be added to forms accessed
by site administrators.

Configuration
-------------

All settings for this module are on the Honeypot configuration page, under the
Configuration section, in the Content authoring settings. You can visit the
configuration page directly at admin/config/content/honeypot.

Note that, when testing Honeypot on your website, make sure you're not logged in
as an administrative user or user 1; Honeypot allows administrative users to
bypass Honeypot protection, so by default, Honeypot will not be added to forms
accessed by site administrators.

Use in Your Own Forms
---------------------

If you want to add honeypot to your own forms, or to any form, you can use this hook:

```
function hook_honeypot_protect_forms_info() {
  return array(
    'my_form_id',
    'my_other_form_id',
  );
}
```

Note that you can enable or disable either the honeypot field, or the time
restriction on the form by including or not including the option in the array.

Documentation
-------------

Additional documentation is located in the Wiki:
https://github.com/backdrop-contrib/honeypot/wiki/Documentation

Issues
------

Bugs and Feature requests should be reported in the Issue Queue:
https://github.com/backdrop-contrib/honeypot/issues

Current Maintainers
-------------------

- Herb v/d Dool (https://github.com/herbdool/)
- Seeking additional maintainers.

Credits
-------

- Ported to Backdrop by Herb v/d Dool (https://github.com/herbdool/)
- Originally developed for Drupal by [Jeff Geerling](https://www.drupal.org/u/geerlingguy)
  of [Midwestern Mac, LLC](midwesternmac.com)
- sponsored by [flockNote](flocknote.com).

License
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.
