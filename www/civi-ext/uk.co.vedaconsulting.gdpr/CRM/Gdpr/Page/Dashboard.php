<?php

use CRM_Gdpr_ExtensionUtil as E;
require_once 'CRM/Core/Page.php';

class CRM_Gdpr_Page_Dashboard extends CRM_Core_Page {
  function run() {

    // Get GDPR settings
    $settings = CRM_Gdpr_Utils::getGDPRSettings();
    
    CRM_Core_Resources::singleton()->addStyleFile('uk.co.vedaconsulting.gdpr', 'css/gdpr.css');

    // Redirect to settings page, if GDPR settings is not set
    if (!$settings) {
    	$message = E::ts("Please add GDPR settings");
    	$url = CRM_Utils_System::url('civicrm/gdpr/settings', 'reset=1');

    	CRM_Core_Session::setStatus($message, 'GDPR', 'warning');
    	CRM_Utils_System::redirect($url);
    	CRM_Utils_System::civiExit();
    }

    // Get contacts count summary who have not had any activity for the set period
    $count = CRM_Gdpr_Utils::getNoActivityContactsSummary();

    // Get contacts who have not had any activity for the set period
    // but clicked through links in email
    $clickThroughCount = CRM_Gdpr_Utils::getContactsWithClickThrough();

    // Get GDPR activity types
    $gdprActTypes = CRM_Gdpr_Utils::getGDPRActivityTypes();
    $gdprActTypesStr = implode(', ', $gdprActTypes);

    // Search group subscription custom search
    $actContactCsDetails = CRM_Gdpr_Utils::getCustomSearchDetails(CRM_Gdpr_Constants::ACT_CONTACT_CUSTOM_SEARCH_NAME);
    $this->assign('actContactCsDetails', $actContactCsDetails);

    // Search group subscription custom search
    $gsCsDetails = CRM_Gdpr_Utils::getCustomSearchDetails(CRM_Gdpr_Constants::SEARCH_GROUP_SUBSCRIPTION_CUSTOM_SEARCH_NAME);
    $this->assign('gsCsDetails', $gsCsDetails);

    $this->assign('count', $count);
    $this->assign('clickThroughCount', $clickThroughCount);
    $this->assign('gdprActTypes', $gdprActTypesStr);
    $this->assign('settings', $settings);

    parent::run();
  }
}
