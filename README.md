# BackdropCMS.org

This repository holds the codebase for
[www.backdropcms.org](https://www.backdropcms.org).

## Issues

This project contains an
[issue tracker](https://github.com/backdrop-ops/backdropcms.org/issues)
specifically for backdropcms.org. Please use this issue tracker when:
* filing bug reports
* requesting new features
* requesting content changes

## Pull Requests

You can help make improvements to backdropcms.org by submitting pull requests to
this repository.

Please do not create pull requests for content changes, instead please create an
issue requesting editorial access to the site.

## Patches

A list of currently applied patches are located in PATCHES.md

## Contributing

To contribute to the development of backdropcms.org, fork this repository into
your own GitHub account, then clone it to your local environment. 
    
* Using [Lando](https://lando.dev/): use Lando to build a local copy of the site, then run the
following commands to get the latest sanitized database and files directory:

```
lando pull-db
lando pull-files
```

* [MAMP](https://mamp.info) is an alternative for Mac users.


See the `/config/README.md` file for instructions on setting up Backdrop's
config workflow.

See the `Updating BackdropCMS.org.md` file for instructions on updating Backdrop core.
