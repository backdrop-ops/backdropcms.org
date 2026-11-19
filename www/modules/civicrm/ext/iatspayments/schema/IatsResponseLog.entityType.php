<?php
use CRM_Iats_ExtensionUtil as E;

return [
  'name' => 'IatsResponseLog',
  'table' => 'civicrm_iats_response_log',
  'class' => 'CRM_Iats_DAO_IatsResponseLog',
  'getInfo' => fn() => [
    'title' => E::ts('IatsResponseLog'),
    'title_plural' => E::ts('IatsResponseLogs'),
    'description' => E::ts('FIXME'),
    'log' => TRUE,
  ],
  'getFields' => fn() => [
    'id' => [
      'title' => E::ts('ID'),
      'sql_type' => 'int unsigned',
      'input_type' => 'Number',
      'required' => TRUE,
      'description' => E::ts('Unique IatsResponseLog ID'),
      'primary_key' => TRUE,
      'auto_increment' => TRUE,
    ],
    'invoice_num' => [
      'title' => E::ts('Invoice Number'),
      'sql_type' => 'varchar(255)',
      'input_type' => 'Text',
      'description' => E::ts('Invoice number sent to iATS'),
      'required' => TRUE,
    ],
    'auth_result' => [
      'title' => E::ts('Authentication Result'),
      'sql_type' => 'varchar(255)',
      'input_type' => 'Text',
      'description' => E::ts('Authorization string returned from iATS'),
      'required' => TRUE,
    ],
    'remote_id' => [
      'title' => E::ts('Iats Remote ID'),
      'sql_type' => 'varchar(255)',
      'input_type' => 'Number',
      'description' => E::ts('iATS-internal transaction id'),
      'required' => TRUE,
    ],
    'response_datetime' => [
      'title' => E::ts('Response DateTime'),
      'sql_type' => 'datetime',
      'input_type' => 'Timestamp',
      'description' => E::ts('Date time of response'),
    ],
  ],
  'getIndices' => fn() => [
    'index_invoice_num' => [
      'fields' => [
        'invoice_num' => TRUE,
      ],
    ],
    'index_auth_result' => [
      'fields' => [
        'auth_result' => TRUE,
      ],
    ],
    'index_remote_id' => [
      'fields' => [
        'remote_id' => TRUE,
      ],
    ],
    'index_response_datetime' => [
      'fields' => [
        'response_datetime' => TRUE,
      ],
    ],
  ],
  'getPaths' => fn() => [],
];
