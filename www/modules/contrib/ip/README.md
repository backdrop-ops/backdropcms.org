IP Address Manager
======================

IP Address Manager records user IP addresses and allows admins to view which IP addresses have been tracked against a user, and which users have been known to use a certain IP address. This can be used to identify duplicate accounts and/or misbehaving users.

The aim of this module is to provide a way to identify a user when they are not logged in to provide some action or context to their presence by maintaining lists of users for some purpose - such as, but not limited to, banning them. It is intended that this functionality will be provided by other modules that will use the services provided by this one to identify users.

One such service is the ability to assign and record demerits against IP addresses that are used for spamming; this is provided by the `ip_demerits` sub-module that is part of this package.

Installation
------------

- Install this module using [the official Backdrop CMS instructions](https://backdropcms.org/guide/modules).

- Enable permissions at `admin/people/permissions`.  Users with the permission `'manage ip addresses'` will be able to see users' IP addresses.

Usage:

- Go to `user/%user/ip` to see this user's IP addresses.
- Go to `admin/people/ip` to see all users' IP addresses.

This module is integrated with Views so you can create your own Views and/or modify the Views provided with this module.

Optional: this module provides drush commands to insert IP address data loaded prior to module installation.

- `drush ip-backlog-nodes` —
  Imports nodes' IP data from watchdog (dblog must be installed).
- `drush ip-backlog-comments` —
  Imports comments' IP data from comment table.

New in version 2.1.0
--------------------

- The Views provided by this module now make a distinction between cancelled and anonymous users when displaying them in tables. New installations will use the new functionality. Upgrades from older versions are left unchanged, but you can modify the existing Views tables to use the new "User id-based..." fields that display this information.

- Beginning with this version, there is a sub-module, IP Demerits, that supports the assignment, removal, and recording of demerits against users and/or IP addresses. See the README file for the sub-module for further information.

Issues
------

Bugs and feature requests should be reported in [the Issue Queue](https://github.com/backdrop-contrib/ip/issues).

Current Maintainers
-------------------

- [Robert J. Lang](https://github.com/bugfolder)

Credits
-------

- Ported to Backdrop CMS by [Robert J. Lang](https://github.com/bugfolder).
- Originally written for Drupal by [GeduR](https://www.drupal.org/u/gedur).

License
-------

This project is GPL v2 software.
See the LICENSE.txt file in this directory for complete text.

