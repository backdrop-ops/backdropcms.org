<?php

use CRM_Airmail_ExtensionUtil as E;

/**
 * Form controller class
 *
 * @see https://docs.civicrm.org/dev/en/latest/framework/quickform/
 */
class CRM_Airmail_Form_DeleteMyEmail extends CRM_Core_Form {
  /**
   * Prevent people double-submitting the form (e.g. by double-clicking).
   * https://lab.civicrm.org/dev/core/-/issues/1773
   *
   * @var bool
   */
  public $submitOnce = TRUE;

  protected $_hasBeenDone = FALSE;
  protected $_showForm = FALSE;

  public function preProcess() {
    if ($_GET['reset'] ?? 0) {
      $contactID = (int) ($_GET['cid'] ?? 0);
      $checksum = $_GET['cs'] ?? '';
      $emailID = (int) ($_GET['eid'] ?? 0);
      $error = NULL;

      if (($contactID > 0) && $checksum && ($emailID > 0)) {
        // Check checksum
        if (CRM_Contact_BAO_Contact_Utils::validChecksum($contactID, $checksum)) {
          // Check email exists.
          $email = \Civi\Api4\Email::get(FALSE)
            ->setCheckPermissions(FALSE)
            ->addWhere('id', '=', $emailID)
            ->addWhere('contact_id', '=', $contactID)
            ->execute()->first();
          if (!$email) {
            $error = E::ts('Your email has already been deleted.');
          }
          else {
            // Set data that then remains set throughout the form's lifecycle.
            $this->set('emailShown', preg_replace('/^(.).*?@(..).*(\.[^.]+)$/', '$1***@$2***$3', $email['email']));
            $this->set('contactID', $contactID);
            $this->set('emailID', $emailID);
            // This is only set during one phase, it is not stored for the later POST request.
            $this->_showForm = TRUE;
          }
        }
        else {
          $error = E::ts('This link has expired.');
        }
      }
      else {
        $error = E::ts('Invalid link.');
      }
    }

    if ($error) {
      CRM_Core_Session::setStatus($error, 'Error', 'crm-error no-popup');
    }
  }
  public function buildQuickForm() {
    $this->assign('showForm', $this->_showForm);
    $this->assign('email', $this->get('emailShown'));

    $this->addRadio(
      'optoutoptions', // field name
      'Opt-out option', // field title/label
      [
        'optout' => E::ts('Opt-out of all bulk mailings'),
        'delete' => E::ts('Opt-out of all bulk mailings and delete my email'),
      ],
      [],
      '<br><br>', // separator
      TRUE // is required
    );

    $this->setDefaults(['optoutoptions' => 'optout']);

    $this->addButtons(array(
      array(
        'type'      => 'submit',
        'name'      => E::ts('Submit'),
        'isDefault' => TRUE,
      ),
    ));

    // export form elements
    $this->assign('elementNames', $this->getRenderableElementNames());
    parent::buildQuickForm();
  }

  public function postProcess() {
    $values = $this->exportValues();

    // Always set optout
    $contactID = $this->get('contactID');
    if (!$contactID) {
      // Should never happen.
      throw new \InvalidArgumentException("Missing Contact ID");
    }
    \Civi\Api4\Contact::update(FALSE)
      ->setCheckPermissions(FALSE)
      ->addWhere('id', '=', $contactID)
      ->addValue('is_opt_out', TRUE)
      ->setLimit(1)
      ->execute();
    $message = E::ts("You have opted-out of all bulk emails");

    if ($values['optoutoptions'] === 'delete' && ($this->get('emailID') ?? NULL) > 0) {
      // Delete their email.
      \Civi\Api4\Email::delete(FALSE)
        ->setCheckPermissions(FALSE)
        ->addWhere('id', '=', $this->get('emailID'))
        ->addWhere('contact_id', '=', $contactID)
        ->execute();
      $message = E::ts("You have opted-out of all bulk emails and your email has been deleted.");
    }

    CRM_Core_Session::setStatus($message, 'Success', 'crm-success no-popup');
    parent::postProcess();

    CRM_Utils_System::redirect(CRM_Utils_System::url('civicrm/deletemyemail'));
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
