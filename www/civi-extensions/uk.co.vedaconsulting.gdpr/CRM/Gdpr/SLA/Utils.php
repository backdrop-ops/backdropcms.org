<?php
use CRM_Gdpr_ExtensionUtil as E;
require_once 'CRM/Core/Page.php';

class CRM_Gdpr_SLA_Utils {

  const cacheKeySLA = 'uk.co.vedaconsulting.gdpr/sla_acceptance';
  static protected $activityTypeName = 'SLA Acceptance';

  static protected $customGroupName = 'SLA_Acceptance';

  static protected $customFieldNameTC = 'Terms_Conditions';

  /**
   * @var key used in session to flag that the acceptance form should be
   * displayed.
   */
  static protected $promptFlagSessionKey = 'Gdpr_SLA_do_prompt';

  /**
   * Getter for promptFlagSessionkey.
   */
  static function getPromptFlagSessionKey() {
    return self::$promptFlagSessionKey;
  }

  /**
   * Sets a flag in the user session to show the form
   * on the next request.
   */
  static function flagShowForm() {
    $session = CRM_Core_Session::singleton();
    $session->set(self::getPromptFlagSessionKey(), 1);
  }

  /**
   * Sets flag in the user session to not show modal form.
   */
  static function unflagShowForm() {
    $session = CRM_Core_Session::singleton();
    $session->set(self::getPromptFlagSessionKey(), -1);
  }

  /**
   * Determines whether to show modal form.
   */
  static function showFormIsFlagged() {
    $session = CRM_Core_Session::singleton();
    return $session->get(self::getPromptFlagSessionKey()) == 1;
  }

  /**
   * Determines whether the form has been submitted and should not
   * be shown again.
   */
  static function showFormIsUnflagged() {
    $session = CRM_Core_Session::singleton();
    return $session->get(self::getPromptFlagSessionKey()) == -1;
  }

  /**
   * Displays modal acceptance form via CiviCRM.
   */
  static function showForm() {
		$formPath = '/civicrm/sla/accept';
    $currPath = $_SERVER['REQUEST_URI'];
    if (FALSE !== strpos($currPath, $formPath)) {
      return;
    }
    $script = <<< EOT
if (typeof CRM == 'object') {
   CRM.loadForm("$formPath")
  // Attach an event handler
  .on('crmFormSuccess', function(event, data) {
  });
}
EOT;
	  CRM_Core_Resources::singleton()->addScript($script);
  }

  /**
   * Gets extension settings.
   *
   * @return array
   */
  static function getSettings() {
    static $settings = [];
    if (!$settings) {
      $settings = CRM_Gdpr_Utils::getGDPRSettings();
    }
    return $settings;
  }

  /**
   * Determines whether this extension should check and prompt the user
   * to accept Terms and Conditions. Alternatively the CMS may implement
   * the acceptance form instead.
   *
   * @return bool
   */
  static function isPromptForAcceptance() {
    $settings = self::getSettings();
    return $settings['sla_prompt'] == 1;
  }

  /**
   * Gets the last Acceptance activity for a contact.
   */
  static function getContactLastAcceptance($contactId) {
    static $cache = null;
    if (is_null($cache)) {
      $cache = Civi::cache()->get( self::cacheKeySLA, [] );
    }
    if (empty($cache[$contactId]) || $cache[$contactId]['expires'] < time()) {
      $field = CRM_Gdpr_SLA_Utils::getTermsConditionsField();
      $result = \Civi\Api4\Activity::get(FALSE)
        ->addSelect('subject', 'activity_date_time', 'SLA_Acceptance.Terms_Conditions')
        ->addWhere('activity_type_id:name', '=', self::$activityTypeName)
        ->addWhere('target_contact_id', '=', $contactId)
        ->addOrderBy('activity_date_time', 'DESC')
        ->setLimit(1)
        ->execute();

      if ($result->count()) {
        $result = $result->first();
        $cache[$contactId] = [
          'subject' => $result['subject'],
          'activity_date_time' => $result['activity_date_time'],
          "custom_{$field['id']}" => $result['SLA_Acceptance.Terms_Conditions'],
          'expires' => strtotime('+ 1 day'),
        ];
      }
      else {
        $cache[$contactId] = [
          'expires' => ($cache[$contactId]['expires'] ?? 0 < time()) ? strtotime('+ 1 day') : $cache[$contactId]['expires'],
        ];
      }
      Civi::cache()->set( self::cacheKeySLA, $cache, new DateInterval('P1D'));
    }
    return $cache[$contactId];
  }

  /**
   * Records a contact accepting Terms and Conditions.
   */
  static function recordSLAAcceptance($contactId = NULL) {
    $userID = CRM_Core_Session::singleton()->getLoggedInContactID();
    $contactId = $contactId ? $contactId : $userID;
    if (!$contactId) {
      return;
    }
    $termsConditionsUrl = self::getTermsConditionsUrl();
    $termsConditionsField = self::getTermsConditionsField();
    //MV 11Oct2018 Incase of offline Data policy acceptance, Update logged in user as source contact
    $sourceContactID = $userID ? $userID : $contactId;
    $params = [
      'source_contact_id' => $sourceContactID,
      'target_id' => $contactId,
      'subject' => E::ts('Data Policy accepted'),
      'status_id' => 'Completed',
      'activity_type_id' => self::$activityTypeName,
      'custom_' . $termsConditionsField['id'] => $termsConditionsUrl,
    ];
    civicrm_api3('Activity', 'create', $params);
  }

  static function isContactDueAcceptance($contactId = NULL) {
    $contactId = $contactId ? $contactId : CRM_Core_Session::singleton()->getLoggedInContactID();
    if (!$contactId) {
      return;
    }
    $settings = self::getSettings();
    $lastAcceptance = self::getContactLastAcceptance($contactId);
    if (!$lastAcceptance) {
      // No acceptance, so due for one.
      return TRUE;
    }
    $months = !empty($settings['sla_period']) ? $settings['sla_period'] : 12;
    $seconds_in_year = 365 * 24 * 60 * 60;
    $acceptancePeriod = (($months / 12) * $seconds_in_year);
    $acceptanceDate = strtotime($lastAcceptance['activity_date_time']);
    $acceptanceDue = $acceptanceDate + $acceptancePeriod;
    // Has the document been updated more recently.
    $lastUpdated = self::getLastUpdated();
    if ($lastUpdated) {
      $lastUpdatedDate = strtotime($lastUpdated);
      if ($lastUpdatedDate > $acceptanceDate) {
        return true;
      }
    }
    return $acceptanceDue < time();
  }

  /**
   * Gets the Url to the current Terms & Conditions file.
   *
   * @param bool $absolute
   *  Whether to include the base url of the site.
   *
   *  @return string
   **/
  static function getTermsConditionsUrl($absolute = FALSE) {
    $url = '';
    $settings = CRM_Gdpr_Utils::getGDPRSettings();
    if (!empty($settings['sla_tc']) || !empty($settings['sla_tc_link'])) {
      if(array_key_exists('sla_data_policy_option', $settings)){
        switch ($settings['sla_data_policy_option']) {
          // File uploaded
          case 1:
          default:
            $config = & CRM_Core_Config::singleton();
            $url = $settings['sla_tc'];
            break;

          // Web page link
          case 2:
            $url = $settings['sla_tc_link'];
            break;
        }
      }
    }

    if (!empty($url)) {
      if (!$absolute) {
        return $url;
      }
      if (0 == strpos($url, '/')) {
        $url = substr($url, 1);
      }
      return CRM_Utils_System::url($url);
    }
  }

  /**
   * Gets the Link label for Terms & Conditions file.
   *
   *  @return string
   **/
  static function getLinkLabel() {
    return self::getSetting('sla_link_label', 'Terms &amp; Conditions');
  }

  /**
   * Gets the checkbox text for agreement.
   *
   *  @return string
   */
  static function getCheckboxText() {
    return self::getSetting('sla_checkbox_text', 'I accept the Data Policy.');
  }

  /**
   * Gets the Page title for the Terms & Conditions page.
   *
   *  @return string
   */
  static function getPageTitle() {
    return self::getSetting('sla_page_title', 'Terms &amp; Conditions');
  }

  /**
   * Gets the introductory text, which is optionally displayed when presenting the agreement
   * checkbox.
   *
   * @return string
   */
  static function getIntro() {
    return self::getSetting('sla_agreement_text', '');
  }

  /**
   * Gets the date when the latest version of the  Terms & Conditions was uploaded.
   *
   * @return
   *  string date in format: Y-m-d
   */
  static function getLastUpdated() {
    return self::getSetting('sla_tc_updated', '');

  }
  private static function getSetting($name, $default = NULL) {
    $val = '';
    $settings = CRM_Gdpr_Utils::getGDPRSettings();
    if (!empty($settings[$name])) {
      $val = $settings[$name];
    }
    return $val ? $val : $default;

  }

  /**
   * Gets a custom field definition by name and group name.
   *
   * @param string $fieldName
   * @param string $groupName
   *
   * @return array
   */
  static function getCustomField($fieldName, $groupName) {
    if (!$fieldName || !$groupName) {
      return;
    }
    $result = civicrm_api3('CustomGroup', 'get', [
  	  'sequential' => 1,
      'name' => $groupName,
      'api.CustomField.get' => [
        'custom_group_id' => "\$value.id",
        'name' => $fieldName
      ],
    ]);
    if (!empty($result['values'][0]['api.CustomField.get']['values'])) {
      return $result['values'][0]['api.CustomField.get']['values'][0];
    }
  }

  /**
   * Get definition for the field holding Terms and Conditions.
   */
  static function getTermsConditionsField() {
    static $field = [];
    if (!$field) {
      $field = self::getCustomField('Terms_Conditions', 'SLA_Acceptance');
    }
    return $field;
  }

}//End Class
