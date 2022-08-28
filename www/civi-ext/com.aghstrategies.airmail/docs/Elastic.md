# Elastic Email Specific Configuration

ATTENTION: the HTTP Web Notifications (Webhooks) feature is only available in the Unlimited Pro and Email API PRO plans. This feature is needed for this extension, so make sure your plan includes it. Source: https://help.elasticemail.com/en/articles/2376855-how-to-manage-http-web-notifications-webhooks

## Settings in Elastic Email

Log in to Elastic Email, go to the Settings menu, then select the Notifications tab.

Create a new webhook by clicking on the (+) button on the right side and give it a name. For the notification link, copy the URL displayed in the CiviCRM setup screen for the extension, and select the Bounce/Error event and Abuse Report options.

You'll also need to ensure that on the main 'Sending' page of their Settings pages, you have checked "Allow custom headers" as this extension relies on that.

(Note: if you want to disable Elastic Email's tracking of opens and clicks - which CiviMail does anyway - you can do so after you have sent your first mailing. Otherwise the admin UI forbids it. You can also ask them to disable this for you, by emailing their support team with your reasons.)


## Complying with Elastic Email's Unsubscribe link requirements

**Elastic Email requires that its own Unsubscribe link is present in all emails. It you don‘t add it they will inject it into your mailings for you. This will cause big problems.**

- CiviCRM won't know about it.

- Elastic Email will suppress all future emails to that email address.

- If someone is subscribed to two groups in CiviCRM, unsubscribing with the Elastic Email unsubscribe link effectively unsubscribes them from both (and any future groups they subscribe to and any future transactional emails like receipts)

- If one incumbant of campaigner@example.org unsubscribes, a future staff member could not be resubscribed.

- If you provide a new email address for someone, then that could get used despite their having "unsubscribed" via Elastic Email, since they only go on the email address.

- It's not possible for CiviCRM to re-subscribe someone unsubscribed this way; you need to manually use a special form from Elastic Email which will send a very generic confirmation email. This seems unlikely to be successful.

Why? Elastic Email's main business is providing a full email marketing solution, not just a SMTP relay. This causes some friction because it means their primary way of working is basically a second CRM with a lot of the same data in as CiviCRM has. This extension works on the idea that we're only interested in the SMTP relay part of Elastic Email's offering; keeping two CRMs in sync is a nightmare thing to have to do and would definitely be out of scope for this extension.

## Compliance: default option

- For CiviMail: provide a prominent Unsubscribe link that uses the `{action.unsubscribeUrl}` token link. This way people will be able to unsubscribe via CiviCRM in the normal way, without affecting their other subscriptions.

- For CiviMail: *also* provide a less-prominent link that uses the `{action.optOutUrl}` token link. This will get wrapped in Elastic's `{unsubscribe:...}` link, meaning you're compliant. People clicking the link will immediately get unsubscribed by Elastic (so you won't be able to email them again) and will then get through to CiviCRM's normal opt-out page. Unfortunately there's not much point in the confirmation stage of the opt-out page now, but at least if people do complete the form to opt-out then the two CRMs will be in sync.

- For Message Templates: You can include your own wording around the Elastic Email `{unsubscribe}` token, so you have control.

In either case, if you do not include Elastic's `{unsubscribe}` token (i.e. you didn't include `{action.optOutUrl}` in a CiviMail mailing, or you haven't directly added it in your message template), then it will be injected by this extension.

You can to provide some text (on the Airmail settings page) that is injected before the link. e.g. reasonable text might be:

> Emails that are not sent to subscribers (e.g. receipts, confirmations etc.) won’t have an unsubscribe link. You can block our use of this email address using the link below, but this will also prevent us sending receipts or confirmations in future.

So you can provide suitable text to explain and dissuade people from using that required link. This will cover you from accidental non-compliance with their terms, and will protect you from their easy-sounding but oh-so-wrong "Unsubscribe" link.


## Compliance: recommended option

Negotiate the "Track Stats Only" flag on your account with Elastic Email. Once agreed, on the settings page:

1. check the box saying you've agreed that.

2. You still need to enter suitable text for the fallback link, but hopefully that won’t need to be used (read below).

3. click Save.

Then:

- use the normal `{action.unsubscribeUrl}` and `{action.optOutUrl}` based links in your CiviMail mailings. Each of these will be wrapped by Elastic's `{unsubscribe}` token, so they'll be happily able to monitor that, but it will no longer prevent you mailing that email address, and the user will see CiviCRM's normal unsubscribe/opt-out confirmation pages.

- For message templates you don't *need* to do anything (explained below).

When you send mail without the required Elastic `{unsubscribe}` token, it will now try to add a simple "Delete my email" link, instead of your wordy explanatory text.

That link is wrapped in Elastic's token, but points to a special page on your site which presents an option to the user:

- opt out from all bulk mail

- opt out from all bulk mail and delete my email (meaning no more receipts etc.)

(It's explained a bit more clearly than that on the page. See the template for wording.)

This way the user can make an informed choice about their data.
