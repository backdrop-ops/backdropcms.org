Ban IP
======================

Block site vistors by IP address.

Installation
------------

- Install this module using the official Backdrop CMS instructions at
https://backdropcms.org/guide/modules

To ban an IP address:
- Navigate to /admin/config/people/ip-blocking.
- Enter an IP address (for example, 10.0.0.1).
- Click Add.

* Note Backdrop will prevent you from banning your own IP address.

To remove the ban from an IP address:
- Navigate to the Ban IP page (see above).
- Beside an IP address, click Delete.


IP blocking via `settings.php`
------------------------------

To bypass database queries for denied IP addresses, use this setting.

Backdrop queries the {blocked_ips} table by default on every page request
for both authenticated and anonymous users. This allows the system to
block IP addresses from within the administrative interface and before any
modules are loaded. However on high traffic websites you may want to avoid
this query, allowing you to bypass database access altogether for anonymous
users under certain caching configurations.

To enable this setting, create an entry in `settings.php` like this:
```
  $settings['blocked_ips'] = array(
    'a.b.c.d',
  );
```

An empty array will have the effect of disabling IP blocking on your site.

If using this setting, you will need to add back any IP addresses which
you may have blocked via the administrative interface. Each element of this
array represents a blocked IP address.

Related Modules
---------------

The [IP Address Blocking](https://backdropcms.org/project/ip_blocking) module
provides similar functionality to this module, along with a number of
improvements. Both modules make use of the `blocked_ips` table inherited from
Drupal 7 core. However, IP Address Blocking adds some (useful) columns to the
table that were not present in D7 and are not supported by this module;
therefore, the two modules are incompatible (and really, there is no reason you
would ever need both).

If you would like the additional functionality of [IP Address
Blocking](https://backdropcms.org/project/ip_blocking), you should use it; if
you simply want to keep the Drupal 7 core functionality, use this module; all
D7-blocked IP addresses will still be blocked.


License
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.

Current Maintainers
-------------------

- [Wilmoth Andy Shillingford](https://github.com/docwilmot)
- [Robert J. Lang](https://github.com/bugfolder)

Credits
-------

This module was originally part of Drupal CMS core.
