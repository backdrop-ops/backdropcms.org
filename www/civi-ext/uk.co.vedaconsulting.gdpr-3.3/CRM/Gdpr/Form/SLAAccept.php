<?php

use CRM_Gdpr_ExtensionUtil as E;
use CRM_Gdpr_SLA_Utils as U;

/**
 * Form controller class
 *
 * @see https://wiki.civicrm.org/confluence/display/CRMDOC/QuickForm+Reference
 */
class CRM_Gdpr_Form_SLAAccept extends CRM_Core_Form {
  public function buildQuickForm() {

    $settings = CRM_Gdpr_Utils::getGDPRSettings();
    $title = U::getPageTitle(); 
    CRM_Utils_System::setTitle($title);
    $tc = U::getTermsConditionsUrl();
    $checkbox_text = U::getCheckboxText();
    $link_label = U::getLinkLabel();

    $this->assign('tc_url', $tc);
    $this->assign('tc_link_label', $link_label);

    $this->assign('agreement_text', $settings['sla_agreement_text']);
    $this->add(
      'checkbox',
      'accept_tc',
      $checkbox_text
    );
    $this->addButtons([
      [
        'type' => 'submit',
        'name' => E::ts('Submit'),
        'isDefault' => TRUE,
      ],
    ]);
    $this->addRule('accept_tc', E::ts('This field is required.'), 'required');

    // export form elements
    $this->assign('elementNames', $this->getRenderableElementNames());
    parent::buildQuickForm();
  }

  public function postProcess() {
    // Remove the flag to display the form. 
    CRM_Gdpr_SLA_Utils::unflagShowForm();
    $values = $this->exportValues();
    if (!empty($values['accept_tc'])) {
      CRM_Gdpr_SLA_Utils::recordSLAAcceptance();
    }
    parent::postProcess();
  }
  
  /**
   * Get the fields/elements defined in this form.
   *
   * @return [string]
   */
  public function getRenderableElementNames() {
    // The _elements list includes some items which should not be
    // auto-rendered in the loop -- such as "qfKey" and "buttons".  These
    // items don't have labels.  We'll identify renderable by filtering on
    // the 'label'.
    $elementNames = [];
    foreach ($this->_elements as $element) {
      /** @var HTML_QuickForm_Element $element */
      $label = $element->getLabel();
      if (!empty($label)) {
        $elementNames[] = $element->getName();
      }
    }
    return $elementNames;
  }

}
