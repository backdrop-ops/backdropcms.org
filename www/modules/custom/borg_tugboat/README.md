BackdropCMS.org Demo (Tugboat) Integration
==========================================

This module provides demo capabilities using the tugboat.qa service. Tugboat
provides up to 40GB (currently) of sandbox space to host Backdrop CMS demo
sites. This service is provided by [Lullabot](https://www.lullabot.com) free
of charge. Smaller service plans are free for all users while larger accounts
may be acquired for Open Source projects like Backdrop CMS. We are limited by
available disk space, not the number of previews. With current configurations
we use about 1.5GB of space per "base preview" (version of Backdrop we support),
plus whatever disk space is used by individual installs of Backdrop (such as
their database size and uploaded files).

## Site Configuration

Using this module requires setting an access token in settings.php:

```
$settings['borg_tugboat_token'] = '12345678901234567890123456789012';
```

An access token can be generate at https://dashboard2.tugboat.qa/access-tokens

The BackdropCMS.org Demos account dashboard is accessible at
https://dashboard2.tugboat.qa/5bdb5c268eabd5000137a86d

Request access from info@backdropcms.org to be added to the project and generate
a token for your own personal localhost setups.

## Using Tugboat Commands

Tugboat commands are executed through the tugboat binary in /bin/tugboat. For
example to list all current sandboxes use:

```
./tugboat -t ela2etnr1kgap1lxb05kslhooeorcxdc ls previews repo=5bdb5c268eabd5000137a87b --json
```

Piping the results into the `jq` command line tool is also very useful for
pretty-printing and filtering the results:

```
./tugboat -t ela2etnr1kgap1lxb05kslhooeorcxdc ls previews repo=5bdb5c268eabd5000137a87b --json | jq
```

For more information about the Tugboat CLI tool, see the documentation at:
https://docs.tugboat.qa/advanced/cli/

## Executing Tugboat Commands in PHP

To execute a command in PHP code, use the provided `_borg_tugboat_execute()`
function such as this:

```
$return_data = array();
$error_string = '';
$success = _borg_tugboat_execute("ls previews repo=$repo", $return_data, $error_string);
```

The returning result data will populated into the `$return_data` array passed by
reference. If `$success` is `FALSE`, any error from Tugboat will be populated
into the `$error_string` value.

