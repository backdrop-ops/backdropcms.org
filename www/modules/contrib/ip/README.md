IP Address Manager
======================

IP Address Manager records user IP addresses and allows admins to view which IP addresses have been tracked against a user, and which users have been known to use a certain IP address. This can be used to identify duplicate accounts.

The aim of this module is to provide a way to identify a user when they are not logged in to provide some action or context to their presence by maintaining lists of users for some purpose - such as, but not limited to, banning them. It is intended that this functionality will be provided by other modules that will use the services provided by this one to identify users.

Installation
------------

- Install this module using [the official Backdrop CMS instructions](https://backdropcms.org/guide/modules).

- Enable permissions at admin/people/permissions.  Users with the permission `'manage ip addresses'` will be able to see users' IP addresses.

Usage:

- Go to /user/%user/ip to see this user's IPs.
- Go to /admin/people/ip to see all users' IPs.

This module is integrated with Views so you can create your own Views.

Optional: this module provides drush commands to insert IP address data loaded prior to module installation.

- `drush ip-backlog-nodes` —
  Imports nodes' IP data from watchdog (dblog must be installed).
- `drush ip-backlog-comments` —
  Imports comments' IP data from comment table.

Documentation
-------------

Additional documentation may be located in [the Wiki](https://github.com/backdrop-contrib/ip/wiki/Documentation).

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

