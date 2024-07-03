# Setting up a local instance of BackdropCMS.org

This document describes how to set up a local instance of the BackdropCMS.org website using the sanitized databases (which are stripped of user accounts' email addresses and password hashes).

BackdropCMS.org uses the CiviCRM module, which stores its data in a separate db. Because CiviCRM is a very complex module, it can be desirable to work on a version of the site without CiviCRM. We give instructions below for both with and without CiviCRM.

## Checkout the repo from git

Begin by checking out this repository, which we will assume creates a directory named `backdropcms.org` containing the repo somewhere on your local system.

Set up your local webserver to point to `backdropcms.org/www/index.php`. 

Do not run the Backdrop installer.

## Download/install the sanitized databases and files

There are sanitized nightly backups of the Backdrop database, CiviCRM database, and files directory at https://sanitize.backdropcms.org. That site is password-protected; you will need to get the username and password from a Backdrop administrator.

Once on that page, you will see backups for Backdropcms.org (first section), as well as for docs.backdropcms.org, forum.backdropcms.org, and events.backdropcms.org. Download the latest backups for backdropcms.org (first row), links to "Database", "CiviCRM", and "Files." The download files will have these names:

* `backdropcmsorg-latest-sanitized.sql.gz`
* `civicrm-latest-sanitized.sql.gz`
* `backdropcmsorg-files-latest.tar.gz`

`cd` to your download directory and unzip the first two (e.g., with `gunzip <filename>`) and expand the last one (e.g., with `tar -xyzf <filename>`).

Remove the existing files directory at `backdropcms.org/www/files` and replace it with the files directory that you just unzipped.

## Install config files

The backdropcms.org site uses `backdropcms.org/config/live-active` as its config location, but for development, we will use `backdropcms.org/config/dev-active`. So,

* `cd` to `backdropcms.org/config`
* Remove all files in `dev-active`, e.g., with `sudo rm -r dev-active/*`.
* Copy `live-active` to `dev-active`, e.g., with `cp live-active/* dev-active`.

## Read in sanitized dbs

In what follows, we assume the databases are named thus:

* `backdropcmsorg` — for the Backdrop database
* `civicrm` — for the CiviCRM database

You may wish to name your local databases differently (for example, if you already have a `civicrm` database on your local MySQL). If so, you'll need to adjust the subsequent instructions accordingly.

Read in the Backdrop database `backdropcmsorg-latest-sanitized.sql` and, if you will be using CiviCRM, read in the CiviCRM database `civicrm-latest-sanitized.sql`.

For the `backdropcmsorg` database, grant these permissions:

```
GRANT SELECT, INSERT, UPDATE, DELETE, CREATE, DROP, INDEX, ALTER,
CREATE TEMPORARY TABLES ON backdropcmsorg.*
TO 'username'@'127.0.0.1' IDENTIFIED BY 'password';
```

The CiviCRM database requires a higher level of permissions for its user. If you are running CiviCRM, grant these permissions on the `civicrm` database:

```
GRANT ALL ON civicrm.*
TO 'username'@'127.0.0.1' IDENTIFIED BY 'password';
```

## Configure settings files

### `settings.local.php`

Most of the settings values you will need are set in `settings.php`, but you will need to customize for your local instance by creating a `settings.local.php` file. 

* Copy the file `settings.local.example.php`, which is at the root of the repo
* Rename it to `settings.local.php`
* Move it into the `www` directory
* Edit the file and fill in the values applicable to your local site in the places where you see empty strings.
* Note that there are two ways to specify the database connection, depending on whether you will be running CiviCRM or not.

Note that the second half of that file contains "PRIVATE Environment specific settings," which are various secret keys and access tokens for the site when interacting with third-party sites. You can leave most of these as empty strings, however, interactions with third-party sites will be disabled as described further below.

### `civicrm.settings.local.php`

If you will be running with CiviCRM enabled, you will need to create a `civicrm.settings.local.php` file. 

* Copy the file `civicrm.settings.local.example.php`, which is at the root of the repo
* Rename it to `civicrm.settings.local.php`
* Move it into the `www` directory
* Edit the file and fill in the values applicable to your local site where you see empty strings, also the username and password for the Backdrop and CiviCRM database connections.

If you will not be running CiviCRM, you can ignore this.

### `civicrm_table_prefixes.php`

The file `civicrm_table_prefixes.php` contains a list of the CiviCRM database tables that need to be made visible to Backdrop; each entry is prefixed with `civicrm.`, which is the name of the CiviCRM database. If your local instance is changing the name of the CiviCRM database, you will need to change every occurrence here (and then be careful because this file is under version control). 

If you are not running CiviCRM, you can ignore this file.

## Up and running

If you will be running CiviCRM, the site should be fully functional; go to the home page and see if it loads.

### Effects of sanitization

The sanitized `backdropcmsorg` database 

* keeps all user names and other user account fields
* truncates all cache tables
* replaces all user account email addresses with a unique but generic value
* replaces all password hashes with a generic hash

This means, however, that you cannot log into your own website account (because the password has changed), and you cannot get a "reset your password" email link because the email address is no longer valid. However, if the site is functional and you have `bee` installed on your system, you can set your account's password using the `bee` command:

```
bee upw <username> <password>
```

The sanitized `civicrm` database

* keeps group membership intact
* replaces all contact address information with generic values
* replaces all contact names with unique generic values
* replaces all emails with unique generic values
* replaces user account names with unique generic values

### Things that won't work on the local site

* Interactions with Tugboat are disabled (see next). If you need to investigate this, in addition to enabling the modules you'll need to enter the `tugboat_access_token` in `settings.local.php`.
* You can't send out emails without filling in the `smtp_pasword` in `settings.local.php`. (We recommend leaving SMTP disabled unless you really know what you're doing, because we don't want a local site to be sending out random emails—and remember that all user account emails have been sanitized).
* The `borg_signup_*` values in `settings.local.php` allow a site to send a request to the BackdropCMS.org site to sign someone up for the newsletter (this includes signups directly from BackdropCMS.org). Without the credentials, that functionality won't work.

### Touch-up

You should disable the `tugboat` and `borg_tugboat` modules so that the local site doesn't try to query Tugboat (and put errors into the dblog). You can do this with `bee` as follows:

```
bee dis -y borg_tugboat
bee dis -y tugboat
```

## Running without CiviCRM

If you will be running with CiviCRM disabled, as noted above, you didn't need to install the CiviCRM database or create a `civicrm.settings.local.php` file; however, you will need to make some changes to your Backdrop database before you attempt to load any page to disable the CiviCRM-related modules.

In MySQL, open the `backdropcmsorg` database and execute the following statements, which will disable the CiviCRM-related modules:

* `UPDATE system SET status = 0 WHERE name LIKE "%civicrm%"`

The affected modules are:

* `civicrm`
* `civicrmtheme`
* `civicrm_cron`
* `civicrm_table_prefixes`
* `webform_civicrm`
* `borg_civicrm`
* `civicrm_group_subset`

Then use `bee` to clear all caches to purge any residues of CiviCRM from the Backdrop database:

```
bee cc all
```

The following things will not work on your local site:

* Of course, all CiviCRM pages (paths that start with `/civicrm/...`) will no longer work.
* On the user profile page, you will not see (nor be able to edit)
    * Name and Address
    * Subscriptions
    * CRM Contact
