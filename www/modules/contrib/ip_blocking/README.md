IP address blocking
===================

Enables blocking of IP addresses. 
This module restores lost Drupal core functionality with some improvements.

You can:  
 - see when IP was blocked, who of users block this IP and reason for blocking (if specified);
 - set a 404 (Not Found) status code for visitors from blocked IP instead of default 403 (Access Denied);
 - enable logging for access attempts from blocked IPs;
 - block or unblock IP from "Recent log messages" event pages (admin/reports/event/EVENT_NUMBER).

New in version 1.x-1.0.5:
 - display number of blocked IPs on the "Status report" page;
 - integration with the new "Antiscan" module (https://backdropcms.org/project/antiscan) to 
   automatically block IP addresses used by bad crawlers or vulnerability scanners.

Installation
------------
Install this module using the official Backdrop CMS instructions at https://backdropcms.org/guide/modules

Note: if the "Ban IP" module is installed, you need to uninstall it first to avoid confusion when using the same, 
but extended database table.

Configuration and usage
-----------------------
Administration page is available via menu *Administration > Configuration > 
User accounts > IP address blocking* (admin/config/people/ip-blocking) 
and may be used for:

- block an IP address:
    - enter a valid IP address (for example, 10.0.0.1);
    - (optional) enter description of reason for blocking this IP;
    - click Add;

- unblock previously blocked IP address:
    - beside an IP address, click "unblock", then confirm unblocking.

While browsing "Recent log messages" (admin/reports/dblog) you can quickly review 
an individual entry (admin/reports/event/EVENT_NUMBER) and block (or unblock) 
an IP address from the "Operation" link.

This link will be shown for events of types 'access denied', 'antiscan', 'ip_blocking', 'login_allowlist', 
'page not found', 'system', 'user' and 'php' only if it is valid IP address, and not the IP address of the currently logged in user.

**Screenshots** are available at https://findlab.net/projects/ip-address-blocking

License
-------
This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.

Current Maintainer
------------------
Vladimir (https://github.com/findlabnet/)

More information
----------------
For bug reports, feature or support requests, please use the module 
issue queue at https://github.com/backdrop-contrib/ip_blocking/issues.
