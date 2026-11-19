<?php
use CRM_Iats_ExtensionUtil as E;

return [
  'name' => 'IatsVerify',
  'table' => 'civicrm_iats_verify',
  'class' => 'CRM_Iats_DAO_IatsVerify',
  'getInfo' => fn() => [
    'title' => E::ts('IatsVerify'),
    'title_plural' => E::ts('IatsVerifies'),
    'description' => E::ts('Table to store verification information'),
    'log' => TRUE,
  ],
  'getFields' => fn() => [
    'id' => [
      'title' => E::ts('ID'),
      'sql_type' => 'int unsigned',
      'input_type' => 'Number',
      'required' => TRUE,
      'description' => E::ts('Unique IatsVerify ID'),
      'primary_key' => TRUE,
      'auto_increment' => TRUE,
    ],
    'customer_code' => [
      'title' => E::ts('Customer Code'),
      'sql_type' => 'varchar(255)',
      'input_type' => 'Number',
      'description' => E::ts('Customer code returned from iATS'),
      'required' => TRUE,
    ],
    'cid' => [
      'title' => E::ts('Contact ID'),
      'sql_type' => 'int unsigned',
      'input_type' => 'EntityRef',
      'description' => E::ts('CiviCRM contact id'),
      'entity_reference' => [
        'entity' => 'Contact',
        'key' => 'id',
        'on_delete' => 'SET NULL',
      ],
    ],
    'contribution_id' => [
      'title' => E::ts('Contribution ID'),
      'sql_type' => 'int unsigned',
      'input_type' => 'EntityRef',
      'description' => E::ts('CiviCRM contribution table id'),
      'entity_reference' => [
        'entity' => 'Contribution',
        'key' => 'id',
        'on_delete' => 'SET NULL',
      ],
    ],
    'recur_id' => [
      'title' => E::ts('Contribution Recur ID'),
      'sql_type' => 'int unsigned',
      'input_type' => 'EntityRef',
      'description' => E::ts('CiviCRM recurring_contribution table id'),
      'entity_reference' => [
        'entity' => 'ContributionRecur',
        'key' => 'id',
        'on_delete' => 'SET NULL',
      ],
    ],
    'contribution_status_id' => [
      'title' => E::ts('Contribution Status ID'),
      'description' => E::ts('CiviCRM new status id'),
      'sql_type' => 'int(10)',
      'input_type' => 'Text',
      'default' => '0',
    ],
    'verify_datetime' => [
      'title' => E::ts('Verify DateTime'),
      'sql_type' => 'datetime',
      'input_type' => 'Timestamp',
      'description' => E::ts('Date time of verification'),
    ],
    'auth_result' => [
      'title' => E::ts('Authentication Result'),
      'sql_type' => 'varchar(255)',
      'input_type' => 'Text',
      'description' => E::ts('Authorization string returned from iATS'),
    ],
  ],
  'getIndices' => fn() => [
    'index_customer_code' => [
      'fields' => [
        'customer_code' => TRUE,
      ],
    ],
    'index_cid' => [
      'fields' => [
        'cid' => TRUE,
      ],
    ],
    'index_contribution_id' => [
      'fields' => [
        'contribution_id' => TRUE,
      ],
    ],
    'index_recur_id' => [
      'fields' => [
        'recur_id' => TRUE,
      ],
    ],
    'index_auth_result' => [
      'fields' => [
        'auth_result' => TRUE,
      ],
    ],
  ],
  'getPaths' => fn() => [],
];
