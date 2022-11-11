<?php
/**
 * Created by IntelliJ IDEA.
 * User: emcnaughton
 * Date: 1/25/18
 * Time: 10:51 AM
 */
return [
  [
    'name' => 'us_zip_geocoder',
    'entity' => 'Geocoder',
    'params' => [
      'version' => 3,
      'name' => 'us_zip_geocoder',
      'title' => 'US Zip based geocoding',
      'class' => 'DataTable\DataTable',
      'valid_countries' => ['US'],
      'required_fields' => ['postal_code'],
      'retained_response_fields' => '["geo_code_1","geo_code_2", "timezone"]',
      'datafill_response_fields' => ["city", "state_province_id"],
    ],
    'metadata' => [
      'argument' => ['pass_through' => [
        'tableName' => 'civicrm_geocoder_zip_dataset',
        'columns' => ['city', 'state_code', 'latitude', 'longitude', 'timezone'],
      ]],
      'is_enabled_on_install' => TRUE,
    ]
  ]
];
