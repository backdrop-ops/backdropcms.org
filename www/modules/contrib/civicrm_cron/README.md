# CiviCRM Cron

Use Backdrop's cron to call CiviCRM's cron. 

Note: CiviCRM's cron can (and in most cases should) be called directly from the
command line, but this is a simple way to define the user, pass, and sitekey
used in the CiviCRM url and call CiviCRM's cron when it's not possible or
convenient to configure CiviCRM's cron.

## Installation

- Install this module using the [official Backdrop CMS instructions](https://backdropcms.org/guide/modules).

## Configuration and Usage

- Make sure CiviCRM is installed.
- Visit **admin/config/civicrm/civicrm-cron** and verify the site key.
- If you are using CiviMail, you will need to set up a cron user with
  a higher level of permission to run cron. Set up a user with the following
  permissions: *view all contacts*, *access CiviCRM*, and *access CiviMail*.
  Add this username and password on the configuration page in the "CiviMail
  Settings" section.

More details may be found (or added) in the [Wiki](https://github.com/backdrop-contrib/civicrm_cron/wiki)

## Issues

Bugs and Feature requests should be reported in the [Issue Queue](https://github.com/backdrop-contrib/civicrm_cron/issues)

## Current Maintainers

- [Laryn Kragt Bakker](https://github.com/laryn), [CEDC.org](https://CEDC.org)
- Collaboration welcome!

## Credits

- Ported to Backdrop by [Laryn Kragt Bakker](https://github.com/laryn), [CEDC.org](https://CEDC.org)
- Created for Drupal by [Kevin Reynen](https://www.drupal.org/u/kreynen).

## License

This project is GPL-2.0 (or later) software. See the LICENSE.txt file in this directory for
complete text.
