# Config Workflow Instructions

These instructions explain BackdropCMS.org's Configuration Management workflow.
For an overview of config management, see the
[API docs](https://api.backdropcms.org/documentation/working-configuration).
BackdropCMS.org uses the
'[Versioned staging directory](https://api.backdropcms.org/documentation/versioned-staging-directory)'
approach with a few changes (namely the inclusion of the `live-active` directory
in version control).

## Rule #1

> **Never commit anything into the `live-active` directory!**

'Live' config will only ever be added and committed to directly from the live
server. All changes that need to be made to configuration should be added and
committed to the staging directory, so it can be safely deployed to any
environment.

## Making config changes in a PR/commit

### Setup

1. Make sure your local `settings.php` file has the following directories
  defined:
  ```php
  $config_directories['active'] = '../config/dev-active';
  $config_directories['staging'] = '../config/staging';
  ```

2. Check that `live-active` is an exact match to what's on BackdropCMS.org (ask
  someone in a GitHub ticket or in Gitter to confirm this for you). See note
  below.

3. Copy the contents of the `live-active` directory into the `staging`
  directory. The best way to do this is to first delete everything in `staging`,
  then copy everything from `live-active`. These two directories need to be an
  exact match.

4. Synchronize config on your local environment to bring your `dev-active`
  directory up-to-date with `staging` (and thus also `live-active`). You can do
  this via the UI at `/admin/config/development/configuration`, or with Drush
  via `drush bcim`.

5. Perform a `git checkout` on the staging directory.

### Make your changes

1. Make your changes to code and/or config in your local environment.

2. Commit just the code changes (don't include any changes to config yet).

3. Copy the contents of the `dev-active` directory into the `staging` directory.
  The best way to do this is to first delete everything in `staging`, then copy
  everything from `dev-active`. These two directories need to be an exact match.
  If you have Drush installed, you can simply run `drush bcex`.

4. Confirm that all of the changes you see in the `staging` directory are
  changes you want to deploy live (using `git status` and/or `git diff`). Note
  that:
  - You do not usually want to commit changes to `system.core.json`
  - The file `xmlsitemap.settings.json` may also report unintended changes

  Perform a `git checkout` on any changes you do **not** want to deploy.

5. Commit the config changes.

6. Push your changes, create a PR, etc.

## Updating `live-active`

**These instructions are solely for people with access to the live server who
have been asked to make sure `live-active` is up-to-date as per step 2 of the
Setup instructions above.**

1. On Backdrop's live server, perform a `git status` to see if there are any
uncommitted changes to the `live-active` config directory.

2. If there are no uncommitted changes, skip to step 5.

3. Copy the contents of the `live-active` directory into the `staging`
  directory. The best way to do this is to first delete everything in `staging`,
  then copy everything from `live-active`. These two directories need to be an
  exact match.

4. Commit the changes to `live-active` and `staging`.

5. Inform the user that `live-active` is up-to-date.
