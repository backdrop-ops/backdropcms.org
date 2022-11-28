IP Demerits
===========

IP Demerits is a sub-module of the IP Address Manager (`ip`) module that lets you assign demerit points to a user and/or IP address for various policy infractions, such as creating obvious spam or spam accounts.

Demerits can be assigned automatically as a result of blocking and/or deleting a user, if desired.


Installation
------------

- Install this module using [the official Backdrop CMS instructions](https://backdropcms.org/guide/modules).

- Enable permissions at admin/people/permissions.
    - Users with the permission `'manage ip addresses'` will be able to see users' IP addresses.
    - Users with the permission `manage ip demerits` will be able to set the definitions of demerits.

Usage:

- Go to `user/%user/ip` to see this user's IP addresses and any demerits they have received.
- Go to `admin/people/ip-demerits` to see all demerits and/or assign a demerit manually. This page has two additional sub-tabs:
  - User totals (`admin/people/ip-demerits/user-totals`) — display total demerit points by user
  - IP totals (`admin/people/ip-demerits/ip-totals`) — display total demerit points by IP address.

This module is integrated with Views so you can create your own Views and/or modify the built-in Views.

Status and plans
----------------

The current version only supports assigning demerits manually or automatically. In its current form, it is useful for seeing if you have repeat miscreants from the same IP address.

Future versions will integrate with the IP Blocking (`ip_blocking`) or the Ban IP (`ban_ip`) modules and can automatically block an IP address if the total points of demerits exceeds a given threshold.

Future versions may also integrate with Rules module to support more complicated demerit workflows.

Issues
------

Bugs and feature requests should be reported in [the issue queue for the IP Address Manager module](https://github.com/backdrop-contrib/ip/issues).

Current Maintainers
-------------------

- [Robert J. Lang](https://github.com/bugfolder)

Credits
-------

- Written for Backdrop CMS by [Robert J. Lang](https://github.com/bugfolder).

License
-------

This project is GPL v2 software.
See the LICENSE.txt file in this directory for complete text.

