Project GitHub Integration
==========================

Project GitHub module provides integration with GitHub. It can automatically
pull in the following information from GitHub:

- Create new project nodes upon first-time official releases on GitHub.
- Sync project descriptions (the body field) with the README file from GitHub.
- Provide zip file "packages" on GitHub when a new release is created on GitHub.
  These packages are created by downloading the GitHub release zip file,
  modifying the contents (by scanning for .info files), and then attaching the
  modified package back to the release on GitHub.
- Create new project *release* nodes upon each official release on GitHub, which
  automatically is populated with the download link and file size on the created
  node.

Installation
------------

- Enable this module along with the Project and Project Release modules.

- On all project nodes, two new options will appear:

  - A GitHub Path option for entering the path on GitHub to a project.
  - The ability to sync a README from GitHub to the project node.

  The GitHub syncing README option works immediately without any additional
  configuration, but in order to be updated continuously, a webhook and
  personal access token need to be set up on GitHub.

- Set up GitHub web hooks:

  This module uses the [GitHub API module](https://backdropcms.org/project/githubapi)
  to interact with GitHub. To set up a connection between a repository or
  organization, visit the GitHub API settings page and add the owner and
  repository path and click "Hook repo". This will set up necessary web hooks
  on GitHub.com.

To test the entire system, create a new release on GitHub. This should
automatically create a new project node and project release node on your site.
The new project should have the GitHub path and README sync option enabled by
default. Try modifying the README.md file on GitHub, and the description on
the project node should be updated immediately.

The most useful troubleshooting tool is checking a combination of the recent
watchdog entries (Reports > Recent Log Messages) and checking the webhook
responses under the repository or organization webhook settings on GitHub.
