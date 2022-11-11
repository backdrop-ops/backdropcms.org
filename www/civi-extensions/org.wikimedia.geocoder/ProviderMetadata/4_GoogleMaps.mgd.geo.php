<?php
/**
 * Created by IntelliJ IDEA.
 * User: emcnaughton
 * Date: 1/19/18
 * Time: 3:09 PM
 */
return [
  [
    'name' => 'google_maps',
    'entity' => 'Geocoder',
    'params' => [
      'version' => 3,
      'name' => 'google_maps',
      'title' => 'Google Maps',
      'class' => 'GoogleMaps\GoogleMaps',
    ],
    'help_text' => ts('Adhering to Terms of service is your responsibility - https://support.google.com/code/answer/55180?hl=en'),
    'user_editable_fields' => ['api_key', 'threshold_standdown'],
    'metadata' => [
      'argument' => 'geocoder.api_key',
      'required_config_fields' => ['api_key'],
      // Not enabled by default, but special handling will enable if api key is already configured.
      'is_enabled_on_install' => FALSE,
    ],
  ]
];
