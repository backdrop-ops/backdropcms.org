<?php
/**
 * Created by IntelliJ IDEA.
 * User: emcnaughton
 * Date: 1/19/18
 * Time: 3:09 PM
 */
return [
  [
    'name' => 'mapquest',
    'entity' => 'Geocoder',
    'params' => [
      'version' => 3,
      'name' => 'mapquest',
      'title' => 'MapQuest',
      'class' => 'MapQuest\MapQuest',
    ],
    'help_text' => ts('api key required - 15000 for free per month - sign up https://developer.mapquest.com/plan_purchase/steps/business_edition/business_edition_free/register'),
    'metadata' => [
      // in addition to the api_key, add a flag to no longer use the open version...
      // Note it seems it might be better to store params here
      // per https://github.com/eileenmcnaughton/org.wikimedia.geocoder/pull/12/files#diff-753486e050c8154bc3527b60ad496b96R121
      'argument' => ['geocoder.api_key', 'pass_through' => TRUE],
      'required_config_fields' => ['api_key'],
      'is_enabled_on_install' => FALSE,
    ],
  ]
];
