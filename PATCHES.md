Patches to Backdrop Core:
=========================

Text module -- formatted text fields should not lock when empty due to access.
  https://github.com/backdrop/backdrop-issues/issues/5151
  https://github.com/backdrop/backdrop/pull/3686.patch


Patches to Backdrop Contrib:
============================

borg theme: (committed to HEAD)
  - Remove bug squad display from base theme.
  - Update CSS for newsletter signup block.
  - Remove upgrade block css from base theme (in subtheme)

---

akismet - prevent PHP fatal when bulk deleting users
  https://github.com/backdrop-contrib/akismet/issues/9
  https://github.com/backdrop-contrib/akismet/pull/10.patch

bakery - Prevent PHP 7.2 errors about deprecated mcrypt.
  https://github.com/backdrop-contrib/bakery/issues/8
  https://github.com/backdrop-contrib/bakery/pull/9.patch

colorbox - Remove dependance on libraries API
  https://github.com/backdrop-contrib/colorbox/issues/9
  https://github.com/backdrop-contrib/colorbox/pull/10.patch

spambot - move last checked uid to state
  https://github.com/backdrop-contrib/spambot/issues/4
  https://github.com/backdrop-contrib/spambot/pull/5.patch

Patches to CiviCRM:
============================

None currently.
