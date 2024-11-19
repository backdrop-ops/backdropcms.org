<?php

use Civi\Api4\Activity;
use CRM_Gdpr_ExtensionUtil as E;

class CRM_Gdpr_CommunicationsPreferences_Utils {

  const SETTING_GROUP = 'GDPR_CommunicationsPreferences_Settings';
  const SETTING_NAME = 'gdpr_communications_preferences_settings';
  /* Setting name for group preferences */
  const GROUP_SETTING_NAME = 'gdpr_communications_preferences_group_settings';
  const COMM_PREF_OPTIONS = 'comm_pref_options';
  const COMM_PREF_ACTIVITY_TYPE = 'Update_Communication_Preferences';

  private static $groups = [];

  public static function getSettingsDefaults() {
    // @fixme This should be replaced with a proper settings definition file (eg. settings/gdpr.setting.php)
    $settings[self::SETTING_NAME] = [
      'page_title' => E::ts('Communication Preferences'),
      'page_intro' => E::ts('We want to ensure we are only sending you information that is of interest to you, in a way you are happy to receive.'),
      'enable_channels' => 1,
      'channels_intro' => E::ts('Please tell us how you would like us to keep in touch.'),
      'channels' => [
        'enable_email' => 1,
        'enable_phone' => 1,
        'enable_post' => 1,
        'enable_sms' => 0,
      ],
      'enable_groups' => 0,
      'groups_heading' => E::ts('Interest groups'),
      'groups_intro' => E::ts('We want to continue to keep you informed about our work. Opt-in to the groups that interest you.'),
      'completion_message' => E::ts('Your communications preferences have been updated. Thank you.'),
      'add_captcha' => 0,
      'comm_pref_in_thankyou' => 'none',
    ];

    foreach (self::getGroups() as $group) {
      $group_values['group_enable'] = 0;
      foreach (['title', 'description'] as $key) {
        if (isset($group[$key])) {
          $group_values['group_' . $key] = $group[$key];
        }
      }
      $settings[self::GROUP_SETTING_NAME]['group_' . $group['id']] = $group_values;
    }
    return $settings;
  }

  /**
   * Get Communication Preferences settings.
   *
   * @param bool $use_defaults
   *  Whether to use default values if the settings do not exist.
   */
  public static function getSettings($use_defaults = TRUE) {
    // @fixme: We should be using settings/gdpr.setting.php and \Civi::settings()->get() instead of this.
    $settings = [];
    $defaults = $use_defaults ? self::getSettingsDefaults() : [];
    foreach ([self::SETTING_NAME, self::GROUP_SETTING_NAME] as $setting_name) {
      $serialized = CRM_Gdpr_Utils::getItem(self::SETTING_GROUP, $setting_name);
      if (!$serialized && $use_defaults)  {
        $settings[$setting_name] = !empty($defaults[$setting_name]) ? $defaults[$setting_name] : [];
      }
      else {
        $settings[$setting_name] = $serialized ? unserialize($serialized) : [];
      }
    }
    if (!empty($settings[self::GROUP_SETTING_NAME])) {
      $settings[self::GROUP_SETTING_NAME] = self::pruneGroupSettings($settings[self::GROUP_SETTING_NAME]);
    }
    return $settings;
  }

  /**
   * Gets available contact profiles as an option array.
   *
   * @return array keyed by profile id, with value the profile label.
   */
  public static function getProfileOptions() {
    $types = ['Individual', 'Contact'];

    //To get Profile with array of group type
    //using core method to get the profiles, because group_type has been imploded with (,) in database civicrm_uf_group.
    //for eg group type can be Individual,Contact or Contact,Individual . so api didn't return all the result where profiles with group type Individual or Contact. (have to mention 'Individual,Contact')
    //we can use core method to get all profiles which are Individual or Contact or Both

    $profiles = CRM_Core_BAO_UFGroup::getProfiles($types);

    $options = [0 => '-- Please select --'] + $profiles;
    return $options;
  }


  /**
   * Remove group settings for groups that no longer exist or are no longer
   * public.
   */
  public static function pruneGroupSettings($group_settings) {
    $groups = self::getGroups();
    $prefix = 'group_';
    $pruned_settings = [];
    foreach ($group_settings as $key => $value) {
      $id = strpos($key, $prefix) === 0 ? substr($key, strlen($prefix)) : NULL;
      if ($id || is_numeric($id) || !empty($groups[$id])) {
        $pruned_settings[$key] = $value;
      }
    }
    return $pruned_settings;
  }

  /**
   * Save Communication Prefences settings.
   *
   * @param array $settings_array
   */
  public static function saveSettings($settings_array) {
    foreach ([self::SETTING_NAME, self::GROUP_SETTING_NAME] as $setting_name) {
      if (isset($settings_array[$setting_name])) {
        $setting_serialized = serialize($settings_array[$setting_name]);
        CRM_Gdpr_Utils::setItem($setting_serialized, self::SETTING_GROUP, $setting_name);
      }
    }
  }

  /**
   * Gets the public groups.
   *
   * @return array
   */
  public static function getGroups() {
    if (!self::$groups) {
      $params = [
          'is_active' => 1,
          'visibility' => "Public Pages",
          // Key by id for convenience.
          'serialized' => FALSE,
      ];
      $result = civicrm_api3('Group', 'get', $params);
      if (!empty($result['values'])) {
        self::$groups = $result['values'];
      }
    }
    return self::$groups;
  }

  /**
   * Sorts an array of groups according to their user-assigned weight.
   *
   * @param array $groups
   *  Group data from the api, keyed by id.
   *
   * @param array $sortBySettings
   *  Array keyed by a Communcations Preferences group setting, value can be
   *  either 'asc' or 'desc'.
   */
  public static function sortGroups($groups, $sortBySettings = ['group_weight' => 'asc']) {
    $settings = self::getSettings(FALSE);
    $group_settings = $settings[self::GROUP_SETTING_NAME];
    $defaults = [
      'group_weight' => 0,
      'group_enable' => 0,
      'group_title' => '',
    ];
    // Filter out arguments that we do not support.
    $sortKeys = array_intersect_key($sortBySettings, $defaults);
    foreach ($groups as $id => $grp) {
      if (!empty($group_settings['group_' . $id])) {
        $item = $group_settings['group_' . $id];
      }
      else {
        $item = $defaults;
      }
      $groups[$id]['group_weight'] = $item['group_weight'];
      $groups[$id]['group_enable'] = $item['group_enable'];
      $groups[$id]['group_title'] = $item['group_title'];
    }
    uasort($groups, function($a, $b) use ($sortKeys) {
      foreach ($sortKeys as $key => $order) {
        if (is_numeric($a[$key]) && is_numeric($b[$key])) {
          $diff = $order == 'asc' ? $a[$key] - $b[$key] : $b[$key] - $a[$key];
        }
        elseif (is_string($a[$key]) && is_string($b[$key])) {
          $diff = $order == 'asc' ? strcmp($a[$key], $b[$key]) : strcmp($b[$key], $a[$key]);
        }
        if ($diff != 0) {
          return $diff;
        }
      }
      return $diff;
    });
    return $groups;
  }


  /**
   * Gets details of the last time a contact updated their communications
   * preferences.
   *
   * @param int $cid
   *  Contact Id.
   *
   * @return array
   *  Array of activity details or empty array.
   */
  public static function getLastUpdatedForContact($cid) {
    $return = [];
    if (!$cid) {
      return $return;
    }
    $result = civicrm_api3('Activity', 'get', [
      'sequential' => 1,
      'activity_type_id' => "Update_Communication_Preferences",
      // 'source_contact_id' => $cid,
      //MV: Civi Older version doesn't return api value using source_contact_id. if we add target_contact_id then BAO query include activity contact table and filter out using params
      'target_contact_id' => $cid,
      'options' => ['sort' => "id desc"],
    ]);
    return !empty($result['values']) ? $result['values'][0] : $return;
  }

  /**
   * Get options for channels.
   * @return array
   */
  public static function getChannelOptions() {
    return $channels = [
      'email' => E::ts('Email'),
      'phone' => E::ts('Phone'),
      'post' => E::ts('Postal Mail'),
      'sms' => E::ts('SMS'),
    ];
  }

  public static function getCommunicationPreferenceMapperField() {
    return [
      'email' => ['do_not_email', 'is_opt_out'],
      'phone' => ['do_not_phone'],
      'post' => ['do_not_mail'],
      'sms' => ['do_not_sms'],
    ];
  }
  public static function getCommunicationPreferenceMapper() {
    return [
      'email' => [
        'UNKNOWN' => [
          'do_not_email' => 'NULL',
        ],
        'YES' => [
          'do_not_email' => 0,
          'is_opt_out' => 0,
        ],
        'NO' => [
          'is_opt_out' => 1,
        ],
      ],
      'phone' => [
        'UNKNOWN' => [
          'do_not_phone' => 'NULL',
        ],
        'YES' => [
          'do_not_phone' => 0,
        ],
        'NO' => [
          'do_not_phone' => 1,
        ],
      ],
      'post' => [
        'UNKNOWN' => [
          'do_not_mail' => 'NULL',
        ],
        'YES' => [
          'do_not_mail' => 0,
        ],
        'NO' => [
          'do_not_mail' => 1,
        ],
      ],
      'sms' => [
        'UNKNOWN' => [
          'do_not_sms' => 'NULL',
        ],
        'YES' => [
          'do_not_sms' => 0,
        ],
        'NO' => [
          'do_not_sms' => 1,
        ],
      ],
    ];
  }

  public static function getCommPreferenceURLForContact($cid, $skipContactIdInURL = FALSE){
    if (empty($cid)) {
      return NULL;
    }

    $urlParams = [
      'reset' => 1,
      'cid'   => $cid,
      'cs'    => CRM_Contact_BAO_Contact_Utils::generateChecksum($cid),
    ];

    //for sumamry hook, cid would add by default, we dont want duplicate URL params.
    if ($skipContactIdInURL) {
      unset($urlParams['cid']);
    }
    return CRM_Utils_System::url('civicrm/gdpr/comms-prefs/update', $urlParams, TRUE, NULL, TRUE, TRUE);
  }

  public static function addCommsPreferenceLinkInThankYouPage($cid, &$form, $entity = 'Event'){
    if (empty($cid)) {
      return;
    }

    $settings = CRM_Gdpr_CommunicationsPreferences_Utils::getSettings();
    $settings = $settings[CRM_Gdpr_CommunicationsPreferences_Utils::SETTING_NAME];

    //To display Communication Preference URL in Thank you page for event
    if (!empty($cid)) {
      $commPrefURL = CRM_Gdpr_CommunicationsPreferences_Utils::getCommPreferenceURLForContact($cid);
      $linkIntro   = CRM_Utils_Array::value('comm_pref_link_intro', $settings, NULL);
      $linkLabel   = CRM_Utils_Array::value('comm_pref_link_label', $settings, 'Update Communication Preferences');
      $form->assign('comm_pref_url', $commPrefURL);
      $form->assign('link_label', $linkLabel);
      $form->assign('link_intro', $linkIntro);
    }
  }

  /**
   * Inject the communication preference fields into a form.
   *
   * Intended to be used for Event registration and contribution thank-you pages.
   */
  /**
   * @param CRM_Gdpr_Form_UpdatePreference|CRM_Core_Form $form
   *
   * @throws \CRM_Core_Exception
   */
  public static function injectCommPreferenceFieldsIntoForm(&$form) {

    // Get the Comms pref options (yes/no) form option group.
    $commPrefOpGroup = CRM_Core_DAO::getFieldValue('CRM_Core_DAO_OptionGroup', self::COMM_PREF_OPTIONS, 'id', 'name');
    $commPrefOptions = ['' => E::ts('--Select--')] + CRM_Core_BAO_OptionValue::getOptionValuesAssocArray($commPrefOpGroup);
    // quick hack to translate yes/no options, because are not coming translated from core's function
    foreach($commPrefOptions as $key => $value){
      $commPrefOptions[$key] = E::ts($value);
    }

    //get Communication preference settings
    $settings = self::getSettings();
    $fieldsSettings = $settings[self::SETTING_NAME];
    $groupSettings  = $settings[self::GROUP_SETTING_NAME];

    $containerPrefix = 'enable_';
    $form->assign('containerPrefix', $containerPrefix);

    //Prepare channel fields
    $form->channelEleNames = [];
    $isChannelsEnabled = $fieldsSettings['enable_channels'];
    if ($isChannelsEnabled) {
      if ($channelIntro = $fieldsSettings['channels_intro']) {
        $channelIntro = preg_replace("/[\r\n]*/", "", $channelIntro);
        $form->assign('channels_intro', $channelIntro);
      }

      foreach ($fieldsSettings['channels'] as $key => $value) {
        if ($value) {
          $name  = str_replace($containerPrefix, '', $key);
          $label = ucwords(str_replace('_', ' ', $name));
          $form->add('select', $key, E::ts($label), $commPrefOptions, TRUE);
          $form->channelEleNames[] = $key;

          //Elements may set to flag frozen, because of thankyou page may be.
          //Check if the fields are frozen fields then unset it. so you can see the fields in thank you page.
          $elemIndex = $form->_elementIndex[$key];
          $form->_elements[$elemIndex]->_flagFrozen = 0;
        }
      }
    }

    // groupChannel was derived from channelEleNames using smarty but ucwords is not supported
    //   in smarty5 so this is a quick refactor to move it to the PHP side without checking
    //   if we can simplify/cleanup.
    foreach ($form->channelEleNames as $channelName) {
      $groupChannel[str_replace($containerPrefix, '', $channelName)] = ucwords($channelName);
    }
    $form->assign('groupChannel', $groupChannel ?? []);

    $form->assign('channelEleNames', $form->channelEleNames);
    $form->assign('channelEleNamesJSON', json_encode($form->channelEleNames));

    //Communication preference Group settings enabled ?
    $isGroupSettingEnabled = !empty($fieldsSettings['enable_groups']) ? $fieldsSettings['enable_groups'] : NULL;
    $form->groupEleNames = [];
    if ($isGroupSettingEnabled) {
      if ($groupsHeading = $fieldsSettings['groups_heading']) {
        $form->assign('groups_heading', $groupsHeading);
      }

      if ($groupsIntro = $fieldsSettings['groups_intro']) {
        $form->assign('groups_intro', $groupsIntro);
      }

      //all for all groups and disable checkbox is group_enabled from settings
      $groups = self::getGroups();
      $groups = self::sortGroups($groups, ['group_weight' => 'asc']);
      foreach ($groups as $group) {
        $container_name = 'group_' . $group['id'];
        if (!empty($groupSettings[$container_name]['group_enable'])) {
          $title = $groupSettings[$container_name]['group_title'];
          $groupsFromSettings[$title] = $group['id'];
          $form->add('Checkbox', $container_name, $title);
          $form->groupEleNames[] = $container_name;

          //Elements may set to flag frozen, because of thankyou page may be.
          //Check if the fields are frozen fields then unset it. so you can see the fields in thank you page.
          $elemIndex = $form->_elementIndex[$container_name];
          $form->_elements[$elemIndex]->_flagFrozen = 0;
        }
      }
      $form->assign('commPrefGroupsetting', $groupSettings);
      $intro = !empty($fieldsSettings['comm_pref_thankyou_embed_intro']) ? $fieldsSettings['comm_pref_thankyou_embed_intro'] : '';

      $form->assign('commPrefIntro', $intro);
    }
    $form->assign('groupEleNames', $form->groupEleNames);
    $form->assign('groupEleNamesJSON', json_encode($form->groupEleNames));
  }

  /**
   * Gets Channel preferences for a Contact.
   *
   * @param int $contactId
   */
  public static function getChannelPrefsForContact($contactId) {
    $contactDetails = civicrm_api3('Contact', 'getsingle', ['id' => $contactId]);
    $lastAcceptance = CRM_Gdpr_SLA_Utils::getContactLastAcceptance($contactId);
    $settings = self::getSettings();
    $fieldSettings = $settings[self::SETTING_NAME];
    $groupSettings  = $settings[self::GROUP_SETTING_NAME];

    $communicationPreferenceMapperFields = self::getCommunicationPreferenceMapperField();
    $values = [];
    foreach ($fieldSettings['channels'] as $key => $value) {
      $name  = str_replace('enable_', '', $key);
      if (!$lastAcceptance) {
        // No acceptance, and preferences are 0 then, set unknown, otherwise display yes/no
        $values[$key] = '';
      }
      elseif ($value) {
        $comPref = FALSE;
        foreach ($communicationPreferenceMapperFields[$name] as $fieldName) {
          if (!empty($contactDetails[$fieldName])) {
            $comPref = TRUE;
            break;
          }
        }
        $values[$key] = $comPref ? 'NO' : 'YES';
      }
    }
    return $values;
  }

  /**
   * Returns name for Channel select element in Comms Prefs form.
   *
   * @param string $channel
   * @return string
   */
  public static function channelElementName($channel) {
    return 'enable_' . $channel;
  }

  /**
   * Gets group selections for a contact.
   *
   * Can be used as defaults for Comms Prefs form.
   *
   * @param int $contactId
   * @return []
   */
  public static function getGroupSelectionsForContact($contactId) {
    $values = [];
    $groups = self::getGroups();
    $settings = self::getSettings();
    $groupSettings  = $settings[self::GROUP_SETTING_NAME];
    foreach ($groups as $group) {
      $container_name = 'group_' . $group['id'];
      if (!empty($groupSettings[$container_name]['group_enable'])) {
        $contactGroupDetails = civicrm_api3('GroupContact', 'get', [
          'contact_id' => $contactId,
           'group_id' => $group['id'],
           'status' => 'Added',
          ]
        );

        if (!empty($contactGroupDetails['id'])) {
          $values[$container_name] = 1;
        }
      }
    }
    return $values;
  }

  /**
   * This is helper function to update communication preference by form submitted values.
   */
  public static function updateCommsPrefByFormValues($contactId, $submittedValues) {
    if (empty($submittedValues) OR empty($contactId)) {
      return;
    }

    //Get all comms preference settings
    $settings = self::getSettings();
    $fieldsSettings = $settings[self::SETTING_NAME];
    $groupSettings  = $settings[self::GROUP_SETTING_NAME];
    $commPrefMapper = self::getCommunicationPreferenceMapper();
    $preferedCommOptn = CRM_Core_PseudoConstant::get('CRM_Contact_DAO_Contact', 'preferred_communication_method');
    //Prepare Comm pref params
    $commPref = ['id' => $contactId];

    // get existing preferred communication methods
    $existingPreferredMethod = [];
    try {
      $apiResult = civicrm_api3('Contact', 'getsingle', [
        'return' => ["preferred_communication_method"],
        'id' => $contactId,
      ]);

      $existingPreferredMethod = $apiResult['preferred_communication_method'];

        if( !is_array( $existingPreferredMethod ) ){
          // create an empty array to avoid error message
          $existingPreferredMethod = [];
        }

      $existingPreferredMethod = array_fill_keys($existingPreferredMethod, 1);
    } catch (Exception $e) {
      CRM_Core_Error::debug_var('updateCommsPrefByFormValues', $e);
    }

    //FIXME, this must go under constant file
    $containerPrefix = 'enable_';

    //Update contact communication preference based on channels selected
    $selectedPreferredOptns = $preferredComm = [];
    foreach ($fieldsSettings['channels'] as $key => $value) {
      $name  = str_replace($containerPrefix, '', $key);
      if (!empty($submittedValues[$key])) {
        $channelValue = $submittedValues[$key];
        $commPref = array_merge($commPref, ($commPrefMapper[$name][$channelValue] ?? []));

        if ($name == 'post') {
          $name = 'postal mail';
        }

        $prefComm = array_search($name, array_map('strtolower', $preferedCommOptn));
        if (strtolower($channelValue) == 'yes') {
          $selectedPreferredOptns[$prefComm] = 1;
        }
        elseif (strtolower($channelValue) == 'no') {
          $selectedPreferredOptns[$prefComm] = 0;
        }
      }
    }

    // Format preferred communication method
    if (!empty($selectedPreferredOptns)) {
      // don't want to lose exising methods which is not in communication preferences ex:Fax
      if (!empty($existingPreferredMethod)) {
        $selectedPreferredOptns = $selectedPreferredOptns + $existingPreferredMethod;
      }

      CRM_Utils_Array::formatArrayKeys($selectedPreferredOptns);
      if (!empty($selectedPreferredOptns)) {
        $prefOptn  = array_intersect_key($preferedCommOptn, array_flip($selectedPreferredOptns));
        $commPref['preferred_communication_method'] = array_values($prefOptn);
      }
    }

    //Using API to update contact
    $contact = civicrm_api3('Contact', 'create', $commPref);

    //By now we have updated the contact preferences, now update groups selected by User.
    $groups = self::getGroups();
    foreach ($groups as $groupId => $group) {
      $container_name = 'group_' . $group['id'];
      if (!empty($groupSettings[$container_name]['group_enable'])) {
        $groupDetails = [
          'contact_id' => $contactId,
          'group_id'   => $group['id'],
        ];
        //Make sure contact is already exist in group and want to remove /add ?
        $existsInGroup = civicrm_api3('GroupContact', 'get', $groupDetails);

        //Set status added or removed based on user selection
        $status = !empty($submittedValues[$container_name]) ? 'Added' : 'Removed';
        $groupDetails['status'] = $status;

        //check before Add / Remove from group.
        if ((!empty($existsInGroup['id']) && $status == 'Removed')
          OR (empty($existsInGroup['id']) && $status == 'Added')
        ) {
          $groupResult = civicrm_api3('GroupContact', 'create', $groupDetails);
        }
      }
    }//end foreach groups
  }

  /**
   * @param int $contactID
   * @param array $submittedValues
   *
   * @return array|false|null
   * @throws \API_Exception
   * @throws \Civi\API\Exception\UnauthorizedException
   */
  public static function createCommsPrefActivity($contactID, $submittedValues = []) {
    if (empty($contactID)) {
      return FALSE;
    }

    // Create Activity for communication preference updated
    $activity = Activity::create(FALSE)
      ->addValue('activity_type_id:name', 'Update_Communication_Preferences')
      ->addValue('source_contact_id', CRM_Core_Session::getLoggedInContactID() ?? $contactID)
      ->addValue('target_contact_id', $contactID)
      ->addValue('subject', $submittedValues['subject'] ?? E::ts('Communication Preferences updated'))
      ->addValue('activity_date_time', date('Y-m-d H:i:s'))
      ->addValue('status_id:name', 'Completed');
    if (!empty($submittedValues['activity_source'])) {
      $activity->addValue('details', $submittedValues['activity_source']);
    }
    return $activity->execute()->first();
  }

  /**
   * @param int $cid
   * @param \CRM_Core_Form $form
   * @param string $entity
   *
   * @throws \CRM_Core_Exception
   */
  public static function commsPreferenceInThankyouPage($cid, &$form, $entity = 'Event') {
    if (empty($cid)) {
      return;
    }

    $settings = self::getSettings();
    $fieldsSettings = $settings[self::SETTING_NAME];
    //Assign required variables to smarty
    $form->assign('entity', $entity);
    $form->assign('contactId', $cid);
    $cs = CRM_Contact_BAO_Contact_Utils::generateChecksum($cid);
    $form->assign('contact_cs', $cs);
    $form->assign('comm_pref_in_thankyou', $fieldsSettings['comm_pref_in_thankyou']);
    switch ($fieldsSettings['comm_pref_in_thankyou']) {
      case 'embed':

          $ajax_permission[] = ['access AJAX API', 'access CiviCRM'];
          if (CRM_Core_Permission::check($ajax_permission)) {
            $form->assign('noperm', 0);
            // Inject comms preference fields in contribution thank you page.
            self::injectCommPreferenceFieldsIntoForm($form);
            $channelDefaults = self::getChannelPrefsForContact($cid);
            $groupDefaults = self::getGroupSelectionsForContact($cid);
            $defaults = array_merge($groupDefaults, $channelDefaults);
            $form->setDefaults($defaults);
          }
          else {
            $form->add('hidden', 'noperm', '1');
            $form->assign('noperm', 1);
          }
        break;

      case 'link':
          self::addCommsPreferenceLinkInThankYouPage($cid, $form, $entity);
        break;

      default:
        break;

    }
  }
}
