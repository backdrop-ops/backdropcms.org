# Config Workflow Instructions

These instructions explain BackdropCMS.org's Configuration Management workflow.
For an overview of config management, see the
[API docs](https://api.backdropcms.org/documentation/working-configuration).
BackdropCMS.org uses the
'[Versioned staging directory](https://api.backdropcms.org/documentation/versioned-staging-directory)'
approach with a few changes (namely the inclusion of the `live-active` directory
in version control).

## Rule #1

**Never commit anything into the `live-active` directory from a local instance!**

'Live' config will only ever be added and committed to directly from the live
server. All changes that need to be made to configuration should be added from
the live server and then copied and committed to the `staging` directory, so it can be
safely deployed to any environment.

## Making config changes in a commit/PR from a local development environment

### Setup

1. Make sure your local `settings.php` file has the following directories
   defined:

   ```php
   $config_directories['active'] = '../config/dev-active';
   $config_directories['staging'] = '../config/staging';
   ```

2. Check that `live-active` is an exact match to what's on BackdropCMS.org (ask
   someone in a GitHub ticket or in Zulip to confirm this for you). See
   **Updating `live-active`** note below.

3. Compare the contents of the `live-active` and `staging` folders. If the step
  "Updating `live-active`" below was carried out, they will be the same.
  
Ideally, your PR should only contain config changes that are relevant to the issue that your PR is responding to, so `staging` should match `live-active` before you begin making your changes. If they do not match, and you have server access, you should follow the steps in **Updating `live-active`** to make them match before proceeding with your PR (or ask again for an admin to perform the match).

If they don't match, you don't have server access, and you don't want to wait further, then you can roll the config changes into your PR: copy the contents of the `live-active` directory into the `staging` directory by first deleting everything in `staging`, then copying everything from `live-active` (or if you have `drush` installed locally, you can run `drush bcex`). 

Once they do match, you can proceed as follows.

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
   everything from `dev-active`. These two directories need to be an exact
   match. If you have Drush installed, you can simply run `drush bcex`.

4. Confirm that all of the changes you see in the `staging` directory are
   changes you want to deploy live (using `git status` and/or `git diff`). Note
   that:

   - You do not usually want to commit changes to `system.core.json`
   - The file `xmlsitemap.settings.json` may also report unintended changes

   Perform a `git checkout` on any changes you do **not** want to deploy.

### Commit and push

5. Commit the config changes. (The commit should contain changed files only in the
  `staging` directory, and no changes in `live-active` (see Rule #1 above).

6. Push your changes, create a PR, etc.

## Updating `live-active`

**These instructions are solely for people with access to the live server who
have been asked to make sure `live-active` is up-to-date as per step 2 of the
Setup instructions above or are making their own changes.**

1. On Backdrop's live server, perform a `git status` to see if there are any
   uncommitted changes to the `live-active` config directory.

2. On the live server, visit [Configuration Manager](https://backdropcms.org/admin/config/development/configuration) to
  see if there are any differences between `staging` and `live-active` not
  reflected in the result of step 1.

2. If there are no uncommitted changes or staging/live-active differences, skip
  to step 5.

3. Otherwise, copy the contents of the `live-active` directory into the `staging`
   directory. These two directories need to be an exact match. You can do this by either:
   
   Delete everything in `staging`, followed by copying everything from `live-active` to `staging`,

   OR, via the command line, `cd ~/repo/www` followed by `drush bcex`.

4. Commit the changes made to `staging` plus any uncommitted changes in `live-active`:
  * `cd ~/repo`
  * `git status` (to review the changes)
  * `git add <file>` if there are untracked file(s) to be added to the commit.
  * `git commit -a --message="Update config files from live-active."`
  * `git push origin main`


5. Inform the user that `live-active` is up-to-date and that `staging` and `live-active` are identical.


