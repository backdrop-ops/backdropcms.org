<?php
use CRM_Iats_ExtensionUtil as E;

return [
  'name' => 'IatsJournal',
  'table' => 'civicrm_iats_journal',
  'class' => 'CRM_Iats_DAO_IatsJournal',
  'getInfo' => fn() => [
    'title' => E::ts('IatsJournal'),
    'title_plural' => E::ts('IatsJournals'),
    'description' => E::ts('Table to iATS journal transactions imported via the iATSPayments ReportLink.'),
    'log' => TRUE,
  ],
  'getFields' => fn() => [
    'id' => [
      'title' => E::ts('ID'),
      'sql_type' => 'int unsigned',
      'input_type' => 'Number',
      'required' => TRUE,
      'description' => E::ts('Unique IatsJournal ID'),
      'primary_key' => TRUE,
      'auto_increment' => TRUE,
    ],
    'iats_id' => [
      'title' => E::ts('iATS ID'),
      'sql_type' => 'int unsigned',
      'input_type' => 'Number',
      'description' => E::ts('iATS Journal Id'),
    ],
    'tnid' => [
      'title' => E::ts('Transaction ID'),
      'sql_type' => 'varchar(255)',
      'input_type' => 'Text',
      'description' => E::ts('Transaction ID'),
      'required' => TRUE,
    ],
    'tntyp' => [
      'title' => E::ts('Transaction Type'),
      'sql_type' => 'varchar(255)',
      'input_type' => 'Text',
      'description' => E::ts('Transaction type: Credit card or ACHEFT'),
      'required' => TRUE,
    ],
    'agt' => [
      'title' => E::ts('Agent'),
      'sql_type' => 'varchar(255)',
      'input_type' => 'Text',
      'description' => E::ts('Agent'),
      'required' => TRUE,
    ],
    'cstc' => [
      'title' => E::ts('Customer Code'),
      'sql_type' => 'varchar(255)',
      'input_type' => 'Text',
      'description' => E::ts('Customer code'),
      'required' => TRUE,
    ],
    'inv' => [
      'title' => E::ts('Invoice Number'),
      'sql_type' => 'varchar(255)',
      'input_type' => 'Text',
      'description' => E::ts('Invoice Number'),
    ],
    'dtm' => [
      'title' => E::ts('Date Time'),
      'sql_type' => 'datetime',
      'input_type' => 'Text',
      'description' => E::ts('DateTime'),
      'required' => TRUE,
    ],
    'amt' => [
      'title' => E::ts('Amount'),
      'sql_type' => 'decimal(20,2)',
      'input_type' => 'Text',
      'description' => E::ts('Amount'),
    ],
    'rst' => [
      'title' => E::ts('Result'),
      'sql_type' => 'varchar(255)',
      'input_type' => 'Text',
      'description' => E::ts('Result'),
    ],
    'cm' => [
      'title' => E::ts('Comment'),
      'sql_type' => 'varchar(255)',
      'input_type' => 'Text',
      'description' => E::ts('Comment'),
    ],
    'currency' => [
      'title' => E::ts('Currency'),
      'sql_type' => 'varchar(3)',
      'input_type' => 'Text',
      'description' => E::ts('Currency'),
      'required' => TRUE,
    ],
    'status_id' => [
      'title' => E::ts('Status of the payment'),
      'sql_type' => 'int unsigned',
      'input_type' => 'Text',
      'description' => E::ts('Status of the payment'),
      'required' => TRUE,
      'default' => '0',
    ],
    'financial_trxn_id' => [
      'title' => E::ts('Foreign key into CiviCRM financial trxn table id'),
      'sql_type' => 'int unsigned',
      'input_type' => 'EntityRef',
      'entity_reference' => [
        'entity' => 'FinancialTrxn',
        'key' => 'id',
        'on_delete' => 'SET NULL',
      ],
      'description' => E::ts('Foreign key into CiviCRM financial trxn table id'),
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
    'verify_datetime' => [
      'title' => E::ts('Verify DateTime'),
      'sql_type' => 'datetime',
      'input_type' => 'Timestamp',
      'description' => E::ts('Date time of verification'),
    ],
  ],
  'getIndices' => fn() => [
    'index_tnid' => [
      'unique' => TRUE,
      'fields' => [
        'tnid' => TRUE,
      ],
    ],
    'index_iats_id' => [
      'unique' => TRUE,
      'fields' => [
        'iats_id' => TRUE,
      ],
    ],
    'index_tntyp' => [
      'fields' => [
        'tntyp' => TRUE,
      ],
    ],
    'index_inv' => [
      'fields' => [
        'inv' => TRUE,
      ],
    ],
    'index_rst' => [
      'fields' => [
        'rst' => TRUE,
      ],
    ],
    'index_dtm' => [
      'fields' => [
        'dtm' => TRUE,
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
