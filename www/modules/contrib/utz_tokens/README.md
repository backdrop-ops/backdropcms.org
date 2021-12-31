User Time Zone Tokens
================

This module provides support for displaying a date or time in the user's own time zone in both formatted date fields and (via token text) within the body of any formatted text field.

A typical usage would be to give the time(s) of an event in the body of a page that will be seen by people in multiple time zones.

Logged-in users will see the date/time in their own time zone. Anonymous users will see it in the default time zone (typically the server time zone), but there is an option to detect their time zone as well.

Usage
-----

### Date Fields

User Time Zone Tokens provides a field formatter for the three Date type fields. Using the "UTZ Date and Time" formatter, you can render user-timezone-aware fields in (for example) node display pages and within Views.

You can use any of the built-in date formats or create your own custom formats. Note, though, that for date ranges (with starting and ending dates), you will define three date formats:

* Start date
* End date
* End date for intervals less than 24 hours.

The reason for the third format is to allow formats that include the year, month, and/or day to drop those values for intervals less than 24 hours. The core Date module does this dropping automatically; this module doesn't (due to how the rendered date ranges are sometimes constructed by Javascript).

### Tokens

Tokens are of the form

[utz-datetime:_datetime_|_format_]

where

* _datetime_ is a string specifying the time in any format suitable for initializing a [DateTime object](https://www.php.net/manual/en/class.datetime).
  * To initialize from a Unix timestamp, prepend '@' to the timestamp, e.g., `@1611363600`.
  * To initialize from a time string, be sure to include the time zone explicitly, e.g., `2021-01-01 12:00 PST`; otherwise the user's local time zone is assumed (which would defeat the purpose of using this token).

* _format_ is a string giving the desired output formatting, which can be either
  * The machine name of a Backdrop date/time format found at /admin/config/regional/date-time (e.g., `'short'`, `'medium'`, `'long'`), or
  * Any valid PHP formatting string. Formatting options are documented [in the PHP Manual](https://www.php.net/manual/en/datetime.format).

To initialize from a timestamp, you can convert a date to a timestamp using [EpochConverter](https://www.epochconverter.com).

Note that _datetime_ and _format_ are separated by a pipe (|), not a colon (:), because _datetime_ might contain a colon (typically separating hours and minutes).

You can use these tokens anywhere that tokens are accepted. To use the tokens in formatted text (the most common use case), install and enable the [Token Filter](https://backdropcms.org/project/token_filter) module.

### Timezone Detection for Anonymous Users

There is an option to automatically detect the user's time zone (which is provided by their browser) and use that for anonymous or all users, which works even for cached pages. To use this capability you will need to install the [Luxon](https://github.com/bugfolder/luxon) module. With Luxon installed and enabled, on the configuration page for this module you will have the option to detect the user's time zone for anonymous or all users. This capability requires Javascript.

Installation
------------

Install this module using [the official Backdrop CMS instructions](https://backdropcms.org/guide/modules).

To use the tokens in formatted text,

- Install and enable the [Token Filter](https://backdropcms.org/project/token_filter) module.
- Enable the "Replace tokens" filter for the text formats that you will be using these tokens in.

To automatically detect the time zone for anonymous or all users,

- Install and enable the [Luxon](https://github.com/bugfolder/luxon) module.
- Go to /admin/config/regional/utz-tokens and check the desired option.

Documentation
-------------

Additional documentation is located in [the Wiki](https://github.com/backdrop-contrib/utz_tokens/wiki/Documentation).

### Theme Function (for Developers)

Developers can incorporate the user-timezone-aware functionality of User Time Zone Tokens into their own rendering of dates/times by using the theme function, e.g.,

```
$datetime = '2021-04-01 12:00pm PDT' // or, for example, '@1617303600'
$format = 'l, F j, g:ia T'; // or, for example, 'long'
$output = theme('utz_datetime', array(
  'datetime' => $datetime,
  'format' => $format,
));
```

Issues
------

Bugs and feature requests should be reported in [the Issue Queue](https://github.com/backdrop-contrib/utz_tokens/issues).

Similar Modules
---------------

User Time Zone Tokens handles time-zone-aware dates and times embedded within content as token text and via the "UTZ Date and time" field formatter for fields provided by the core Date module. Another approach to applying time zone awareness to fields provided by the core Date module is the [Client Side Date Field Formatter](https://github.com/backdrop-contrib/cs_date_formatter) module.

Current Maintainers
-------------------

- [Robert J. Lang](https://github.com/bugfolder)

Credits
-------

- Written for Backdrop CMS by [Robert J. Lang](https://github.com/bugfolder).

License
-------

This project is GPL v2 software.
See the LICENSE.txt file in this directory for complete text.

