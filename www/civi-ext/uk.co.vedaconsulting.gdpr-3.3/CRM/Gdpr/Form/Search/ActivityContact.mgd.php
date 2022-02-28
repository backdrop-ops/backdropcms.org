<?php
// This file declares a managed database record of type "CustomSearch".
// The record will be automatically inserted, updated, or deleted from the
// database as appropriate. For more details, see "hook_civicrm_managed" at:
// http://wiki.civicrm.org/confluence/display/CRMDOC42/Hook+Reference
use CRM_Gdpr_ExtensionUtil as E;

return [
  0 => 
  [
    'name' => 'CRM_Gdpr_Form_Search_ActivityContact',
    'entity' => 'CustomSearch',
    'params' => 
    [
      'version' => 3,
      'label' => 'ActivityContact',
      'description' => E::ts('Contacts without Activity for a period (GDPR)'),
      'class_name' => 'CRM_Gdpr_Form_Search_ActivityContact',
    ],
  ],
];
