<?php
/*
 +--------------------------------------------------------------------+
 | Copyright CiviCRM LLC. All rights reserved.                        |
 |                                                                    |
 | This work is published under the GNU AGPLv3 license with some      |
 | permitted exceptions and without any warranty. For full license    |
 | and copyright information, see https://civicrm.org/licensing       |
 +--------------------------------------------------------------------+
 */

/**
 *
 * @package CRM
 * @copyright CiviCRM LLC https://civicrm.org/licensing
 */

/**
 * Form helper class for custom data section.
 */
class CRM_Contact_Form_Inline_CustomData extends CRM_Contact_Form_Inline {

  /**
   * Custom group id.
   *
   * @var int
   */
  public $_groupID;

  /**
   * Entity type of the table id.
   *
   * @var string
   */
  protected $_entityType;

  /**
   * Call preprocess.
   */
  public function preProcess() {
    parent::preProcess();

    $this->_groupID = CRM_Utils_Request::retrieve('groupID', 'Positive', $this, TRUE, NULL);
    $this->assign('customGroupId', $this->_groupID);
    $customRecId = CRM_Utils_Request::retrieve('customRecId', 'Positive', $this, FALSE, 1);
    $cgcount = CRM_Utils_Request::retrieve('cgcount', 'Positive', $this, FALSE, 1);
    $subType = CRM_Contact_BAO_Contact::getContactSubType($this->_contactId, ',');
    CRM_Custom_Form_CustomData::preProcess($this, NULL, $subType, $cgcount,
      $this->_contactType, $this->_contactId);
  }

  /**
   * Build the form object elements for custom data.
   */
  public function buildQuickForm() {
    parent::buildQuickForm();
    CRM_Custom_Form_CustomData::buildQuickForm($this);
  }

  /**
   * Set defaults for the form.
   *
   * @return array
   */
  public function setDefaultValues() {
    return CRM_Custom_Form_CustomData::setDefaultValues($this);
  }

  /**
   * Process the form.
   */
  public function postProcess() {
    // Process / save custom data
    // Get the form values and groupTree
    $params = $this->getSubmittedValues();
    CRM_Core_BAO_CustomValueTable::postProcess($params,
      'civicrm_contact',
      $this->_contactId,
      $this->_entityType
    );

    $this->log();

    CRM_Contact_BAO_GroupContactCache::opportunisticCacheFlush();

    $this->response();
  }

}
