AntiBot
=======

Antibot is an extremely lightweight module designed to eliminate robotic form
submissions on your website in an innovative-fashion. The module works
completely behind the scenes and doesn't require any interaction from the
end-users (no annoying CAPTCHAs!). The only requirement to the end user is
that they must have JavaScript enabled. If they do not, the protected forms
will be hidden and a message will appear, telling the user that the form
requires JavaScript be enabled in order to use it.

Antibot aims to:

* Prevent robotic spam submissions on your site's forms (like comments).
* Be as lightweight as any module could possibly be.
* Protect forms while still being able the cache the page.
* Avoid any end-user interaction or annoying CAPTCHA codes.
* Be much more reliable than a honeypot trap.
* Require no third-party integrations and API keys.
* Work on mobile and touch-screen devices.
* Also prevent remotely posted form submissions

How does it work?

1. Admins choose which forms to enable protection for by specifying form IDs.
1. The protected form's action path is switched to /antibot.
1. When the page is loaded, if the user does not have JavaScript enabled, the
   form is hidden and a message is presented to them.
1. After the page is loaded, Antibot, using JavaScript, waits for a mouse to
   move, an enter or tab key to be pressed, or a mobile swipe gesture before
   the action of the form is switched back to the path that it was originally
   set to be. This indicates that the person behind the controls is a human and
   not a robot.
1. Since the action of the form is purposely incorrect until the JavaScript
   changes it, bot submissions will be redirected and the form submissions
   completely disregarded.
1. Since there is no dynamic code generated for each form, pages with Antibot
   can be cached safely.
1. Antibot also generates a unique key value for each form (based on the ID)
   which is required in order for the form to pass validation. The JavaScript
   will automatically insert this value in to the form once it is unlocked.
   This prevents bots from remotely posting forms on your site because that key
   will be missing.

Use cases:

1. A user has JavaScript enabled. They never know the difference and submit the
   form as they normally would.
1. A user does not have JavaScript enabled. The form is hidden and a message is
   present in it's place, telling them they need JavaScript in order to use the
   form.
1. A bot without JavaScript hits your site and attempts to submit the form.
   Since it does not have JavaScript, the form action redirects them to
   /antitbot, which is a landing page explaining what happened. The form data
   is completely disregarded.
1. A bot with JavaScript hits your site (unlikely). Since Antibot waits for
   keypresses or mouse/swipe movements, the form remains protected, and the
   robotic submissions brings them to /antibot, where nothing happens.
1. A bot remotely posts data to your forms (using something like cURL). This
   fails validation because Antibot requires a unique key to be present in the
   values of all protected forms, which it inserts automatically via JavaScript.


Installation
------------

- Install this module using the official Backdrop CMS instructions at
  https://backdropcms.org/guide/modules.

- Visit the configuration page under Administration > Configuration > Sytem >
  Antibot (admin/config/system/antibot) and create a list of form Ids that
  you want to protect. You can use wildcard (*) characters. By default, comment
  forms, site-wide contact forms, and user forms are protected. There is no
  limit.

- There is an additional admin setting that allows admins to be shown the form
  IDs of all forms on the page and whether or not they are Antibot-activated.


Documentation
-------------

Additional documentation is located in the Wiki:
https://github.com/backdrop-contrib/antibot/wiki/Documentation.

Issues
------

Bugs and Feature requests should be reported in the Issue Queue:
https://github.com/backdrop-contrib/antibot/issues.

Current Maintainers
-------------------

- Jen Lampton (https://github.com/jenlampton).
- Seeking additional maintainers.

Credits
-------

- Ported to Backdrop CMS by [Jen Lampton](https://github.com/jenlampton).
- Originally written and Maintained for Drupal by [Mike Stefanello](https://www.drupal.org/u/mstef).

License
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.
