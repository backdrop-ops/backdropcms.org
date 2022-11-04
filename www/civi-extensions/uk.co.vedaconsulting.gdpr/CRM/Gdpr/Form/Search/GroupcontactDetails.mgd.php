<?php
use CRM_Gdpr_ExtensionUtil as E;

// This file declares a managed database record of type "CustomSearch".
// The record will be automatically inserted, updated, or deleted from the
// database as appropriate. For more details, see "hook_civicrm_managed" at:
// http://wiki.civicrm.org/confluence/display/CRMDOC42/Hook+Reference
return [
  0 =>
  [
    'name' => 'CRM_Gdpr_Form_Search_GroupcontactDetails',
    'entity' => 'CustomSearch',
    'params' =>
    [
      'version' => 3,
      'label' => 'GroupcontactDetails',
      'description' => E::ts('Search Group Subscription by Date Range'),
      'class_name' => 'CRM_Gdpr_Form_Search_GroupcontactDetails',
    ],
  ],
];
