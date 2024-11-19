<?php

use CRM_Gdpr_ExtensionUtil as E;

/**
 * Form controller class
 *
 * @see https://wiki.civicrm.org/confluence/display/CRMDOC/QuickForm+Reference
 */
class CRM_Gdpr_Form_Forgetme extends CRM_Core_Form {

  /**
   * Contact ID.
   *
   * @var int
   */
  protected $_contactID = NULL;

  /**
   * Form preProcess function.
   *
   * @throws \Exception
   */
  public function preProcess() {

    // <!-- To DO - check permission -->

    $this->_contactID = CRM_Utils_Request::retrieve('cid', 'Positive', $this, TRUE);
  }

  public function buildQuickForm() {

    $this->addButtons([
      [
        'type' => 'next',
        'name' => E::ts('Forget me'),
        'spacing' => '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;',
        'isDefault' => TRUE,
      ],
      [
        'type' => 'cancel',
        'name' => E::ts('Cancel'),
      ],
    ]);
    parent::buildQuickForm();
  }

  public function postProcess() {

    if (!$this->_contactID) {
      CRM_Core_Error::fatal(E::ts("Something went wrong. Please contact Admin."));
    }

    // Remove all the linked relationship records of this contact
    $params = [
      'sequential' => 1,
      'contact_id_a' => $this->_contactID,
      'contact_id_b' => $this->_contactID,
      'options' => ['or' => [["contact_id_a", "contact_id_b"]]],
    ];
    self::removeEntityRecords('Relationship', $params);

    // Remove all the address records of this contact
    $params = [
      'sequential' => 1,
      'contact_id' => $this->_contactID,
    ];
    self::removeEntityRecords('Address', $params);

    // Remove all the IM records of this contact
    $params = [
      'sequential' => 1,
      'contact_id' => $this->_contactID,
    ];
    self::removeEntityRecords('Im', $params);

    // Finally make contact as anonymous
    self::makeContactAnonymous();

    return;
  }

  // Function to get stored entity records of a given type and remove them
  private function removeEntityRecords($entity = NULL, $params = []) {

    // return, if entity or params are not passed
    if (!$entity || empty($params)) {
      CRM_Core_Session::setStatus(E::ts("{$entity} records has not been deleted."), E::ts('Record not Deleted cleanly'), 'error');
      return;
    }

    $recordIds = [];

     // Get all records of the given entity
    $records = CRM_Gdpr_Utils::CiviCRMAPIWrapper($entity, 'get', $params);

    if ($records && !empty($records['values'])) {
      foreach ($records['values'] as $key => $record) {
        array_push($recordIds, $record['id']);
      }
    }

    // delete all the records
    foreach ($recordIds as $key => $recordId) {
      $result = CRM_Gdpr_Utils::CiviCRMAPIWrapper($entity, 'delete', [
        'sequential' => 1,
        'id' => $recordId,
      ]);
    }

  }

  private function makeContactAnonymous() {
    if (!$this->_contactID) {
      CRM_Core_Error::fatal(E::ts("Something went wrong. Please contact Admin."));
    }
    $params = ['id' => $this->_contactID];
    // Update contact Record
    $updateResult = CRM_Gdpr_Utils::CiviCRMAPIWrapper('Contact', 'anonymize', $params);

    if ($updateResult && !empty($updateResult['values'])) {
      CRM_Core_Session::setStatus(E::ts("Contact has been made anonymous."), E::ts('Forget successful'), 'success');

      //MV:#7040, if successfully anonymized then record activity.
      self::createForgetMeActivity($this->_contactID);

      //MV:#7020, send email notification to DPO based on settings.
      self::sendEmailNotificationToDPO($this->_contactID);

    } else {
      CRM_Core_Session::setStatus(E::ts("Records has not been cleared."), E::ts('Record not Deleted cleanly. Please contact admin!'), 'error');
    }

  }

  public static function createForgetMeActivity($contactID) {
    if (empty($contactID)) {
      return FALSE;
    }

    $activityTypeIds = array_flip(CRM_Core_PseudoConstant::activityType(TRUE, FALSE, FALSE, 'name'));
    //check Activity type exits before fire an API.
    if (!empty($activityTypeIds[CRM_Gdpr_Constants::FORGET_ME_ACTIVITY_TYPE_NAME])) {

      $activityTypeId = $activityTypeIds[CRM_Gdpr_Constants::FORGET_ME_ACTIVITY_TYPE_NAME];
      //Make logged in user record as source contact record
      $sourceContactID = $contactID;
      if ($loggedinUser = CRM_Core_Session::singleton()->getLoggedInContactID()) {
        $sourceContactID = $loggedinUser;
      }
      $subject = E::ts('GDPR - Contact has been made anonymous');
      $params = [
        'activity_type_id'  => $activityTypeId,
        'source_contact_id' => $sourceContactID,
        'target_id'         => $contactID,
        'activity_date_time'=> date('Y-m-d H:i:s'),
        'subject'           => $subject,
        'status_id'         => 2, //COMPLETED
      ];

      CRM_Gdpr_Utils::CiviCRMAPIWrapper('Activity', 'create', $params);
    }
  } //End function

  public function sendEmailNotificationToDPO($contactID) {
    if (empty($contactID)) {
      return FALSE;
    }

    //Get GDPR settings and make sure the setting email notification to DPO has been enabled ?
    $emailToDPO = $dpoContactEmail = $dpoContactId = FALSE;
    $settings = CRM_Gdpr_Utils::getGDPRSettings();
    if (!empty($settings['email_to_dpo'])) {
      $emailToDPO = $settings['email_to_dpo'];
      $dpoContactId = $settings['data_officer'];
    }

    //Get Data protection Officer email address
    if ($dpoContactId) {
      $apiParams = [
        'contact_id' => $dpoContactId,
        'is_primary' => 1,
        'sequential' => 1,
      ];
      $apiResult = CRM_Gdpr_Utils::CiviCRMAPIWrapper('Email', 'get', $apiParams);
      $emailDetails = $apiResult['values'][0];
      $dpoContactEmail = !empty($emailDetails['email']) ? $emailDetails['email'] : FALSE;
    }

    //Now we have all details to send email notification to Point of Contact/DPO
    if ($emailToDPO && $dpoContactEmail) {

      $defaultSubject = E::ts("{$contactID} has been anonymized");
      $msg = E::ts("Contact ID : {$contactID} has been anonymized.");

      //get the default domain email address.
      list($domainEmailName, $domainEmailAddress) = CRM_Core_BAO_Domain::getNameAndEmail();

      $subject = !empty($settings['email_dpo_subject']) ? $settings['email_dpo_subject'] : $defaultSubject;
      $mailParams = [
        'subject' => $subject,
        'text'    => NULL,
        'html'    => $msg,
        'toName'  => CRM_Core_DAO::getFieldValue('CRM_Contact_DAO_Contact', $dpoContactId, 'display_name'),
        'toEmail' => $dpoContactEmail,
        'from' => "\"{$domainEmailName}\" <{$domainEmailAddress}>",
      ];

      $sent = CRM_Utils_Mail::send($mailParams);
    }
    return FALSE;
  }
}
