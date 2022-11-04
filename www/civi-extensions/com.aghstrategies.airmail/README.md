This Extension:
--------------

Process Bulk CiviMail responses for a variety of SMTP Services including:

- Amazon SES
- Elastic Email
- Sendgrid


## Configuration

### Settings for this extension on your site

Drupal Ex: http://{yourDomain}/civicrm/airmail/settings  
Wordpress Ex: http://{yourDomain}/wp-admin/admin.php?civiwp=CiviCRM&q=civicrm%2Fairmail%2Fsettings  

Enter a secret code, it does not matter what this code is should be a random string of numbers and letters.
For Open/Click Processing slect "CiviMail"
For External SMTP Service Select the service you are using.

Save Configuration

On save the post URL should display below in a green box it will look something like this:

Wordpress Ex: http://{yourDomain}/civicrm?civiwp=CiviCRM&q=civicrm/airmail/webhook&reset=1&secretcode={secretCode}

### Configuration specific to your SMTP Service

+ [Amazon SES specfic Configuration](/docs/ses.md)
+ [SendGrid specific Configuration](/docs/Sendgrid.md)
+ [Elastic Email specific Configuration](/docs/Elastic.md)
