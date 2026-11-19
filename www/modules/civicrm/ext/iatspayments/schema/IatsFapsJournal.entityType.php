<?php
use CRM_Iats_ExtensionUtil as E;

return [
  'name' => 'IatsFapsJournal',
  'table' => 'civicrm_iats_faps_journal',
  'class' => 'CRM_Iats_DAO_IatsFapsJournal',
  'getInfo' => fn() => [
    'title' => E::ts('IatsFapsJournal'),
    'title_plural' => E::ts('IatsFapsJournals'),
    'description' => E::ts('Table of iATS/FAPS transactions imported via the query api.'),
    'log' => TRUE,
  ],
  'getFields' => fn() => [
    'id' => [
      'title' => E::ts('ID'),
      'sql_type' => 'int unsigned',
      'input_type' => 'Number',
      'required' => TRUE,
      'description' => E::ts('CiviCRM Journal Id'),
      'primary_key' => TRUE,
      'auto_increment' => TRUE,
    ],
    'transactionId' => [
      'title' => E::ts('Transaction ID'),
      'sql_type' => 'int unsigned',
      'input_type' => 'Number',
      'description' => E::ts('FAPS Transaction Id'),
    ],
    'authCode' => [
      'title' => E::ts('Authentication Code'),
      'sql_type' => 'varchar(255)',
      'description' => E::ts('Authentication Code'),
      'required' => true,
    ],
    'isAch' => [
      'title' => E::ts('Is ACH Transaction'),
      'sql_type' => 'boolean',
      'input_type' => 'Radio',
      'required' => TRUE,
      'default' => '0',
      'description' => E::ts('Transaction type: is ACH'),
    ],
    'cardType' => [
      'title' => E::ts('Cartd Type'),
      'sql_type' => 'varchar(255)',
      'description' => E::ts('Card Type'),
      'required' => true,
    ],
    'processorId' => [
      'title' => E::ts('Unique merchant account Identifier'),
      'sql_type' => 'varchar(255)',
      'description' => E::ts('Unique merchant account Identifier'),
      'required' => true,
    ],
    'cimRefNumber' => [
      'title' => E::ts('CIM Reference Number'),
      'sql_type' => 'varchar(255)',
      'description' => E::ts('CIM Reference Number'),
      'required' => true,
    ],
    'orderId' => [
      'title' => E::ts('Order Id = Invoice Number'),
      'sql_type' => 'varchar(255)',
      'description' => E::ts('Order Id = Invoice Number'),
    ],
    'transDateAndTime' => [
      'title' => E::ts('Date Time'),
      'sql_type' => 'datetime',
      'description' => E::ts('Datetime'),
      'required' => true,
    ],
    'Amount' => [
      'title' => E::ts('Amount'),
      'sql_type' => 'decimal(20,2)',
      'description' => E::ts('Amount'),
    ],
    'authResponse' => [
      'title' => E::ts('Response'),
      'sql_type' => 'varchar(255)',
      'description' => E::ts('Response'),
    ],
    'currency' => [
      'title' => E::ts('Currency'),
      'sql_type' => 'varchar(3)',
      'description' => E::ts('Currency'),
    ],
    'status_id' => [
      'title' => E::ts('Transaction Status'),
      'sql_type' => 'int unsigned',
      'description' => E::ts('Status of the payment'),
      'required' => true,
      'default' => '0',
    ],
    'financial_trxn_id' => [
      'title' => E::ts('Financial Trxn ID'),
      'sql_type' => 'int unsigned',
      'description' => E::ts('Foreign key into CiviCRM financial trxn table id'),
      'input_type' => 'EntityRef',
      'entity_reference' => [
        'entity' => 'FinancialTrxn',
        'key' => 'id',
        'on_delete' => 'SET NULL',
      ],
    ],
    'cid' => [
      'title' => E::ts('CiviCRM contact id'),
      'sql_type' => 'int unsigned',
      'description' => E::ts('CiviCRM contact id'),
      'input_type' => 'EntityRef',
      'entity_reference' => [
        'entity' => 'Contact',
        'key' => 'id',
        'on_delete' => 'SET NULL',
      ],
    ],
    'contribution_id' => [
      'title' => E::ts('Contribution ID'),
      'sql_type' => 'int unsigned',
      'description' => E::ts('CiviCRM contribution table id'),
      'input_type' => 'EntityRef',
      'entity_reference' => [
        'entity' => 'Contribution',
        'key' => 'id',
        'on_delete' => 'SET NULL',
      ],
    ],
    'recur_id' => [
      'title' => E::ts('Recurrring Contribution ID'),
      'sql_type' => 'int unsigned',
      'description' => E::ts('CiviCRM recurring_contribution table id'),
      'input_type' => 'EntityRef',
      'entity_reference' => [
        'entity' => 'ContributionRecur',
        'key' => 'id',
        'on_delete' => 'SET NULL',
      ],
    ],
    'verify_datetime' => [
      'title' => E::ts('Date time of Verification'),
      'sql_type' => 'datetime',
      'description' => E::ts('Date time of Verification'),
      'required' => true,
    ],
  ],
  'getIndices' => fn() => [
    'index_tranactionId_processor_id_authResponse' => [
      'unqiue' => TRUE,
      'fields' => [
        'transactionId' => TRUE,
        'processorId' => TRUE,
        'authResponse' => TRUE,
      ],
    ],
    'index_isAch' => [
      'fields' => [
        'isAch' => TRUE,
      ],
    ],
    'index_cardType' => [
      'fields' => [
        'cardType' => TRUE,
      ],
    ],
    'index_authResponse' => [
      'fields' => [
        'authResponse' => TRUE,
      ],
    ],
    'index_orderId' => [
      'fields' => [
        'orderId' => TRUE,
      ],
    ],
    'index_transDateAndTime' => [
      'fields' => [
        'transDateAndTime' => TRUE,
      ],
    ],
    'index_verify_datetime' => [
      'fields' => [
        'verify_datetime' => TRUE,
      ],
    ],
  ],
  'getPaths' => fn() => [],
];
