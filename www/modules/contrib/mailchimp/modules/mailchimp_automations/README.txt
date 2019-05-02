Integrate your Backdrop entities with MailChimp's workflow automation endpoints.

## Installation

1. Enable the MailChimp Automation module
2. Make sure you have a recent version of the MailChimp PHP API library, which includes the MailchimpAutomations API service.

## Usage

1. Define which entity types you want to show campaign activity for at
/admin/config/services/mailchimp/automations.
  * Select a Backdrop entity type.
  * Select a bundle.
  * Select the email entity property.
  * Select the appropriate MailChimp List
  * Select the appropriate MailChimp Workflow
  * Select the appropriate MailChimp Workflow Email
2. Configure permissions for managing MailChimp Automations

## Notes

1. The "Import mailchimp automation entity" button on the Automations admin tab will
throw a PHP error due to a bug in Entity API. You can prevent this error by
applying the patch in https://drupal.org/comment/8648215#comment-8648215 to
the entity module.
2. See additional options in the mailchimp_automations.api.php file, such as passing merge variables to MailChimp.
