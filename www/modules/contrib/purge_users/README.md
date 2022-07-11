Auto Purge Users
======================

Auto Purge Users lets administrators delete inactive user accounts based on
various time conditions. User accounts are selected for purging according to
criteria that check for different types of inactivity and/or blockage. Some of
the criteria that are used to select users are:

* Those who exceed a configured period of inactivity;
* Those who have not activated their account since registration;
* Those who have not logged in for a long period.
* Those who have been blocked.

The user accounts that are purged can be notified that their account has been
deleted. Optionally you can limit the purge to user accounts with specific
roles.

User accounts can be deleted automatically on cron by enabling the auto purge
option on the configuration page or can be deleted manually by pressing the
"Purge user accounts now" button. Users deleted during cron are logged in the
watchdog log.

Installation
------------

- Install this module using [the official Backdrop CMS instructions](https://backdropcms.org/guide/modules).

- Visit the configuration page at Administer > Configuration > User accounts >
Auto purge users (admin/config/people/purge-rule) to configure the duration of
account inactivity, status of accounts to purge, and to filter the users by
their roles.

Differences from Drupal 7
-------------------------

* There are some changes to wording and presentation in the documentation and on
the configuration page.

* The Drupal version had a bug in handling the "Authenticated" role limitation,
which is fixed in the Backdrop module.

Issues
------

Bugs and feature requests should be reported in [the Issue Queue](https://github.com/backdrop-contrib/purge_users/issues).

Current Maintainers
-------------------

- [Robert J. Lang](https://github.com/bugfolder)

Credits
-------

- Ported to Backdrop CMS by [Robert J. Lang](https://github.com/bugfolder).
- Originally written for Drupal by [jim.applebee](https://www.drupal.org/u/jimapplebee) and [Andras Szilagyi](https://www.drupal.org/u/andras_szilagyi).

License
-------

This project is GPL v2 software.
See the LICENSE.txt file in this directory for complete text.
