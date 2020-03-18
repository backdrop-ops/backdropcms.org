Tugboat
=======

[Tugboat.qa](https://tugboat.qa/) is a service that allows for the deployment of
preview websites, generally for testing and quality assurance during
development. This module provides integration with Tugboat to allow creating
on-the-fly sites with the push of a button.

This module was originally built as a custom module for BackdropCMS.org where it
was used to allow people to demo a Backdrop site quickly and easily. It has been
re-written to use Tugboat's new [API](https://api.tugboat.qa/) and to be more
generic (removing some of the Backdrop-specific wording, etc.).

To use this module, you'll need a Tugboat.qa account and a Git repository that
will be used to create the preview sites. See the
[Tugboat documentation](https://docs.tugboat.qa/setting-up-tugboat/) for more
details.

Installation
------------

- Install this module using the official Backdrop CMS instructions at
  https://backdropcms.org/guide/modules.

- Visit the configuration page under Administration > Configuration > Web
  Services > Tugboat (`admin/config/services/tugboat`) and enter the required
  information.

- Test your setup at the 'Create' page URL you provided (e.g. `/tugboat`).

- Optionally override the module's template files in your theme to customise the
  'Create' and 'Ready' page designs/wording.

Issues
------

Bugs and Feature requests should be reported in the Issue Queue:
https://github.com/backdrop-contrib/tugboat/issues.

Current Maintainers
-------------------

- [Peter Anderson](https://github.com/BWPanda)

Credits
-------

- Originally written for BackdropCMS.org by
  [Nate Lampton](https://github.com/quicksketch).
- Moved to its own contrib repo using
  [instructions](https://gbayer.com/development/moving-files-from-one-git-repository-to-another-preserving-history/)
  by Greg Bayer.

License
-------

This project is GPL v2 software.
See the LICENSE.txt file in this directory for complete text.
