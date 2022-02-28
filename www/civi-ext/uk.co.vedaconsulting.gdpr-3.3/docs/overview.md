# Overview

Our new extension aims to enable charities/organisations to manage their supporters in a GDPR compliant manner. GDPR in itself does not introduce many new requirements however it does introduce a number of new obligations on organisations that hold and use data about individuals.

It’s important to understand that simply moving to an opt in process and regarding all existing contacts as being opted out overnight is probably not what is best for your organisation. There are many factors to consider before determining whether to base your marketing contacts on an opt in. For example a membership organisation is likely to be well within its rights to base member communications on the organisation’s ‘legitimate interests’ unless the member explicitly opts out. You may also be able to import contacts from third party fundraising systems, where they have already stated that they are happy to be contacted by the charity they are fundraising for. The overall aim of this extension is to help organisations navigate the journey to GDPR compliance without compromising their presence with and income from their existing supporters.

Under GDPR, therefore, you need to be able to record whether your contacts have given consent to receive marketing. If so, you must be able to show who consented, when they consented, how the consent was given, and exactly what the consent is for (including for which communications channel – post, email or phone, for example).

If you have not asked them to provide consent, your marketing would be based on ‘legitimate interests’. In this case you must record any contact from them asking not to receive marketing, or specifying which marketing they do not want to receive. You should also be able to show that your legitimate interests are not outweighed by their interests. If people don’t respond to your communications over a period of time, the longer this goes on, the harder it might be to argue that you still have a legitimate interest in contacting them.

More details about GDPR and CiviCRM can be found at our [GDPR Site]

The current version (2.1) of this extension does the following:

* Allows you to record the data protection officer or person responsible for data protection compliance in your organisation.
* A new tab 'GDPR' in contact summary will display group subscription log for the contact.
* Custom search 'Search Group Subscription by Date Range' which can be access from GDPR Dashboard.
* Access list of contacts who have not had any activity for a set period of days from GDPR Dashboard.
* The ability to carry out an action on those contacts who have not had any activity.
* Ability to force acceptance of data policy/terms and conditions when a contact logs in and recording this as an activity against the contact with a copy of the terms and conditions agreed to. This is currently Drupal specific.
* The right to be forgotten, allowing users of CivicRM to easily anonymise a contact record, hiding any person details but keeping the financial and other history. The action also exists as an API and therefore can be bolted into other processes.
* User friendly communication preferences, moving to explicitly worded opt in mechanisms.
* Communication preference to include medium per group. Currently CiviCRM supports include or exclude from a group but it does not allow for the selection of the communication medium that should be used for example happy to receive email newsletters but please don’t send me any other emails.
* Inclusion of two new tokens which automatically include checksum and link to the communication preferences page.
* Include a terms and conditions acceptance for events and contributions (membership sign ups) if configured.
* Ability to include profile fields on the communication preferences page, allowing users to ensure other information, such as the name and phone number, for the contact is also valid.
* Notify Data Protection Officer via email for any contacts who have been forgotten, including only the contact id
* Permissions around access to the GDPR settings, Forget me action and the GDPR Dashboard.

Future releases will include:

* Recording audit information when a contact is exported.
* Allowing all exports to be produced with passwords if produced with the MS Excel Extension.
* Ensure Scheduled reminders have a setting to exclude those contacts who have no bulk emails set if the scheduled reminder is deemed to be marketing oriented as a posed to transactional.
* Allow communications preference options to be controlled by the groups the contact belongs to, this will allow members to view more groups than non members as an example.
* Include a more prominent block in mass mailings for users who have not yet updated their commmunication preferences.

We'd also like to take this opportunity to thank [Paul Ticher][Paul Ticher Site] for coming on board with this project as a consultant and expert in the sector.

[Paul Ticher Site]: http://www.paulticher.com/data-protection
[GDPR Site]: https://gdpr.vedaconsulting.co.uk
