<?php

use CRM_Airmail_Utils as E;

/**
 * Form controller class
 *
 * @see https://wiki.civicrm.org/confluence/display/CRMDOC/QuickForm+Reference
 */
class CRM_Airmail_Form_Airmail_Settings extends CRM_Core_Form {

  public function buildQuickForm() {

    CRM_Core_Resources::singleton()->addScriptFile('com.aghstrategies.airmail', 'js/elastic.js');

    CRM_Utils_System::setTitle(E::ts('Airmail Settings'));

    // Add form Elements
    $attr = NULL;
    $secretCode = $this->add('text', 'secretcode', E::ts('Secret Code'), $attr, TRUE);
    $secretCode->setSize(40);
    $smtpService = $this->add('select', 'external_smtp_service', E::ts('External SMTP Service'), NULL, TRUE);
    $smtpService->loadArray(E::listBackends(TRUE));
    $this->addButtons(array(
      array(
        'type' => 'submit',
        'name' => E::ts('Save Configuration'),
        'isDefault' => TRUE,
      ),
    ));

    $eeUnsubscribeWrap = $this->addElement('checkbox', 'ee_wrapunsubscribe', E::ts('Elastic Email’s Track Stats Only flag has been negotiated for the sending domain.'));
    $eeUnsubscribeExplainer = $this->add('textarea', 'ee_unsubscribe', E::ts('Elastic Email unsubscribe link'), ['rows' => 5, 'cols' => 80]);

    $settings = E::getSettings();
    $this->setDefaults($settings);
    $this->assign('url', $this->getUrl());

    // export form elements
    $this->assign('elementNames', $this->getRenderableElementNames());
    parent::buildQuickForm();
  }

  /**
   * Compile what the endpoint URL should be
   *
   * @return string
   *   The URL for the webhook endpoint.
   */
  public function getUrl() {
    $settings = E::getSettings();
    $q = ['reset' => 1];
    if (!empty($settings['secretcode'])) {
      $q['secretcode'] = $settings['secretcode'];
    }
    $q = http_build_query($q, '', '&', PHP_QUERY_RFC3986);
    return CRM_Utils_System::url('civicrm/airmail/webhook', $q, TRUE, NULL, FALSE, TRUE);
  }

  public function postProcess() {
    // save settings to database
    $vars = $this->getSubmitValues();
    $settings = E::getSettings();
    foreach ($settings as $k => $oldValue) {
      $settings[$k] = $vars[$k] ?? NULL;
    }
    E::saveSettings($settings);

    parent::postProcess();

    // Reset the form so the URL reflects the changed secret code:
    CRM_Utils_System::redirect(CRM_Utils_System::url('civicrm/airmail/settings', 'reset=1'));
  }

  /**
   * Get the fields/elements defined in this form.
   *
   * @return array (string)
   */
  public function getRenderableElementNames() {
    // The _elements list includes some items which should not be
    // auto-rendered in the loop -- such as "qfKey" and "buttons".  These
    // items don't have labels.  We'll identify renderable by filtering on
    // the 'label'.
    $elementNames = array();
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
