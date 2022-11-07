SMTP Authentication
===================

Send (authenticated) email through external SMTP servers.

This module allows your site to bypass the PHP mail() function and send email directly to an SMTP server. The module supports SMTP authentication and can even connect to servers using SSL if supported by PHP.


Installation
------------

- Install this module using the official Backdrop CMS instructions at
  https://backdropcms.org/guide/modules

- Visit the configuration page under Administration > Configuration > System >
  SMTP Authentication (admin/config/system/smtp) and enter the required
  information.

- To send SMTP mail using OAuth2 authentication (required for Google mail),
install the SMTP OAuth Authentication submodule (part of this package) and the
[Google Auth](https://backdropcms.org/project/gauth) module (which it depends
upon).

Requirements
------------

* This module sends email by connecting to an SMTP server. Therefore, you need
  to have access to an SMTP server for this module to work. Mandrill and
  SendGrid are good examples of mail server service providers.

* The following PHP extensions need to be installed: ereg, hash, date & pcre.

* Optional: To connect to an SMTP server using SSL, you need to have the
  openssl package installed on your server, and your webserver and PHP
  installation need to have additional components installed and configured.

Backdrop will often use the email address entered into Administrator ->
Configuration -> Site information -> E-mail address as the from address.  It is
important for this to be the correct address and some ISPs will block email that
comes from an invalid address.

Connecting to an SMTP server using SSL is possible only if PHP's openssl
extension is working.  If the SMTP module detects openssl is available it
will display the options in the modules settings page.

Sending mail through Gmail requires SSL or TLS.


License
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.


Current Maintainers
-------------------

- Jen Lampton (https://github.com/jenlampton).
- Seeking additional maintainers.

Credits
-------

- Ported to Backdrop CMS by biolithic (https://github.com/biolithic).

This module is a port of the SMTP module for Drupal which was written and maintained by a large number of contributors, including:

- José San Martin (https://www.drupal.org/u/jos%C3%A9-san-martin)
- wundo (https://www.drupal.org/u/wundo)
- Drupal maintenance sponsored by Chuva Inc. <http://chuva-inc.com/>
- This module uses the smtp and mail class's from PHPMailer. (https://github.com/PHPMailer/PHPMailer)

The SMTP OAuth Authentication submodule was written for Drupal by [Sadashiv Dalvi](https://github.com/sadashivdalvi) and ported to Backdrop by [Robert J. Lang](https://github.com/bugfolder).

