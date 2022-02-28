# SendGrid Event Notification app listener for CiviCRM

Please consider using https://github.com/aghstrategies/com.aghstrategies.airmail as it is more recent.

SendGrid is a 3rd party bulk email delivery provider that features an Event Notification app (https://sendgrid.com/docs/API_Reference/Webhooks/event.html) which is included with their service. This functionality was chosen to integrate as a CiviCRM extension because it's easy to configure what notifications you want sent, features basic HTTP authentication, requires just a "responder" or "listener" (this extension) to receive the notifications from SendGrid and add them to the CiviCRM database, and could be developed to be relatively agnostic of a specific organization and distributed to the CiviCRM community.

The email events that SendGrid sends notifications of include: _Processed, Dropped, Deferred, Delivered, Bounced, Opened, Clicked, Unsubscribed From, Marked as Spam, ASM Group Unsubscribe, ASM Group Resubscribe._ While it is safe to select all actions to be reported by the SendGrid Event Notification app, for better performance _Processed, ASM Group Unsubscribe_, and _ASM Group Resubscribe_ should be deselected. They are essentially meaningless and therefore ignored. CiviCRM already counts an email as delivered as soon as the mail is sent, so _Delivered_ is also ignored. _Deferred_ is simply a temporary failure that will be reattempted; this extension does nothing more that record it to the main CiviCRM log, so you may wish to deselect this action as well.If a particular event type (like click-throughs) were not selected to be sent then the extension simply skips processing that event. This way if a Civi site owner wants to have CiviMail process certain events they can.

The extension allows the site admin to select if they would like SendGrid or CiviCRM to process _Open_ and _Click-through_ events, and if tracking should be made optional per mailing. The extension adds a Mail Spam Report template and includes spam reports on the Mail Summary and the Detailed Report for the mailing. The extension also supports authentication with a username and password.

To install add to your CiviCRM Extentions folder, enable, then go to Mailings > SendGrid Configuration to configure settings. The extension will display the HTTP Post URL to configure in your SendGrid Event Notifications App, as well as other server configuration instructions if needed.

This extension was initially developed by Dave Reedy and Evan Chute of the International Mountain Biking Association (https://github.com/imba-us/com.imba.sendgrid).  
It also includes major changes by Andrew Hunt from AGH Strategies (https://github.com/agh1/com.imba.sendgrid/tree/bigchanges).

### Testing

To recreate a bounce message being sent from sendgrid to be processed by this extension issue,

Go to {yoururl}/wp-admin/admin.php?page=CiviCRM&q=civicrm%2Fsendgrid (can be found thru the ui by going to Civi Admin Menu -> Mailings -> SendGrid Configuration) document the HTTP Post URL

Find a bounce in sendgrid on sendgrid.com, document the "email", "smtp-id", "processed string (timestamp)", and "reason"

Find that mailing in civi by going to the api and doing a get for Entity "MailingEventQueue" for the contact with the email

```php

$result = civicrm_api3('MailingEventQueue', 'get', array(
  'sequential' => 1,
  'contact_id' => 18309,
));

```
Document the "id", "job_id", "email_id", "contact_id", and the "hash" from the api call,

Create a curl command that looks like this:

```

curl -X POST -H "Content-Type: application/json" -d '[
   {
     "event": "bounce",
     "email": "{email from sendgrid}",
     "smtp-id": "{smtp-id from sendgrid}",
     "timestamp": {processed string from sendgrid made into strtotime()},
     "job_id": {job_id from civi},
     "hash": "{hash from civi}",
     "event_queue_id": {id from civi},
     "reason": "{reason from sendgrid}"
   }
 ]' --url "{HTTP Post URL}"

```

Run it from the command line. Look up that contact and check activities, then go to the mailing report the bounce should be reported.
