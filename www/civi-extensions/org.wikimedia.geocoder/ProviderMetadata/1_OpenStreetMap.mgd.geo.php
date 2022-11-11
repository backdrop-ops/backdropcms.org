<?php
/**
 * Created by IntelliJ IDEA.
 * User: emcnaughton
 * Date: 1/19/18
 * Time: 4:33 PM
 */

return [
  [
    'name' => 'open_street_maps',
    'entity' => 'Geocoder',
    'params' => [
      'version' => 3,
      'name' => 'open_street_maps',
      'title' => 'Nominatim (Open street maps)',
      'class' => 'Nominatim\Nominatim',
      'url' => 'https://nominatim.openstreetmap.org/search',
    ],
    'metadata' => [
      'argument' => ['geocoder.url', 'server.User-Agent:CiviCRM', 'server.Referrer'],
      'required_config_fields' => ['url'],
      'is_enabled_on_install' => TRUE,
    ],
  ]
];
