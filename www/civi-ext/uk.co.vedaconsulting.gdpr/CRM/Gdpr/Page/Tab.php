<?php

use CRM_Gdpr_ExtensionUtil as E;
//require_once 'CRM/Gdpr/Utils.php';

class CRM_Gdpr_Page_Tab extends CRM_Core_Page {

  public function run() {
  	// Retrieve contact id from url
  	$contactId = CRM_Utils_Request::retrieve('cid', 'Positive', CRM_Core_DAO::$_nullObject, TRUE);

  	// Get all group subscription
  	$groupSubscriptions = CRM_Gdpr_Utils::getallGroupSubscription($contactId);
  	$this->assign('groupSubscriptions', $groupSubscriptions);
  	$this->assign('contactId', $contactId);

    $summary['communications_preferences'] = $this->getCommunicationsPreferencesDetails($contactId);
    $summary['data_policy'] = $this->getDataPolicyDetails($contactId);
    $this->assign('summary', $summary);

    CRM_Core_Resources::singleton()->addStyleFile('uk.co.vedaconsulting.gdpr', 'css/gdpr.css');

    parent::run();
  }

  public function getDataPolicyDetails($contactId) {
    $details = [
      'title' => E::ts('Data Policy acceptance.'),
      'details' => E::ts('Not yet accepted by the contact.'),
    ];
    $activity = CRM_Gdpr_SLA_Utils::getContactLastAcceptance($contactId);
    $isDue = CRM_Gdpr_SLA_Utils::isContactDueAcceptance($contactId);
    if ($activity) {
      $details['details'] = $activity['subject'];
      $details['date'] = $activity['activity_date_time'];
      $field = CRM_Gdpr_SLA_Utils::getTermsConditionsField();
      $key = 'custom_' . $field['id'];
      $url = !empty($activity[$key]) ? $activity[$key] : '';
      $separator = '<br />';
      if ($isDue) {
        $dueMsg = '<span class="notice">The contact is due to renew their acceptance.</span>';
      }
      if ($url) {
        $path = parse_url($url, PHP_URL_PATH);
        $label = pathinfo($path, PATHINFO_FILENAME);
        $details['details']  .= $separator . '<a target="blank" href="' . $url . '">' . $label  .'</a>' . $separator . $dueMsg;
      }
    }
    return $details;
  }

  public function getCommunicationsPreferencesDetails($contactId) {
    $details = [
      'title' => E::ts('Communications Preferences'),
      'details' => E::ts('Not yet updated by the contact.'),
    ];
    $activity = CRM_Gdpr_CommunicationsPreferences_Utils::getLastUpdatedForContact($contactId);
    if ($activity) {
      $details['details'] = $activity['subject'];
      $details['date'] = $activity['activity_date_time'];
    }
    return $details;
  }
}
