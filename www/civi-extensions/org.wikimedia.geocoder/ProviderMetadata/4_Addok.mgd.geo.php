<?php
/**
 * User: olivier
 * Date: 6/1/19
 * Time: 3:09 PM
 */
return [
  [
    'name' => 'addok',
    'entity' => 'Geocoder',
    'params' => [
      'version' => 3,
      'name' => 'addok',
      'title' => 'Addok',
      'class' => 'Addok\Addok',
      'valid_countries' => ['1076'],
      'url' => 'https://api-adresse.data.gouv.fr',
      'retained_response_fields' => ['geo_code_1', 'geo_code_2'],
    ],
    'help_text' => ts('France only'),
    'metadata' => [
      'argument' => 'geocoder.url',
      'required_config_fields' => ['url'],
      'is_enabled_on_install' => FALSE,
    ],
  ]
];
