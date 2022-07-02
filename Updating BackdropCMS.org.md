# Updating BackdropCMS.org

This document describes the process for updating the BackdropCMS.org website (and its related subdomains). It assumes you have

* SSH access to the server, and
* A local instance of BackdropCMS.org that you use for development.


## Update staging

Any changes made in the live site should be copied into `config/staging` and added to the repo. See the section "Updating `live-active`" in the file `config/README.md` for a detailed description of how to do this.


## Update local instance

Load your local version of backdropcms.org. Do a `git pull` to bring it up to date.

Make your local changes (e.g., by loading an updated version of Backdrop core.)

Check the PATCHES.md file. If there are any patches, apply them to your local instance, e.g., by

* Downloading and saving the patch file locally (e.g., `NNN.patch`);
* Apply it locally:
  * If the patch comes from the same repository
    * Store it at repo root;
    * use `git apply NNN.patch`.
  * If the patch comes from a different repository (like a contrib module patch),
    * Store it at the root of the module it applies to;
    * `cd` to the module root;
    * use `patch -p1 < NNN.patch`

Once you've made the changes, create a PR against the repo that includes the updates and patches.

Go to [https://github.com/backdrop/backdrop](https://github.com/backdrop/backdrop) and merge the PR.


## Pull changes to BackdropCMS.org

SSH to the server (`ssh backdrop@backdropcms.org`). Then execute these commands:

```
cd repo
git pull
```

If any of the changes added update hooks, visit `https://backdropcms.org/update.php` and run any updates needed.
