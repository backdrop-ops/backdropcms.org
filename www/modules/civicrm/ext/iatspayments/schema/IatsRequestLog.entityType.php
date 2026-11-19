<?php
use CRM_Iats_ExtensionUtil as E;

return [
  'name' => 'IatsRequestLog',
  'table' => 'civicrm_iats_request_log',
  'class' => 'CRM_Iats_DAO_IatsRequestLog',
  'getInfo' => fn() => [
    'title' => E::ts('IatsRequestLog'),
    'title_plural' => E::ts('IatsRequestLogs'),
    'description' => E::ts('Table for request log'),
    'log' => TRUE,
  ],
  'getFields' => fn() => [
    'id' => [
      'title' => E::ts('ID'),
      'sql_type' => 'int unsigned',
      'input_type' => 'Number',
      'required' => TRUE,
      'description' => E::ts('Unique IatsRequestLog ID'),
      'primary_key' => TRUE,
      'auto_increment' => TRUE,
    ],
    'invoice_num' => [
      'title' => E::ts('Invoice Number'),
      'sql_type' => 'varchar(255)',
      'input_type' => 'Number',
      'description' => E::ts('Invoice number being sent to iATS'),
      'required' => TRUE,
    ],
    'ip' => [
      'title' => E::ts('IP Address'),
      'sql_type' => 'varchar(255)',
      'input_type' => 'Text',
      'description' => E::ts('IP from which this request originated'),
    ],
    'cc' => [
      'title' => E::ts('last Four Digits of Credit Card Number'),
      'sql_type' => 'varchar(4)',
      'input_type' => 'Number',
      'description' => E::ts('CC last four digits'),
    ],
    'customer_code' => [
      'title' => E::ts('Customer Code'),
      'sql_type' => 'varchar(255)',
      'input_type' => 'Number',
      'description' => E::ts('Customer code if used'),
      'required' => TRUE,
    ],
    'total' => [
      'title' => E::ts('Total'),
      'sql_type' => 'decimal(20,2)',
      'input_type' => 'Text',
      'description' => E::ts('Charge amount request'),
    ],
    'request_datetime' => [
      'title' => E::ts('Request DateTime'),
      'sql_type' => 'datetime',
      'input_type' => 'Timestamp',
      'description' => E::ts('Date time of request'),
    ],
  ],
  'getIndices' => fn() => [
    'index_invoice_num' => [
      'fields' => [
        'invoice_num' => TRUE,
      ],
    ],
    'index_cc' => [
      'fields' => [
        'cc' => TRUE,
      ],
    ],
    'index_request_datetime' => [
      'fields' => [
        'request_datetime' => TRUE,
      ],
    ],
    'index_customer_code' => [
      'fields' => [
        'customer_code' => TRUE,
      ],
    ],
    'index_total' => [
      'fields' => [
        'total' => TRUE,
      ],
    ],
  ],
  'getPaths' => fn() => [],
];
