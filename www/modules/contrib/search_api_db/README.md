# Search API Database Search

This module provides a database based implementation of the Search API. The
database and target to use for storing and accessing the indexes can be selected
when creating a new server.

All Search API datatypes are supported by using appropriate SQL datatypes for
their respective columns (with "String"/"URI", and "Integer"/"Duration" being
equivalent).

The "direct" parse mode for queries will result in a simple splitting of the
query string into keys. Additionally, search keys containing whitespace will be
split, as searching for phrases is currently not supported.

## Installation

 - Install this module and its dependencies using the official
  [Backdrop CMS instructions](https://backdropcms.org/guide/modules)

## Configuration and Usage

### Hidden configuration variables

- `autocomplete_max_occurrences` (default: 0.9)
  By default, keywords that occur in more than 90% of results are ignored for
  autocomplete suggestions. This setting lets you modify that behaviour by
  providing your own ratio. Use 1 or greater to use all suggestions.

### Supported optional features

- `search_api_autocomplete`
  Introduced by module: search_api_autocomplete
  Lets you add autocompletion capabilities to search forms on the site. (See
  also "Hidden variables" below for backend-specific customization.)
  NOTE: Due to internal database restrictions, this will perform significantly
  better if only a single field is used for autocompletion.
- `search_api_facets`
  Introduced by module: search_api_facetapi
  Allows you to create facetted searches for dynamically filtering search
  results.

If you feel some service option is missing, or have other ideas for improving
this implementation, please file a feature request in the project's issue queue.

### Known problems

Using facets with a database server will only work if the database user
Backdrop is using has the "CREATE TEMPORARY TABLES" permission.

### Developer information

Database queries for searches with this module are tagged with
`search_api_db_search` to allow easy altering. As metadata, such database
queries will have the Search API query object set as `search_api_query`, and the
field settings of the server for the corresponding search index as
`search_api_db_fields`.

## Current maintainers

- [Laryn Kragt Bakker](https://github.com/laryn)

## Credits

 - Ported to Backdrop by [docwilmot](https://github.com/docwilmot)
 - Maintainer on Drupal [drunken monkey](https://www.drupal.org/u/drunken-monkey)

## License

This project is GPL v2 software. See the LICENSE.txt file in this directory
for complete text.

