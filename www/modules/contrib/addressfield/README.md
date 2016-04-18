Address Field
=============

Provides a field for international postal addresses. Uses a subset of the top-level address elements defined in the [xNAL standard](http://xml.coverpages.org/xnal.html) (see below).

The field configuration lets you determine which elements of an address should be present in the data entry form and which ones should be rendered for display.

### Features

* Standardized storage of international postal addresses based on the xNAL
  standard (the same format used by Google Maps for geocoding).
* Per-country edit form and formatting of addresses.
* Proper formatting of address forms and output on a country by country basis.
* Feeds integration for address importing.
* Rich snippets / semantic markup via the Schema.org module.

This module does not store or manipulate geographic coordinates or integrate with GIS systems, but it works well in conjunction with modules that do, like Geocoder.

### Glossary

As mentioned, this module uses the xNAL (Extensible Name and Address Language) vocabulary for describing address information. The terms used to designate various parts of an address may not be immediately recognizable, so this quick glossary equates the address parts in xNAL to their equivalent U.S. address terms (with some being self-evident):

  country => Country (always required, 2 character ISO code)
  name_line => Full name (default name entry)
  first_name => First name
  last_name => Last name
  organisation_name => Company
  administrative_area => State / Province / Region (ISO code when available)
  sub_administrative_area => County / District (unused)
  locality => City / Town
  dependent_locality => Dependent locality (unused)
  postal_code => Postal code / ZIP Code
  thoroughfare => Street address
  premise => Apartment, Suite, Box number, etc.
  sub_premise => Sub premise (unused)


Installation
------------

- Install this module using the official Backdrop CMS instructions at
  https://backdropcms.org/guide/modules

- Add an Address Field to any entity (Node, User, or Term).

- Configure the Address Field to show or hide various parts of the address.


License
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.


Current Maintainers
-------------------

- Jen Lampton (https://github.com/jenlampton)
- Seeking additional maintainers.


Credits
-------

- Originally written for Drupal by Damien Tournoud (https://www.drupal.org/u/damien-tournoud).
- Drupal project maintained by Bojan Živanović (https://www.drupal.org/u/bojanz).
- Drupal project also maintained by Ryan Szrama (https://www.drupal.org/u/rszrama).
- Ported to Backdrop CMS by Jen Lampton (https://github.com/jenlampton).
