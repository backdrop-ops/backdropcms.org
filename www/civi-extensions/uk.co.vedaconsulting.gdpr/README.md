# GDPR #


### Overview ###

Extension to support General Data Protection Regulation

### Installation ###

* Install the extension manually in CiviCRM. More details [here](https://docs.civicrm.org/sysadmin/en/latest/customize/extensions/#installing-a-new-extension) about installing extensions in CiviCRM.
* Fill in GDPR settings (Navigate to Contacts >> GDPR Dashboard or navigate to civicrm/gdpr/settings)

### Usage ###

* A new tab 'GDPR' in contact summary will display group subscription log for the contact, as well as the last time they accepted the site Data Policy and updated their Communications Preferences.
* 'Forget Me' button in GDPR tab, which performs the below action.
  * Anonymize/Update contact's last name based on GDPR settings.
  * Delete contact's email/address/phone/IM/website.
  * Cancel all active memberships and update to 'GDPR Cancelled' status using staus override.
* 'Export' button in GDPR tab.
  * You can export contact, activities, contributions, memberships, particpant & case records of the contact.
  * You can export in CSV or PDF format.
  * Highly recommended to install wkhtmltopdf if you are exporting the data in PDF format, as wkhtmltopdf provides better performance than dompdf.
* Custom search 'Search Group Subscription by Date Range' which can be accessed from GDPR Dashboard.
* Access list of contacts who have not had any activity for a set period of days from GDPR Dashboard and perform action on the contacts. This will help to get a list of contacts who did not have a particular set of activities and can be deleted from GDPR Dashboard by clicking on numaric value of "No of contacts".
* Sitewide Data Policy acceptance can be configured from within GDPR Settings.
* Event settings have a new tab to set Terms and Conditions which are added to the registration form.
* A Communications Preferences page at civicrm/gdpr/comms-prefs/update allows contacts to update their channels and group subscriptions. The settings for this can be reached from the GDPR Dashboard. There are tokens and an action link available to generate personalized links (with checksum) to the Communications Preferences page.

### Wordpress Shortcode ###

[civicrm component="gdpr" hijack="0" action="update-preferences"]

NOTE: Specify hijack option (hijack="1") to replace all other content on that page rather than being displayed inline.

### Documentation ###

View the [GDPR
documentation](https://docs.civicrm.org/gdpr/en/latest/) at
https://docs.civicrm.org/.

### Support ###

support (at) vedaconsulting.co.uk

### Change Log ###

v3.4 Includes:
 
More options for 'Export' functionality in GDPR tab:

- Relationships, GA & GRPR as export options
- Exclude bulk email activities during export

v3.3 Includes:

- WP Shortcode
- Fixes in documentation & call to reconcile modules #295
- Set subject on 'Update Communication Preferences' based on source form
- Convert to use API4 for creating the activity
- Add GDPR configuration to Administer menu
- PHP notice and deprecation fixes
- Fix template syntax error on dashboard

v3.2

README Updated for failed 3.2 release

v3.1

Includes:
* Export option in GDPR tab https://github.com/veda-consulting-company/uk.co.vedaconsulting.gdpr/issues/85 (See notes about Exporting data in Usage section above. Also you need to rebuild menu if you are upgrading from previous versions)

v3.0
Includes:
* Value for 'is_deceased' of the contact is left unchanged while a contact is being Anonymized # 235
* Check for the existence of the selected profile # 231
* Fix for the Event Registration form # 211

v2.9
Includes:
* Added the setting to choose whether exports should be tracked.

v2.8

Includes:
* Save information that data was exported - #157
* Various fixes.

v2.7

Includes:
* Data Policy/T&C web page link instead of uploaded file - #97
* Capture T&C condition field for new contacts - #123
* Fatal Error making contribution after redirect #118
* Anon overwriting current user #121,  Inherit profile's duplicate match option #127
* Comms prefs in contributions and events #129
* Update all privacy options during 'Forget Me' process #132, #133
* Forget me should clean out selected custom groups #146
* Transalation updates
* What to do with email in case of Forget me action #145
* Allow "Forget Me" to optionally delete some activity types #13
* Add missing data check and means to fix it. #167
* Default terms and condition text #172, #165
* Navigation Menu item for multi domain #183

v2.6

Issues

* Currently 'forget contact' permission in contact GDPR tab requires 'administer GDPR' permission https://github.com/veda-consulting/uk.co.vedaconsulting.gdpr/issues/114
* Extra div tags on event and contribution pages https://github.com/veda-consulting/uk.co.vedaconsulting.gdpr/issues/104
* Notice and empty "search" on dashboard https://github.com/veda-consulting/uk.co.vedaconsulting.gdpr/issues/100
* Bad redirect after fill communication preferences form https://github.com/veda-consulting/uk.co.vedaconsulting.gdpr/issues/99
* Missing Translations https://github.com/veda-consulting/uk.co.vedaconsulting.gdpr/issues/110

Features

* Allow Forget Me to optionally delete some activity types
* Make address history button optional https://github.com/veda-consulting/uk.co.vedaconsulting.gdpr/issues/108
* GDRP & Captcha https://github.com/veda-consulting/uk.co.vedaconsulting.gdpr/issues/103


v2.5

* README Updated for failed 2.4 release

v2.4

* Confirm Payment button locks when terms not checked  https://github.com/veda-consulting/uk.co.vedaconsulting.gdpr/issues/84
* Incorrect Data Policy file path stored in GDPR settings  https://github.com/veda-consulting/uk.co.vedaconsulting.gdpr/issues/80
* Missing T&C tab on Contributions in 4.6 https://github.com/veda-consulting/uk.co.vedaconsulting.gdpr/issues/78
* Some tokens not working when this extension is enabled https://github.com/veda-consulting/uk.co.vedaconsulting.gdpr/issues/73

v2.3
* Navigation from Dashboard to settings page requires administer CiviCRM permission https://github.com/veda-consulting/uk.co.vedaconsulting.gdpr/issues/64
* Fatal error when event registered https://github.com/veda-consulting/uk.co.vedaconsulting.gdpr/issues/62
* Contact GDPR Tab to only display public group history https://github.com/veda-consulting/uk.co.vedaconsulting.gdpr/issues/61
* Data policy text/label changes not reflected on the comms preferences page https://github.com/veda-consulting/uk.co.vedaconsulting.gdpr/issues/59
* Communications Preferences Settings Preview link not working https://github.com/veda-consulting/uk.co.vedaconsulting.gdpr/issues/58
* Actions dropdown disabled with selected record only https://github.com/veda-consulting/uk.co.vedaconsulting.gdpr/issues/56

v2.2
* Bulk Email Token fix
* Documentation Updates
* Rename DPO contact reference
* Wordpress front end URL for comms preferences
* Only show active profiles on the comms references confiuration page
* CiviCRM 4.6 IM call conditioned

v2.1
* Various fixes
* 4.6 Compatibility
