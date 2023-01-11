CiviCRM Table Prefixes
======================

This module is a helper for CiviCRM installations where CiviCRM has its own separate db. This module creates prefixes in Backdrop for the CiviCRM tables so that CiviCRM fields may be used in Backdrop db queries, enabling things like using CiviCRM fields in Backdrop Views.

Installation
------------

- Install this module using [the official Backdrop CMS instructions](https://backdropcms.org/guide/modules).

- Visit the configuration page under Administration > Configuration > System >
  CiviCRM table prefixes (admin/config/system/civicrm-table-prefixes) and follow the instructions there.
  
- You will be modifying your `settings.php` file (or `settings.local.php` file) and adding a file `civicrm_table_prefixes.php` to the root of your Backdrop installation.

Usage Notes
-----------

- If CiviCRM adds tables over time (which happens, for example, when you add new custom data groups), you will need to update your `civicrm_table_prefixes.php` file to make the new tables available to Backdrop. The Backdrop status report at Administration > Reports > Status will alert you when this is necessary and the configuration page for this module will provide the required replacement text.

- Since the `civicrm_table_prefixes.php` file contains the CiviCRM database name, if you use a non-version-controlled `civicrm.settings.local.php` file to give local and remote dbs different names, you should probably also keep `civicrm_table_prefixes.php` out of version control since the local and remote versions would be different.
  
Issues
------

Bugs and feature requests should be reported in [the Issue Queue](https://github.com/backdrop-contrib/civicrm_table_prefixes/issues).

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

