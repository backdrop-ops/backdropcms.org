Project Telemetry
===============

Project Telemetry module provides API-endpoints for remote sites to report usage
information back to a central server. This module is utilized by Backdrop core's
"Telemetry" module, which has Backdrop sites send information about how sites
are configured back to backdropcms.org.

Installation
------------

- Enable this module along with the Project module.

- Create a project node if none exist, or visit an existing one.

- Data can be sent by sending POST requests to the
  `project-telemetry-post.php` file. Data should be of the following given
  format:

  ```
  {
    "project_name": {
      {
        "version": "1.x-dev",
        "setting_1": "foo",
        "setting_2": "bar"
      }
    }
  }
  ```

  An example cURL request from the command line would look like this:

  ```
  curl  https://localhost/modules/project/project_telemetry/project-telemetry-post.php -X -H "x-site-key: 123456" POST -d '{"test": "hi"}' -H 'Content-Type: application/json'
  ```

- Now visit the given project node. There should be a new tab on the project
  page, e.g. `node/10/telemetry`.
