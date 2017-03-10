Bakery module
=============

Provides single sign-on (SSO) functionality for two or more sites.

Deploy this module on the authoritative "master" Backdrop server and the secondary
"slave" or subsite server. The master and slave must be on the same domain*.


Installation
------------

1. Install this module using the official Backdrop CMS instructions at
  https://backdropcms.org/guide/modules

2. Visit the configuration page under Administration > Configuration > System >
  Bakery (admin/config/system/bakery) and configure, as described below.


Set-Up
------

Enable and configure Bakery on the master server first. It is recommended that
you use the UID 1 Backdrop account for this configuration.

This is the master site:

  3. Check the box for "Is this the master site?"
  4. Enter the full URL of this site, including ending forward slash
    - Example: http://example.org/

For SSO to work, Bakery must know the slave, or subsites, to use.

  5. Enter the full URLs of each slave site, separated by newlines
    - Example:  http://store.example.org/
                http://api.example.org/

Two other required fields for Bakery to work are the private key and the cookie
domain.

  6. Enter a long and secure private key
  7. Enter the domain to use for Bakery cookies. These cookies are shared so
      the domain should be the top level, with leading period.
    - Example: .example.org
  8. Save configuration (we'll come back to the other fields)

Now to enable and configure Bakery on the slave or subsite. If possible, you
should log in and use the UID 1 Backdrop account for this configuration.

  9. Enable Bakery at admin/modules
  10. Visit admin/config/system/bakery to configure

This is a subsite site.

  11. Do not check the master site box
  12. Enter the full URL of the master site set in step #4
  13. The slave sites textarea can be left blank
  14. Enter the exact same private key set in step #6
  15. Enter the exact same domain set in step #7
  16. Save configuration (we'll come back to the other fields)

Bakery should now be set to work for the master and this slave site. Open a
different browser than the one you are currently using and visit the master
site. Log in with a standard account. Once successful visit the slave site and
confirm that you are also logged in. If you encountered problems at any point
please consult the section here labeled "Problems and support".

You can now enable and configure Bakery for sites in your network if required,
or read the section labeled "Sharing account information using Bakery".

* Master and slave must be on the same domain, but are not required to be at
certain levels. For example, you can have the master be sub.example.com and a
slave be example.com.

Documentation
-------------

Additional documentation is located in the Wiki:
https://github.com/backdrop-contrib/bakery/wiki/Documentation

Issues
------

Bugs and Feature requests should be reported in the Issue Queue:
https://github.com/backdrop-contrib/bakery/issues

Current Maintainers
-------------------

- docwilmot (https://github.com/docwilmot)
- Jen Lampton (https://github.com/jenlampton)
- seeking additional maintainers

Credits
-------

- Ported to Backdrop CMS by [docwilmot](https://github.com/docwilmot).
- Originally written for Drupal by [chx](https://www.drupal.org/u/chx)
- Maintained for Drupal by [David Strauss](https://www.drupal.org/u/david-strauss).
- Maintained for Drupal by [drumm](https://www.drupal.org/u/drumm)
- Maintained for Drupal by [pwolanin](https://www.drupal.org/u/pwolanin)
- Maintained for Drupal by [douggreen](https://www.drupal.org/u/douggreen)
- Maintained for Drupal by [coltrane](https://www.drupal.org/u/coltrane)

License
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.
