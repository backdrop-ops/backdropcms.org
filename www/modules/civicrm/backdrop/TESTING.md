This repository contains the CiviCRM-Backdrop integration module. It does NOT
contain tests for core functionality.  (For tests of core functionality, see
https://github.com/civicrm/civicrm-core .)

At time of writing, these tests execute within the context of the default
Backdrop database.  Consequently, you must be quite careful to write tests
which don't leave a mess in the database.
