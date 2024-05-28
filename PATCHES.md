Patches to Backdrop Core:
=========================

- none


Patches to Backdrop Contrib:
============================

borg theme: (committed to HEAD)
  - Remove bug squad display from base theme.
  - Update CSS for newsletter signup block.
  - Remove upgrade block css from base theme (in subtheme)

borg -  https://github.com/backdrop-contrib/borg/issues/41
  - HOTFIX for PHP errors in logs www/themes/borg/template.php

---

akismet - prevent PHP fatal when bulk deleting users
  https://github.com/backdrop-contrib/akismet/issues/9
  https://github.com/backdrop-contrib/akismet/pull/10.patch

bakery - Prevent PHP 7.2 errors about deprecated mcrypt.
  https://github.com/backdrop-contrib/bakery/issues/8
  https://github.com/backdrop-contrib/bakery/pull/9.patch

spambot - move last checked uid to state
  https://github.com/backdrop-contrib/spambot/issues/4
  https://github.com/backdrop-contrib/spambot/pull/5.patch

Patches to CiviCRM:
============================

None currently.
