<?php

use CRM_Gdpr_ExtensionUtil as E;
use CRM_Gdpr_CommunicationsPreferences_Utils as U;

/**
 * Form controller class
 *
 * @see https://wiki.civicrm.org/confluence/display/CRMDOC/QuickForm+Reference
 */
class CRM_Gdpr_Form_UpdatePreference extends CRM_Core_Form {
  protected $settings;
  protected $commPrefSettings;
  protected $commPrefGroupsetting;
  public $channelEleNames;
  public $groupEleNames;
  protected $_fields = [];

  public $containerPrefix = 'enable_';

  /**
   * @var int the Contact ID
   * @deprecated Use $this->getContactID and $this->_cid;
   */
  public $_id;

  /**
   * @var int The contact ID
   */
  public $_cid;

  public $_gid;
  public $_context;
  public $_ruleGroupID;

  public function getContactID() {
    if (!empty($this->_cid)) {
      $contactID = $this->_cid;
    }
    elseif (!empty($this->_cid)) {
      $contactID = $this->_cid;
    }
    else {
      $contactID = parent::getContactID();
    }
    $this->_cid = $this->_cid = $contactID;
    return $contactID;
  }

  public function preProcess() {
    //Retrieve contact id from URL
    if (empty($this->getContactID())) {
      // Do nothing as we need anonymous users to be able to "update" their details
    };

    //Add Gdpr CSS file
    CRM_Core_Resources::singleton()->addStyleFile('uk.co.vedaconsulting.gdpr', 'css/gdpr.css');
    parent::preProcess();
  }

  public function getSettings() {
    $this->settings = U::getSettings();
    $this->commPrefSettings = $this->settings[U::SETTING_NAME];
    $this->commPrefGroupsetting   = $this->settings[U::GROUP_SETTING_NAME];
  }

  public function buildQuickForm() {
    //Get all Communication preference settings
    $this->getSettings();
    $userID = CRM_Core_Session::getLoggedInContactID();

    if (!empty($this->commPrefSettings['profile'])) {
      $this->buildCustom($this->commPrefSettings['profile']);
    }

    //Display Page Title from settings
    if ($pageTitle = $this->commPrefSettings['page_title']) {
      $this->setTitle(E::ts($pageTitle));
    }

    //Display Page intro from settings.
    if ($introText = $this->commPrefSettings['page_intro']) {
      $this->assign('page_intro', $introText);
    }

    //Include reCAPTCHA?
    if ($this->commPrefSettings['add_captcha']) {
      if (is_callable(['CRM_Utils_ReCAPTCHA', 'enableCaptchaOnForm'])) {
        $button = substr($this->controller->getButtonName(), -4);
        // We show reCAPTCHA for anonymous user if enabled.
        // 'skip' button is on additional participant forms, we only show reCAPTCHA on the primary form.
        if (!CRM_Core_Session::getLoggedInContactID() && ($button !== 'skip')) {
          if (\Civi\Api4\UFGroup::get(FALSE)
            ->addWhere('id', '=', $this->commPrefSettings['profile'])
            ->execute()
            ->first()['add_captcha']
          ) {
            CRM_Utils_ReCAPTCHA::enableCaptchaOnForm($this);
          }
        }
      }
    }

    //Inject channels and groups into comms preferenec form.
    //we have moved this section into helper functions, because we are reusing same functions in other place like event / contribution thank you page to have comms preference embed form
    U::injectCommPreferenceFieldsIntoForm($this);

    //GDPR Terms and conditions
    //if already accepted then we dont this link at all
    $isContactDueAcceptance = empty($this->_cid) ?  TRUE : CRM_Gdpr_SLA_Utils::isContactDueAcceptance($this->_cid);
    if ($gdprTermsConditionsUrl = CRM_Gdpr_SLA_Utils::getTermsConditionsUrl()) {
      $this->assign('gdprTcURL', $gdprTermsConditionsUrl);
    }
    if ($gdprTermsConditionslabel = CRM_Gdpr_SLA_Utils::getLinkLabel()) {
      $this->assign('gdprTcLabel', $gdprTermsConditionslabel);
    }
    if ($isContactDueAcceptance) {
      $termsConditionsField = $this->getTermsAndConditionFieldId();

      $tcFieldName  = 'custom_'.$termsConditionsField;
      $tcLink = E::ts("<a href='%1' target='_blank'>%2</a>", [1 => $gdprTermsConditionsUrl, 2 => $gdprTermsConditionslabel]);
      $this->assign('tcLink', $tcLink);
      $this->assign('tcIntro', CRM_Gdpr_SLA_Utils::getIntro());
      $tcFieldlabel = CRM_Gdpr_SLA_Utils::getCheckboxText();
      $this->assign('tcFieldlabel', $tcFieldlabel);
      $this->assign('tcFieldName', $tcFieldName);
      $this->assign('isContactDueAcceptance', $isContactDueAcceptance);

      $this->add('checkbox', $tcFieldName, $gdprTermsConditionslabel, NULL, TRUE);
    }
    else {
      $accept_activity = CRM_Gdpr_SLA_utils::getContactLastAcceptance($this->_cid);
      $accept_date = '';
      if (!empty($accept_activity['activity_date_time'])) {
        $accept_date = date('d/m/Y', strtotime($accept_activity['activity_date_time']));
      }

      $tcFieldlabel = E::ts("Here is our <a href='%1' target='_blank'>%2</a>, which you agreed to on %3.",
        [
          1 => $gdprTermsConditionsUrl,
          2 => $gdprTermsConditionslabel,
          3 => $accept_date,
        ]
      );
      $this->assign('tcFieldlabel', $tcFieldlabel);
    }

    //have source field for offline comms preference, make sure we dont show this field when contact update their own preferences
    if ($userID && $userID != $this->_cid) {
      $this->add('text', 'activity_source', E::ts('Source of Communication Preferences'));
    }

    $this->addButtons([
      [
        'type' => 'submit',
        'name' => E::ts('Save'),
        'isDefault' => TRUE,
      ],
    ]);

    // export form elements
    $this->assign('groupEleNames', $this->groupEleNames);

    //add form rule
    $this->addFormRule(['CRM_Gdpr_Form_UpdatePreference', 'formRule'], $this);
    if (!empty($this->commPrefSettings['profile'])) {
      $this->addFormRule(['CRM_Profile_Form', 'formRule'], $this);
    }

    parent::buildQuickForm();
  }

  /**
   * Is this being called from an entity reference field.
   *
   * E.g clicking on 'New Organization' from the employer field
   * would create a link with the context = 'dialog' in the url.
   *
   * @return bool
   */
  public function isEntityReferenceContactCreateMode(): bool {
    return $this->_context === 'dialog';
  }

  /**
   * Add the custom fields.
   *
   * @param int $ufGroupID
   * @param string $name
   * @param bool $viewOnly
   */
  public function buildCustom($ufGroupID, $name = 'custom_pre') {
    if ($ufGroupID) {
      //For Profile form validation
      $this->_gid = $ufGroupID;
      $dao = new CRM_Core_DAO_UFGroup();
      $dao->id = $ufGroupID;
      if ($dao->find(TRUE)) {
        $this->_isUpdateDupe = $dao->is_update_dupe; // Profile duplicate match option
        $this->_ufGroup = (array) $dao;
      }

      $button = substr($this->controller->getButtonName(), -4);
      $contactID = $this->_cid;

      if ($contactID) {
        CRM_Core_BAO_UFGroup::filterUFGroups($ufGroupID, $contactID);
      }

      try {
        $fields = CRM_Core_BAO_UFGroup::getFields($ufGroupID, FALSE, CRM_Core_Action::ADD,
          NULL, NULL, FALSE, NULL,
          FALSE, NULL, CRM_Core_Permission::CREATE,
          'field_name', TRUE
        );
      } catch (Exception $e) {
        CRM_Core_Error::debug_var('CRM_Gdpr_Form_UpdatePreference buildCustom', $e->getMessage());
        CRM_Core_Error::debug_log_message('Error in retrieving the profile fields');
        CRM_Core_Error::debug_log_message('Please ensure that the Profile you have selected in the GDPR settings page exists and is enabled');
      }

      if (!empty($fields) && is_array($fields)) {
        $this->assign($name, $fields);
        foreach ($fields as $key => $field) {
          //make the field optional if primary participant
          //have been skip the additional participant.
          if ($button == 'skip') {
            $field['is_required'] = FALSE;
          }
          $this->_mode = $contactID ? CRM_Profile_Form::MODE_EDIT : CRM_Profile_Form::MODE_CREATE;
          CRM_Core_BAO_UFGroup::buildProfile($this, $field, $this->_mode, $contactID, TRUE);

          $this->_fields[$key] = $field;
        }
      }
    }
  }

  public static function formRule($fields, $files, $form){
    $errors = [];

    if (!empty($form->groupEleNames)) {
      foreach ($form->groupEleNames as $groupName => $groupEleName) {
        //get the channel array and group channel array
        $channelArray = [] ;
        $groupChannelArray = [];
        foreach ($form->channelEleNames as $channel) {
          $groupChannel = str_replace($form->containerPrefix, '', $channel);
          $channelSettingValue = $form->commPrefGroupsetting[$groupEleName][$groupChannel];

          if (!is_null($channelSettingValue) && $channelSettingValue != '') {
            $channelArray[$groupChannel] = ($fields[$channel] == 'YES') ? 1 : 0;
            $groupChannelArray[$groupChannel] = empty($form->commPrefGroupsetting[$groupEleName][$groupChannel]) ? 0 : 1;
          }
        }

        
        if( !is_array( $channelArray ) ){
          // create an empty array to avoid error message
          $channelArray = [];
        }
        
        //check any difference then return as error
        if(!empty($fields[$groupEleName]) && ($diff = array_diff_assoc($groupChannelArray, $channelArray))){
          //do something here.
          $diff = implode(', ', array_keys($diff));
          $errors[$groupEleName] = E::ts("Communication Preferences {$diff} has to be selected for this group");
        }
      }
    }

    return empty($errors) ? TRUE : $errors;
  }

  public function setDefaultValues() {
    $defaults = [];
    if (!empty($this->_cid)) {
      $channelPrefs = U::getChannelPrefsForContact($this->_cid);
      $groupPrefs = U::getGroupSelectionsForContact($this->_cid);
      $defaults = array_merge($channelPrefs, $groupPrefs);


      //Set Profile defaults
      $fields = [];
      $removeCustomFieldTypes = ['Contribution', 'Membership'];
      $contribFields = CRM_Contribute_BAO_Contribution::getContributionFields();

      foreach ($this->_fields as $name => $field) {
        //don't set custom data Used for Contribution (CRM-1344)
        if (substr($name, 0, 7) == 'custom_') {
          $id = substr($name, 7);
          if (!CRM_Core_BAO_CustomGroup::checkCustomField($id, $removeCustomFieldTypes)) {
            continue;
          }
          // ignore component fields
        }
        elseif (array_key_exists($name, $contribFields) || (substr($name, 0, 11) == 'membership_') || (substr($name, 0, 13) == 'contribution_')) {
          continue;
        }
        $fields[$name] = $field;
      }

      if (!empty($fields)) {
        CRM_Core_BAO_UFGroup::setProfileDefaults($this->_cid, $fields, $defaults);
      }
    }
    //#7955 params from URL
    $emailPrimary = CRM_Utils_Request::retrieve('field_email', 'String', CRM_Core_DAO::$_nullObject);
    if ($emailPrimary) {
      $defaults['email-Primary'] = $emailPrimary;
    }
    return $defaults;
  }

  public function getTermsAndConditionFieldId() {
    $termsConditionsField =  CRM_Gdpr_SLA_Utils::getTermsConditionsField();
    return $termsConditionsField['id'];
  }

  public function postProcess() {
    $submittedValues = $this->exportValues();
    $existingContact = $this->getContactID();

    $contactType = 'Individual';
    if ($existingContact) {
      $contactType = CRM_Core_DAO::getFieldValue('CRM_Contact_DAO_Contact', $existingContact, 'contact_type');
    }
    $contactID = CRM_Contact_BAO_Contact::createProfileContact(
      $submittedValues,
      $this->_fields,
      $existingContact,
      NULL,
      NULL,
      $contactType,
      TRUE
    );
     //Terms and condition Record SLA acceptance
    $termsConditionsField = $this->getTermsAndConditionFieldId();
    $tcFieldName  = 'custom_'.$termsConditionsField;
    if (!empty($submittedValues[$tcFieldName])) {
      CRM_Gdpr_SLA_Utils::recordSLAAcceptance($contactID);
    }

    //we have now moved this section into common helper function which reused in other place like event/contribution thank you to let update comms preference using embed form.
    U::updateCommsPrefByFormValues($contactID, $submittedValues);
    $submittedValues['subject'] = E::ts('GDPR Communication Preferences Form');
    U::createCommsPrefActivity($contactID, $submittedValues);

    $this->sendConfirmation();

    if (!empty($this->commPrefSettings['completion_message'])) {
      $thankYouMsg = html_entity_decode($this->commPrefSettings['completion_message']);
      CRM_Core_Session::setStatus($thankYouMsg, E::ts('Communication Preferences'), 'Success');
    }

    //Get the destination url from settings and redirect if we found one.
    if (!empty($this->commPrefSettings['completion_redirect'])) {
      $destinationURL = !empty($this->commPrefSettings['completion_url']) ? $this->commPrefSettings['completion_url'] : NULL;
      //MV: commenting this line, We have already restricted the setting to get only absolute URL.
      //check URL is not absolute and no leading slash then add leading slash before redirect.
      $parseURL = parse_url($destinationURL);
      if (empty($parseURL['host']) && (strpos($destinationURL, '/') !== 0)) {
        $destinationURL = '/'.$destinationURL;
      }
      CRM_Utils_System::redirect($destinationURL);
    }
    parent::postProcess();
  }

  public function sendConfirmation() {
    if (empty($this->commPrefSettings['is_email_confirm'])) {
      return;
    }

    $contactID = $this->getContactID();

    list($displayName, $email) = CRM_Contact_BAO_Contact_Location::getEmailDetails($contactID);

    $tplParams = [
      'email' => $email,
      'confirm_email_text' => CRM_Utils_Array::value('confirm_email_text', $this->commPrefSettings),
      'display_name' => $displayName,
    ];

    $sendTemplateParams = [
      'groupName' => 'msg_tpl_workflow_gdpr',
      'valueName' => 'gdpr_update_preferences',
      'contactId' => $contactID,
      'tplParams' => $tplParams,
    ];

    $sendTemplateParams['from'] = CRM_Utils_Array::value('confirm_from_name', $this->commPrefSettings) . " <" . CRM_Utils_Array::value('confirm_from_email', $this->commPrefSettings) . ">";
    $sendTemplateParams['toName'] = $displayName;
    $sendTemplateParams['toEmail'] = $email;
    $sendTemplateParams['autoSubmitted'] = TRUE;
    $sendTemplateParams['cc'] = CRM_Utils_Array::value('cc_confirm', $this->commPrefSettings);
    $sendTemplateParams['bcc'] = CRM_Utils_Array::value('bcc_confirm', $this->commPrefSettings);
    CRM_Core_BAO_MessageTemplate::sendTemplate($sendTemplateParams);
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
