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

- Set up a personal access token:

  - Visit your GitHub profile page (or better yet, set up a dedicated new
    account). Edit the profile. Under the "Applications" tab click
    "Generate new token". This will create a special token that can access
    certain items as if it was your user account.

  - Name your token something identifiable, such as "project_github_token".

  - Set permissions for the token. The Project GitHub integration module only
    requires access to the "public_repo" permission (and "repo" if working with
    private repositories). It's recommended to use the bare minimum for
    security. You can always create a new token with more permissions later.

  - Generate the token. On the next page, you'll be shown the new personal
    access token. Note you MUST copy it at this point. GitHub will never show
    it to you again.

  - Now that you have a personal access token, add this line to the bottom of
    your site's settings.php file (replacing your token in the value):

    $settings['project_github_access_token'] = 'e7f11ec_hash_from_github_4f6af62';

- Set up GitHub webhooks:

  - Webhooks can be set up at the organization or on a per-repository basis.
    You can find and create webhooks by editing the profile of an organization
    or the settings of a repository on GitHub. Find the tab for "Webhooks &
    Services".

  - This module requires two separate webhooks set up each as follows:

    - Payload URL: http://example.com/project/github/release
    - Content type: application/json
    - Secret: [make up a secret value and save it for later]
    - Let me select individual events:
      - Release

    - Payload URL: http://example.com/project/github/push
    - Content type: application/json
    - Secret: [use the same secret value as the first hook]
    - Just the push event

  - Lastly, add the secret you created in your webhooks to the settings.php
    file:

    $settings['project_github_secret_key'] = 'secret_key_created_in_webhooks';

To test the entire system, create a new release on GitHub. This should
automatically create a new project node and project release node on your site.
The new project should have the GitHub path and README sync option enabled by
default. Try modifying the README.md file on GitHub, and the description on
the project node should be updated immediately.

The most useful troubleshooting tool is checking a combination of the recent
watchdog entries (Reports > Recent Log Messages) and checking the webhook
responses under the repository or organization webhook settings on GitHub.
