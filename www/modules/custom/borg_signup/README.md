BackdropCMS.org Signup Block
======================

This module provides a Newsletter signup block that can be placed on any *.backdropcms.org to sign up an email address for the Newsletter.

When you click/tap the "Signup" button in the block, the module communicates with BackdropCMS.org to add the entered email address to the "Newsletter" CiviCRM group on BackdropCMS.org, creating a new CiviCRM contact if necessary.

Installation
------------

- Install this module on BackdropCMS.org and on the other *.backdropcms.org sites.

- In the settings.php or settings.local.php file for all sites, add the following code:

```
define('BORG_SIGNUP_KEY', 'SOME_UNIQUE_RANDOM_KEY_OF_YOUR_CHOICE');
define('BORG_SIGNUP_URL', 'https://backdropcms.org/borg-signup');
```

You should create a random key for `BORG_SIGNUP_KEY`, then use the same key and the same `BORG_SIGNUP_URL` on all sites.

The key is a secret key; it should not be put in any file that is part of a public code repository.

The URL is the endpoint to which signup communication requests are submitted.

Issues
------

Bugs and feature requests should be reported in the [BackdropCMS.org issue queue](https://github.com/backdrop-ops/backdropcms.org/issues).

Current Maintainers
-------------------

- [Robert J. Lang](https://github.com/bugfolder).

Additional maintainers are welcome.

Credits
-------

- Created for Backdrop CMS by [Robert J. Lang](https://github.com/bugfolder).

License
-------

This project is GPL v2 software.
See the LICENSE.txt file in the Backdrop root directory for complete text.
