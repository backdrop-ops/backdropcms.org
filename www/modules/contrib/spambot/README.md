Spambot
=======

Introduction
------------

Spambot protects the user registration form from spammers and spambots by
verifying registration attempts against the Stop Forum Spam
(www.stopforumspam.com) online database.
It also adds some useful features to help deal with spam accounts.

This module works well for sites which require user registration
before posting is allowed (which is most forums).


Installation
------------

Install as you would normally install a contributed Backdrop module.


Configuration
-------------

Configure user permissions in Administration » User accounts » Permissions:

Users in roles with the "Protected from spambot scans" permission would not
be scanned by cron.

Go to the '/admin/config/system/spambot' page and check additional settings.


License
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.


Maintainers
-----------
Ported to Backdrop by docwilmot

Seeking maintainers.

Drupal maintainers:
* bengtan (bengtan) - https://www.drupal.org/u/bengtan
* Michael Moritz (miiimooo) - https://www.drupal.org/u/miiimooo
* Dmitry Kiselev (kala4ek) - https://www.drupal.org/u/kala4ek
