Patches to Backdrop Core:
=========================

Text module -- formatted text fields should not lock when empty due to access.
  https://github.com/backdrop/backdrop-issues/issues/5151
  https://github.com/backdrop/backdrop/pull/3686.patch


Patches to Backdrop Contrib:
============================

borg theme: none.

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

markdown - PHP notice
  https://github.com/backdrop-contrib/markdown/issues/4
  https://github.com/backdrop-contrib/markdown/pull/7.patch

restrict_abusive_words - prevent PHP notice
  https://github.com/backdrop-contrib/restrict_abusive_words/issues/1
  fix committed to -dev

restrict_abusive_words - make messages configurable.
  https://github.com/backdrop-contrib/restrict_abusive_words/issues/4
  https://patch-diff.githubusercontent.com/raw/backdrop-contrib/restrict_abusive_words/pull/6.patch

restrict_abusive_words - search within words.
  https://github.com/backdrop-contrib/restrict_abusive_words/issues/3
  https://patch-diff.githubusercontent.com/raw/backdrop-contrib/restrict_abusive_words/pull/7.patch

spambot - move last checked uid to state
  https://github.com/backdrop-contrib/spambot/issues/4
  https://github.com/backdrop-contrib/spambot/pull/5.patch
