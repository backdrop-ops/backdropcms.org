<?php
use CRM_Gdpr_ExtensionUtil as E;
require_once 'CRM/Core/Form.php';

/**
 * Form controller class
 *
 * @see http://wiki.civicrm.org/confluence/display/CRMDOC43/QuickForm+Reference
 */
class CRM_Gdpr_Form_Activitycontact extends CRM_Core_Form {
  function buildQuickForm() {

    // Get GDPR activity types
    $gdprActTypes = CRM_Gdpr_Utils::getGDPRActivityTypes();
    $gdprActTypesStr = implode(', ', $gdprActTypes);
    $this->assign('gdprActTypes', $gdprActTypesStr);

    // Get GDPR settings
    $settings = CRM_Gdpr_Utils::getGDPRSettings();

    $this->add('text', 'contact_name', E::ts('Contact Name'),
      CRM_Core_DAO::getAttribute('CRM_Batch_DAO_Batch', 'title')
    );

    $this->addButtons(
      [
        [
          'type' => 'refresh',
          'name' => E::ts('Search'),
          'isDefault' => TRUE,
        ],
      ]
    );
    
    parent::buildQuickForm();
    $this->assign('suppressForm', TRUE);
    $this->assign('settings', $settings);
  }

  function postProcess() {
    $values = $this->exportValues();
  }
}
