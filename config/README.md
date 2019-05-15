Config Workflow Instructions
=============================


## How to work with confifg for api.backdropcms.org


Rule #1 never commit anything into the live-active directory.

Live config will only ever be added and committed, directly from the live
server. All changes that need to be made to configuration should be added and
committed into the staging directory, so it can be safely deployed to any
environment.


## Steps to commit config are as follows:

### Before you do any work

1) Check that live-active is an exact match to what's on backdropcms.org. (Ask
   someone in a GitHub ticket or in Gitter to check & commit for you)

2) Manually copy the contents of the live-active directory into the staging
   directory. Check to see that there are no differences. If there are, resolve
   them before you begin work. (delete config files that are not needed, add
   ones that are)

3) Do a config sync on your local environment to bring YOUR dev-active directory
   up date with staging (and thus, also live).

4) Do a `git checkout` on the staging directory.


### Now you are ready to begin your work.

1) Make changes to code and/or config in your local environment.

2) Commit changes to code (do not incclude changes to config in this step).

3) Manually copy the entire contents of your dev-active directory into staging.

4) Do a `git status` and/or `git diff` to confirm all the changes you see in
   the staging directory are changes you want to deploy to production.
   * You do not usually want to commit changes to `system.core.json`
   * The file `xmlsitemap.settings.json` may also report unintended changes.

5) Do a `git checkout` for any changes you NOT want to deploy.

6) Git add & commit all the changes you want to deploy.

7) Push your branch back up to origin.
