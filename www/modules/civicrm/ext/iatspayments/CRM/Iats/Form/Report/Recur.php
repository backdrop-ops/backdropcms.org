<?php

/**
 * @file
 * +--------------------------------------------------------------------+
 * | CiviCRM version 4.4                                                |
 * +--------------------------------------------------------------------+
 * | Copyright CiviCRM LLC (c) 2004-2013                                |
 * +--------------------------------------------------------------------+
 * | This file is a part of CiviCRM.                                    |
 * |                                                                    |
 * | CiviCRM is free software; you can copy, modify, and distribute it  |
 * | under the terms of the GNU Affero General Public License           |
 * | Version 3, 19 November 2007 and the CiviCRM Licensing Exception.   |
 * |                                                                    |
 * | CiviCRM is distributed in the hope that it will be useful, but     |
 * | WITHOUT ANY WARRANTY; without even the implied warranty of         |
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.               |
 * | See the GNU Affero General Public License for more details.        |
 * |                                                                    |
 * | You should have received a copy of the GNU Affero General Public   |
 * | License and the CiviCRM Licensing Exception along                  |
 * | with this program; if not, contact CiviCRM LLC                     |
 * | at info[AT]civicrm[DOT]org. If you have questions about the        |
 * | GNU Affero General Public License or the licensing of CiviCRM,     |
 * | see the CiviCRM license FAQ at http://civicrm.org/licensing        |
 * +--------------------------------------------------------------------+.
 */

/**
 *
 * @package CRM
 * @copyright CiviCRM LLC (c) 2004-2013
 * $Id$
 */
class CRM_Iats_Form_Report_Recur extends CRM_Report_Form {

  protected $_customGroupExtends = ['Contact', 'Individual'];

  static private $processors = [];
  static private $version = [];
  static private $financial_types = [];
  static private $prefixes = [];
  static private $contributionStatus = [];

  /**
   *
   */
  public function __construct() {

    self::$version = _iats_civicrm_domain_info('version');
    self::$financial_types = (self::$version[0] <= 4 && self::$version[1] <= 2) ? [] : CRM_Contribute_PseudoConstant::financialType();
    if (self::$version[0] <= 4 && self::$version[1] < 4) {
      self::$prefixes = CRM_Core_PseudoConstant::individualPrefix();
      self::$contributionStatus = CRM_Contribute_PseudoConstant::contributionStatus();
    }
    else {
      self::$prefixes = CRM_Contact_BAO_Contact::buildOptions('individual_prefix_id');
      self::$contributionStatus = CRM_Contribute_BAO_Contribution::buildOptions('contribution_status_id');
    }

    $params = ['version' => 3, 'sequential' => 1, 'is_test' => 0, 'return.name' => 1];
    $result = civicrm_api('PaymentProcessor', 'get', $params);
    foreach ($result['values'] as $pp) {
      self::$processors[$pp['id']] = $pp['name'];
    }
    $this->_columns = [
      'civicrm_contact' => [
        'dao' => 'CRM_Contact_DAO_Contact',
        'order_bys' => [
          'sort_name' => [
            'title' => ts("Last name, First name"),
          ],
        ],
        'fields' => [
          'first_name' => [
            'title' => ts('First Name'),
          ],
          'last_name' => [
            'title' => ts('Last Name'),
          ],
          'prefix_id' => [
            'title' => ts('Prefix'),
          ],
          'sort_name' => [
            'title' => ts('Contact Name'),
            'no_repeat' => TRUE,
            'default' => TRUE,
          ],
          'id' => [
            'no_display' => TRUE,
            'required' => TRUE,
          ],
        ],
      ],
      'civicrm_email' => [
        'dao' => 'CRM_Core_DAO_Email',
        'order_bys' => [
          'email' => [
            'title' => ts('Email'),
          ],
        ],
        'fields' => [
          'email' => [
            'title' => ts('Email'),
            'no_repeat' => TRUE,
          ],
        ],
        'grouping' => 'contact-fields',
      ],
      'civicrm_phone' => [
        'dao' => 'CRM_Core_DAO_Phone',
        'fields' => [
          'phone' => [
            'title' => ts('Phone'),
            'no_repeat' => TRUE,
          ],
        ],
        'grouping' => 'contact-fields',
      ],
      'civicrm_contribution' => [
        'dao' => 'CRM_Contribute_DAO_Contribution',
        'fields' => [
          'id' => [
            // 'no_display' => TRUE,.
            'title' => ts('Contribution ID(s)'),
            'required' => TRUE,
            'dbAlias' => "GROUP_CONCAT(contribution_civireport.id SEPARATOR ', ')",
          ],
          'total_amount_sum' => [
	    'title' => ts('Amount - to date'),
	    'required' => TRUE,
	    'dbAlias' => "SUM(contribution_civireport.total_amount)",
	  ],
        ],
        'filters' => [
          'total_amount' => [
            'title' => ts('Total Amount'),
            'operatorType' => CRM_Report_Form::OP_FLOAT,
            'type' => CRM_Utils_Type::T_FLOAT,
          ],
        ],
      ],
      'civicrm_payment_token' =>
        [
          'dao' => 'CRM_Contribute_DAO_Contribution',
          'order_bys' => [
            'expiry_date' => [
              'title' => ts("Expiry Date"),
            ],
          ],
          'fields' =>
            [
              'token' => ['title' => 'customer code', 'default' => TRUE],
              'expiry_date' => ['title' => 'Expiry Date', 'default' => TRUE],
            ],
        ],
      'civicrm_contribution_recur' => [
        'dao' => 'CRM_Contribute_DAO_ContributionRecur',
        'order_bys' => [
          'id' => [
            'title' => ts("Series ID"),
          ],
          'amount' => [
            'title' => ts("Current Amount"),
          ],
          'start_date' => [
            'title' => ts('Start Date'),
          ],
          'modified_date' => [
            'title' => ts('Modified Date'),
          ],
          'next_sched_contribution_date' => [
            'title' => ts('Next Scheduled Contribution Date'),
          ],
          'cycle_day'  => [
            'title' => ts('Cycle Day'),
          ],
          'failure_count'  => [
            'title' => ts('Failure Count'),
          ],
          'payment_processor_id' => [
            'title' => ts('Payment Processor'),
          ],
        ],
        'fields' => [
          'id' => [
            // 'no_display' => TRUE,.
            'required' => TRUE,
            'title' => ts("Series ID"),
          ],
          'recur_id' => [
            'name' => 'id',
            'title' => ts('Series ID'),
          ],
          'invoice_id' => [
            'title' => ts('Invoice ID'),
            'default' => FALSE,
          ],
          'currency' => [
            'title' => ts("Currency"),
          ],
          'amount' => [
            'title' => ts('Amount'),
            'default' => TRUE,
          ],
	  'financial_type_id' => [
	    'title' => ts('Financial Type'),
	    'default' => TRUE,
	  ],
          'contribution_status_id' => [
            'title' => ts('Donation Status'),
          ],
          'frequency_interval' => [
            'title' => ts('Frequency interval'),
            'default' => TRUE,
          ],
          'frequency_unit' => [
            'title' => ts('Frequency unit'),
            'default' => TRUE,
          ],
          'installments' => [
            'title' => ts('Installments'),
            'default' => TRUE,
          ],
          'start_date' => [
            'title' => ts('Start Date'),
            'default' => TRUE,
          ],
          'create_date' => [
            'title' => ts('Create Date'),
          ],
          'modified_date' => [
            'title' => ts('Modified Date'),
          ],
          'cancel_date' => [
            'title' => ts('Cancel Date'),
          ],
          'next_sched_contribution_date' => [
            'title' => ts('Next Scheduled Contribution Date'),
            'default' => TRUE,
          ],
          'cycle_day'  => [
            'title' => ts('Cycle Day'),
          ],
          'failure_count' => [
            'title' => ts('Failure Count'),
          ],
          'failure_retry_date' => [
            'title' => ts('Failure Retry Date'),
          ],
          'payment_processor_id' => [
            'title' => ts('Payment Processor'),
            'default' => TRUE,
          ],
        ],
        'filters' => [
          'contribution_status_id' => [
            'title' => ts('Donation Status'),
            'operatorType' => CRM_Report_Form::OP_MULTISELECT,
            'options' => self::$contributionStatus,
            'default' => [5],
            'type' => CRM_Utils_Type::T_INT,
          ],
          'payment_processor_id' => [
            'title' => ts('Payment Processor'),
            'operatorType' => CRM_Report_Form::OP_MULTISELECT,
            'options' => self::$processors,
            'type' => CRM_Utils_Type::T_INT,
          ],
          'currency' => [
            'title' => 'Currency',
            'operatorType' => CRM_Report_Form::OP_MULTISELECT,
            'options' => CRM_Core_OptionGroup::values('currencies_enabled'),
            'default' => NULL,
            'type' => CRM_Utils_Type::T_STRING,
          ],
          'financial_type_id' => [
            'title' => ts('Financial Type'),
            'operatorType' => CRM_Report_Form::OP_MULTISELECT,
            'options'  => self::$financial_types,
            'type' => CRM_Utils_Type::T_INT,
          ],
          'frequency_unit' => [
            'title' => ts('Frequency Unit'),
            'operatorType' => CRM_Report_Form::OP_MULTISELECT,
            'options' => CRM_Core_OptionGroup::values('recur_frequency_units'),
	    'type' => CRM_Utils_Type::T_STRING,	  
          ],
          'next_sched_contribution_date' => [
            'title' => ts('Next Scheduled Contribution Date'),
            'operatorType' => CRM_Report_Form::OP_DATE,
            'type' => CRM_Utils_Type::T_DATE,
          ],
          'next_scheduled_day' => [
            'title' => ts('Next Scheduled Day'),
            'operatorType' => CRM_Report_Form::OP_INT,
            'type' => CRM_Utils_Type::T_INT,
          ],
          'cycle_day' => [
            'title' => ts('Cycle Day'),
            'operatorType' => CRM_Report_Form::OP_INT,
            'type' => CRM_Utils_Type::T_INT,
          ],
          'failure_count' => [
            'title' => ts('Failure Count'),
            'operatorType' => CRM_Report_Form::OP_INT,
            'type' => CRM_Utils_Type::T_INT,
          ],
          'start_date' => [
            'title' => ts('Start Date'),
            'operatorType' => CRM_Report_Form::OP_DATE,
            'type' => CRM_Utils_Type::T_DATE,
          ],
          'modified_date' => [
            'title' => ts('Modified Date'),
            'operatorType' => CRM_Report_Form::OP_DATE,
            'type' => CRM_Utils_Type::T_DATE,
          ],
          'cancel_date' => [
            'title' => ts('Cancel Date'),
            'operatorType' => CRM_Report_Form::OP_DATE,
            'type' => CRM_Utils_Type::T_DATE,
          ],
        ],
      ],
      'civicrm_address' => [
        'dao' => 'CRM_Core_DAO_Address',
        'fields' => [
          'street_address' => [
            'title' => ts('Address'),
            'default' => FALSE,
          ],
          'supplemental_address_1' => [
            'title' => ts('Supplementary Address Field 1'),
            'default' => FALSE,
          ],
          'supplemental_address_2' => [
            'title' => ts('Supplementary Address Field 2'),
            'default' => FALSE,
          ],
          'city' => [
            'title' => 'City',
            'default' => FALSE,
          ],
          'state_province_id' => [
            'title' => 'Province',
            'default' => FALSE,
            'alter_display' => 'alterStateProvinceID',
          ],
          'postal_code' => [
            'title' => 'Postal Code',
            'default' => FALSE,
          ],
          'country_id' => [
            'title' => 'Country',
            'default' => FALSE,
            'alter_display' => 'alterCountryID',
          ],
        ],
        'grouping' => 'contact-fields',
      ],
    ];
    if (empty(self::$financial_types)) {
      unset($this->_columns['civicrm_contribution_recur']['filters']['financial_type_id']);
    }
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
    $this->_from = "
      FROM civicrm_contact  {$this->_aliases['civicrm_contact']}
        INNER JOIN civicrm_contribution_recur   {$this->_aliases['civicrm_contribution_recur']}
          ON {$this->_aliases['civicrm_contact']}.id = {$this->_aliases['civicrm_contribution_recur']}.contact_id";
    $this->_from .= "
      LEFT JOIN civicrm_contribution  {$this->_aliases['civicrm_contribution']}
        ON ({$this->_aliases['civicrm_contribution_recur']}.id = {$this->_aliases['civicrm_contribution']}.contribution_recur_id AND 1 = {$this->_aliases['civicrm_contribution']}.contribution_status_id)";
    $this->_from .= "
      LEFT JOIN civicrm_email  {$this->_aliases['civicrm_email']}
        ON ({$this->_aliases['civicrm_contact']}.id = {$this->_aliases['civicrm_email']}.contact_id AND
          {$this->_aliases['civicrm_email']}.is_primary = 1 )";
    $this->_from .= "
      LEFT JOIN civicrm_address {$this->_aliases['civicrm_address']}
        ON ({$this->_aliases['civicrm_contact']}.id = {$this->_aliases['civicrm_address']}.contact_id AND
          {$this->_aliases['civicrm_address']}.is_primary = 1 )";
    $this->_from .= "
      LEFT JOIN civicrm_phone {$this->_aliases['civicrm_phone']}
        ON ({$this->_aliases['civicrm_contact']}.id = {$this->_aliases['civicrm_phone']}.contact_id AND
          {$this->_aliases['civicrm_phone']}.is_primary = 1)";
    $this->_from .= "
      LEFT JOIN civicrm_payment_token {$this->_aliases['civicrm_payment_token']}
        ON ({$this->_aliases['civicrm_payment_token']}.id = {$this->_aliases['civicrm_contribution_recur']}.payment_token_id)";
  }

  /**
   *
   */
  public function groupBy() {
    $this->_groupBy = "GROUP BY " . $this->_aliases['civicrm_contribution_recur'] . ".id";
  }

  /**
   *
   */
  public function alterDisplay(&$rows) {
    foreach ($rows as $rowNum => $row) {
      // Convert display name to links.
      if (array_key_exists('civicrm_contact_sort_name', $row) &&
        CRM_Utils_Array::value('civicrm_contact_sort_name', $rows[$rowNum]) &&
        array_key_exists('civicrm_contact_id', $row)
      ) {
        $url = CRM_Utils_System::url('civicrm/contact/view',
          'reset=1&cid=' . $row['civicrm_contact_id'],
          $this->_absoluteUrl
        );
        $rows[$rowNum]['civicrm_contact_sort_name_link'] = $url;
        $rows[$rowNum]['civicrm_contact_sort_name_hover'] = ts('View Contact Summary for this Contact.');
      }

      // Link to recurring series.
      if (($value = $row['civicrm_contribution_recur_id'] ?? NULL) &&
        CRM_Core_Permission::check('access CiviContribute')
      ) {
        $url = CRM_Utils_System::url("civicrm/contact/view/contributionrecur",
          "reset=1&id=" . $row['civicrm_contribution_recur_id'] .
          "&cid=" . $row['civicrm_contact_id'] .
          "&action=view&context=contribution&selectedChild=contribute",
          $this->_absoluteUrl
        );
        $rows[$rowNum]['civicrm_contribution_recur_id_link'] = $url;
        $rows[$rowNum]['civicrm_contribution_recur_id_hover'] = ts("View Details of this Recurring Series.");
        $entryFound = TRUE;
      }

      // Handle contribution status id.
      if ($value = $row['civicrm_contribution_recur_contribution_status_id'] ?? NULL) {
        $rows[$rowNum]['civicrm_contribution_recur_contribution_status_id'] = self::$contributionStatus[$value];
      }
      // handle financial type id
      if ($value = $row['civicrm_contribution_recur_financial_type_id'] ?? NULL) {
        $rows[$rowNum]['civicrm_contribution_recur_financial_type_id'] = self::$financial_types[$value];
      }
      // Handle processor id.
      if ($value = $row['civicrm_contribution_recur_payment_processor_id'] ?? NULL) {
        $rows[$rowNum]['civicrm_contribution_recur_payment_processor_id'] = self::$processors[$value];
      }
      // Handle address country and province id => value conversion.
      if ($value = $row['civicrm_address_country_id'] ?? NULL) {
        $rows[$rowNum]['civicrm_address_country_id'] = CRM_Core_PseudoConstant::country($value, FALSE);
      }
      if ($value = $row['civicrm_address_state_province_id'] ?? NULL) {
        $rows[$rowNum]['civicrm_address_state_province_id'] = CRM_Core_PseudoConstant::stateProvince($value, FALSE);
      }
      if ($value = $row['civicrm_contact_prefix_id'] ?? NULL) {
        $rows[$rowNum]['civicrm_contact_prefix_id'] = self::$prefixes[$value];
      }
    }
  }

}
