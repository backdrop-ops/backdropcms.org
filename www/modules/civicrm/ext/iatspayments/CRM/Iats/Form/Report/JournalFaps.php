<?php

require_once('CRM/Report/Form.php');

/**
 * @file
 */

/**
 *
 * $Id$
 */
class CRM_Iats_Form_Report_JournalFaps extends CRM_Report_Form {

  // protected $_customGroupExtends = array('Contact');

  /* static private $processors = array();
  static private $version = array();
  static private $financial_types = array();
  static private $prefixes = array(); */
  static private $contributionStatus = [];
  static private $card_types = [
    'Visa' => 'Visa',
    'Mastercard' => 'MasterCard',
    'AMEX' => 'AMEX',
    'Discover' => 'Discover',
  ];

  /**
   *
   */
  public function __construct() {
    self::$contributionStatus = CRM_Contribute_BAO_Contribution::buildOptions('contribution_status_id');
    $this->_columns = [
      'civicrm_iats_faps_journal' =>
        [
          'fields' =>
            [
              'id' => ['title' => 'CiviCRM Journal Id', 'default' => TRUE],
              'transactionId' => ['title' => '1stPay Transaction Id', 'default' => TRUE],
              'isAch' => ['title' => 'isACH', 'default' => TRUE],
              'processorId' => ['title' => 'Processor Id', 'default' => TRUE],
              'cimRefNumber' => ['title' => 'Customer code', 'default' => TRUE],
              'orderId' => ['title' => 'Invoice Reference', 'default' => TRUE],
              'transDateAndTime' => ['title' => 'Transaction date', 'default' => TRUE],
              'amount' => ['title' => 'Amount', 'default' => TRUE],
              'authResponse' => ['title' => 'Response string', 'default' => TRUE],
              'currency' => ['title' => 'Currency', 'default' => TRUE],
              'status_id' => ['title' => 'Payment Status', 'default' => TRUE],
            ],
          'order_bys' => 
            [
              'id' => ['title' => ts('CiviCRM Journal Id'), 'default' => TRUE, 'default_order' => 'DESC'],
              'transactionId' => ['title' => ts('1stPay Transaction Id')],
              'transDateAndTime' => ['title' => ts('Transaction Date Time')],
            ],
          'filters' =>
             [
               'transDateAndTime' => [
                 'title' => 'Transaction date', 
                 'operatorType' => CRM_Report_Form::OP_DATE,
                 'type' => CRM_Utils_Type::T_DATE,
               ],
               'orderId' => [
                 'title' => 'Invoice Reference', 
                 'type' => CRM_Utils_Type::T_STRING,
               ],
               'amount' => [
                 'title' => 'Amount', 
                 'operatorType' => CRM_Report_Form::OP_FLOAT,
                 'type' => CRM_Utils_Type::T_FLOAT
               ],
               /*'isAch' => array(
                 'title' => 'Type', 
                 'operatorType' => CRM_Report_Form::OP_MULTISELECT,
                 'options' => self::$transaction_types,
                 'type' => CRM_Utils_Type::T_STRING,
               ), */
               'processorId' => [
                 'title' => 'Processor Id',
                 'type' => CRM_Utils_Type::T_STRING,
               ],
               'authResponse' => [
                 'title' => 'Response string',
                 'type' => CRM_Utils_Type::T_STRING,
               ],
               'status_id' => [
                 'title' => ts('Payment Status'),
                 'operatorType' => CRM_Report_Form::OP_MULTISELECT,
                 'options' => self::$contributionStatus,
                 'type' => CRM_Utils_Type::T_INT,
               ],
             ],
        ],
    ];
    parent::__construct();
  }

  /**
   *
   */
  public function getTemplateName() {
    return 'CRM/Report/Form.tpl';
  }

  /**
   *
   */
  public function from() {
    $this->_from = "FROM civicrm_iats_faps_journal  {$this->_aliases['civicrm_iats_faps_journal']}";
  }

}
