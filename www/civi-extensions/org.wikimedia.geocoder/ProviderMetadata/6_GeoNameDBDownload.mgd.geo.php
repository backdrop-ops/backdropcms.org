<?php
/**
 * Created by IntelliJ IDEA.
 * User: emcnaughton
 * Date: 1/25/18
 * Time: 10:51 AM
 */
return [
  [
    'name' => 'geonames_db_table',
    'entity' => 'Geocoder',
    'help_text' => ts('Additional config is required to set this up - you need to get a data set from http://download.geonames.org/export/zip/ & import it into your civicrm DB. Default table name is civicrm_geonames_lookup. A dataset for NZ ships with the extension as a sample. You need to load it though'),
    'params' => [
      'version' => 3,
      'name' => 'geonames_db_table',
      'title' => 'Geonames downloaded database',
      'class' => 'DataTable\DataTable',
      'valid_countries' => ['NZ'],
      'required_fields' => ['postal_code'],
      'retained_response_fields' => ['geo_code_1', 'geo_code_2'],
      'datafill_response_fields' => ['city'],
    ],
    'metadata' => [
      'argument' => ['pass_through' => ['tableName' => 'civicrm_geonames_lookup', 'columns' => ['city', 'latitude', 'longitude']]],
      'is_enabled_on_install' => FALSE,
    ]
  ]
];
