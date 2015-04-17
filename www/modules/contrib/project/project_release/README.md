Project Release
===============

Project Release module provides the capability to attach release nodes to a
project node. Releases are essentially nothing but a version string and a
downloadable file. This module also includes Views integration that by default,
will display a list of releases on project node pages.

Installation
------------

- Enable this module along with the Project module.

- A "project_release" node type will automatically be created for you. Configure
  it as necessary under Administration > Structure > Content Types > Project
  Release (admin/structure/types/manage/project-release).

- If you intend to serve release history information to the Backdrop CMS Update
  module, you may wish to configure your web server to make a short URL path
  to the project-release-serve-history.php file. See the documentation in that
  file for more information about setup.

