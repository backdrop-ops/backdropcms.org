Akismet
=======

Integrates with the [https://akismet.com](Akismet) content moderation service.

Requirements
------------

None.


Installation
------------

* Install this module using the official Backdrop CMS instructions at
  <https://backdropcms.org/guide/modules>
* Go to <https://akismet.com>,
  * sign up or log in with your account
  * go to your Account Overview
  * Find your Akismet API key at the top of the page.
* Enter your API keys on Administration » Configuration » Content authoring
  » Akismet » Settings.
* If your site runs behind a reverse proxy or load balancer:
  * Open sites/default/settings.php in a text editor.
  * Ensure that the "reverse_proxy" settings are enabled and configured
    correctly.
  Your site MUST send the actual/proper IP address for every site visitor to
  Akismet.  You can confirm that your configuration is correct by going to
  Reports » "Recent log messages".  In the details of each log entry, you should
  see a different IP address for each site visitor in the "Hostname" field.
  If you see the same IP address for different visitors, then your reverse proxy
  configuration is not correct.
* On servers running PHP <5.4, and PHP as CGI (not Apache module), inbound HTTP
  request headers are not made available to PHP.  Add the following lines to
  your .htaccess file:

    RewriteEngine On
    RewriteRule .* - [E=HTTP_AUTHORIZATION:%{HTTP:Authorization}]

Configuration
-------------

The Akismet protection needs to be enabled and configured separately for each
form that you want to protect with Akismet:

* Go to Administration » Configuration » Content authoring » Akismet.
* Add a form to protect and configure the options as desired.

Note the "bypass permissions" for each protected form:  If the currently
logged-in user has any of the listed permissions, then Akismet is NOT involved
in the form submission (at all).

Testing
-------

Do NOT test Akismet without enabling the testing mode. Doing so would negatively
affect your own author reputation across all sites in the Akismet network.

To test Akismet:

* Go to Administration » Configuration » Content authoring » Akismet » Settings.
* Enable the "Testing mode" option.
  Note: Ensure to read the difference in behavior.
* Log out or switch to a different user, and perform your tests.
* Disable the testing mode once you're done with testing.

FAQ
---

Q: Akismet does not stop any spam on my form?

A: Do you see the Akismet privacy policy link on the protected form?  If not, you
   most likely did not protect the form (but a different one instead).

   Note: The privacy policy link can be toggled in the global module settings.

Q: Can I protect other forms that are not listed?
Q: Can I protect a custom form?
Q: The form I want to protect is not offered as an option?

A: Out of the box, the Akismet module allows to protect Backdrop CMS core forms only.
   However, the Akismet module provides an API for other modules.  Other modules
   need to integrate with the Akismet module API to expose their forms.  The API
   is extensively documented in akismet.api.php in this directory.

   To protect a custom form, you need to integrate with the Akismet module API.
   If you have a completely custom form (not even using Backdrop's Form API), see
   the Akismet developer documentation: <https://akismet.com/development/api/>

Issues
------

To submit bug reports and feature suggestions, or to track changes:
  <https://github.com/project/akismet/issues>

For issues pertaining to the Akismet service, contact [https://akismet.com/contact](Akismet Support),
for example, inappropriately blocked posts, spam posts getting through, etc.

Current Maintainers
-------------------

* Herb v/d Dool <https://github.com/herbdool/>
* Seeking additional maintainers.

Credits
-------

* Ported to Backdrop by Herb v/d Dool <https://github.com/herbdool/>
* Originally developed for Drupal by [Katherine Senzee (ksenzee)](http://drupal.org/u/ksenzee)

This module is a fork of the Mollom module. The Mollom spam-control service was
discontinued in 2018. This module attempts to serve as a drop-in replacement
for sites that were previously using Mollom and want to switch to Akismet. It
makes as few changes to the original Mollom module code as possible.

License
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.
