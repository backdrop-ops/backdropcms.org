# Repeating Dates

Ships with a custom field type for recurring (repeating) dates based on ICAL
rules.

Views support for individual date values.

![Widget screenshot](https://raw.githubusercontent.com/backdrop-contrib/repeating_dates/1.x-1.x/screenshots/screenshot-widget-weekly.png)

## Installation

Install this module using the official Backdrop CMS
 [instructions](https://docs.backdropcms.org/documentation/extend-with-modules)

Then you can add the "Date (repeating)" field to content types. Note that it's
 really a standalone field type - you can not use any of the other Date field
 types as base for recurring dates.

The field cardinality global setting controls the max amount of date values,
 that get saved to the database. Make sure that this value is high enough.

## Issues

Bugs and feature requests should be reported in the
 [Issue Queue](https://github.com/backdrop-contrib/repeating_dates/issues).

## Current maintainers

* [Indigoxela](https://github.com/indigoxela)

## Credits

To calculate the list of dates, this module uses the MIT licensed
 [RRULE for PHP](https://github.com/rlanvin/php-rrule) library by Rémi Lanvin
([rlanvin](https://github.com/rlanvin))

This module is inspired by, but in no way similar to Date Repeat, which is
 currently getting ported - but that turned out to be very difficult,
 as it's an incomplete stub.

## License

This project is GPL v2 software. See the LICENSE.txt file in this directory for complete text.
