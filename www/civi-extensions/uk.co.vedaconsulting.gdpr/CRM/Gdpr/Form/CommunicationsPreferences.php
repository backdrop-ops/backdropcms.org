<?php

use CRM_Gdpr_ExtensionUtil as E;
use CRM_Gdpr_CommunicationsPreferences_Utils as U;

/**
 * Form controller class
 *
 * @see https://wiki.civicrm.org/confluence/display/CRMDOC/QuickForm+Reference
 */
class CRM_Gdpr_Form_CommunicationsPreferences extends CRM_Core_Form {
  /**
   * API values of public groups.
   */
  protected $groups = [];

  protected $groupContainerNames = [];

  public function buildQuickForm() {
    CRM_Utils_System::setTitle(E::ts('Communications Preferences'));
    $channels = U::getChannelOptions();
    $text_area_attributes = ['cols' => 60, 'rows' => 5];
    $this->add(
      'text',
      'page_title',
      E::ts('Page title'),
      ['size' => 40]
    );
    $this->add(
      'wysiwyg',
      'page_intro',
      E::ts('Introduction'),
      $text_area_attributes
    );
    $this->add(
      'select',
      'profile',
      E::ts('Include Profile'),
      U::getProfileOptions()
    );
    $descriptions['profile'] = E::ts('Include a profile so the user can identify and check their details are up-to-date. It should  include a primary email address field.');
    $this->add(
      'advcheckbox',
      'use_as_mailing_subscribe',
      E::ts('Use as the mailing subscribe page'),
      '',
      false
    );
    $descriptions['use_as_mailing_subscribe'] = E::ts('Check to use the Communications Preferences page instead of the default Mailing Subscribe page.');
    $this->add(
      'advcheckbox',
      'add_captcha',
      E::ts('Include reCAPTCHA?'),
      '',
      false
    );
    $recaptchaSettingsUrl = CRM_Utils_System::url('civicrm/admin/setting/misc', 'reset=1');
    $descriptions['add_captcha'] = E::ts('Check to use reCAPTCHA in Communications Preferences page. Make sure you have configured the <a href="'.$recaptchaSettingsUrl.'">reCAPTCHA keys</a>.');
    // Let the template know about elements in this section.
    $page_elements = [
      'page_title',
      'page_intro',
      'profile',
      'use_as_mailing_subscribe',
      'add_captcha'
    ];
    $this->assign('page_elements', $page_elements);
    // Comms prefs channels
    $this->add(
      'advcheckbox',
      'enable_channels',
      E::ts('Enable Channels'),
      '',
      false,
      [
        'data-toggle' => '.channels-wrapper',
        'class' => 'toggle-control'
      ]
    );
    $this->add(
      'text',
      'channels_intro',
      E::ts('Heading for the channels section'),
      ['size' => 40]
    );
    $channel_group = $this->add(
      'group',
      'channels',
      E::ts('Users can opt-in to these channels')
    );
    foreach ($channels as $channel => $label) {
      $elem = HTML_QuickForm::createElement(
        'checkbox',
        'enable_' . $channel,
        $label,
        $label,
        ['class' => 'enable-channel']
      );
      $channel_checkboxes[] = $elem;
    }
    $channel_group->setElements($channel_checkboxes);
    $channels_elements = [
      'channels_intro',
      'channels',
    ];
    $this->assign('channels_elements', $channels_elements);
    $this->add(
      'checkbox',
      'enable_groups',
      E::ts('Allow users to opt-in to mailing groups.'),
      '',
      false,
      [
        'data-toggle' => '.groups-wrapper',
        'class' => 'toggle-control'
      ]
    );
    $this->add(
      'text',
      'groups_heading',
      E::ts('Heading for the groups section'),
      ['size' => 40]
    );
    $this->add(
      'wysiwyg',
      'groups_intro',
      E::ts('Introduction or description for this section.'),
      $text_area_attributes
    );
    $groups = $this->getGroups();
    $group_containers = [];
    foreach ($groups as $group) {
      $container_name = 'group_' . $group['id'];
      $this->groupContainerNames[] = $container_name;

      $group_container = $this->add(
        'group',
        $container_name,
        $group['title']
      );
      $group_elems = [];
      $group_elems[] = HTML_QuickForm::createElement(
        'advcheckbox',
        'group_enable',
        E::ts('Enable'),
        '',
        [
         'data-group-id' => $group['id'],
        ]
      );
      $group_elems[] = HTML_QuickForm::createElement(
        'text',
        'group_title',
        $group['title'],
        ['size' => 30]
      );
      $weight_opts = range(0, 50);
      $weight_opts = array_combine($weight_opts, $weight_opts);

      $group_elems[] = HTML_QuickForm::createElement(
        'select',
        'group_weight',
        E::ts('Weight'),
        $weight_opts
      );
      $group_elems[] = HTML_QuickForm::createElement(
        'textarea',
        'group_description',
        'Description',
        [
          'cols' => 30,
          'rows' => 6
        ]
      );
      foreach ($channels as $key => $label) {
        $group_elems[] = HTML_Quickform::createElement(
          'advcheckbox',
          $key,
          $label,
          $label
        );
      }
      $group_container->setElements($group_elems);
      $group_containers[] = $container_name;
    }
    $this->addRadio(
      'completion_redirect',
      E::ts('On completion'),
      [1 => E::ts('Redirect to another page'), 0 => E::ts('Display a message on the form page.')]
    );
    $this->add(
      'text',
      'completion_url',
      E::ts('Completion page'),
      ['size' => 50]
    );
    $descriptions['completion_url'] = E::ts('Add the a URL for a page to redirect the user after they complete the form. The page should already exist. The URL may be absolute (http://example.com/thank-you) or relative (thank-you), with no leading forward slash. Leave blank to redirect to the front page.');
    $this->add(
      'wysiwyg',
      'completion_message',
      E::ts('Completion message'),
      $text_area_attributes
    );
    $descriptions['completion_message'] = E::ts('A message to display to the user after the form is submitted. ');
    // Let the template know about which fields belong in the groups section.
    $groups_elements = [
      'groups_heading',
      'groups_intro',
    ];
    $this->assign('descriptions', $descriptions);
    $this->assign('groups_elements', $groups_elements);
    $this->assign('group_containers', $group_containers);
    // Use the current logged in user for the preview.
    $current_cid = CRM_Core_Session::singleton()->getLoggedInContactID();
    if ($current_cid) {
      $url = CRM_Gdpr_CommunicationsPreferences_Utils::getCommPreferenceURLForContact($current_cid);
      $this->assign('communications_preferences_page_url', $url);
    }

    $this->addRadio(
      'comm_pref_in_thankyou',
      E::ts('Add to Event and Contribution Thank-you pages'),
      [
        'embed' => E::ts('Embed the Communication Preferences form'),
        'link' => E::ts('Add a link to the form'),
        'none' => E::ts('Do Nothing')
      ],
      [
        'class' => 'toggle-select thank-you-select',
        'data-toggle-mapping' => json_encode(
          [
            'embed' => '.thank-you-embed-wrapper',
            'link' => '.thank-you-link-wrapper'
          ]
        )
      ]
    );
    $this->add(
      'wysiwyg',
      'comm_pref_thankyou_embed_intro',
      E::ts('Introductory text'),
      $text_area_attributes
    );
    $this->add(
      'wysiwyg',
      'comm_pref_thankyou_embed_complete_msg',
      E::ts('Completion text'),
      $text_area_attributes
    );
    $this->add(
      'text',
      'comm_pref_link_label',
      E::ts('Link label')
    );
    $this->add(
      'wysiwyg',
      'comm_pref_link_intro',
      E::ts('Text above the link'),
      $text_area_attributes
    );

    $this->buildMailBlock();

    $this->addButtons([
      [
        'type' => 'submit',
        'name' => E::ts('Save'),
        'isDefault' => TRUE,
      ],
    ]);
    parent::buildQuickForm();
  }

  /**
   * Build Email Block
   */
  public function buildMailBlock() {
    $this->registerRule('emailList', 'callback', 'emailList', 'CRM_Utils_Rule');
    $this->addYesNo('is_email_confirm', E::ts('Send Confirmation Email?'), NULL, NULL, ['onclick' => "return showHideByValue('is_email_confirm','','confirmEmail','block','radio',false);"]);
    $this->add('textarea', 'confirm_email_text', E::ts('Text'));
    $this->add('text', 'cc_confirm', E::ts('CC Confirmation To'));
    $this->addRule('cc_confirm', E::ts('Please enter a valid list of comma delimited email addresses'), 'emailList');
    $this->add('text', 'bcc_confirm', E::ts('BCC Confirmation To'));
    $this->addRule('bcc_confirm', E::ts('Please enter a valid list of comma delimited email addresses'), 'emailList');
    $this->add('text', 'confirm_from_name', E::ts('Confirm From Name'));
    $this->add('text', 'confirm_from_email', E::ts('Confirm From Email'));
    $this->addRule('confirm_from_email', E::ts('Email is not valid.'), 'email');
  }

  public function setDefaultValues() {
    // Set some initial defaults
    $defaults['is_email_confirm'] = 0;

    // Load defaults from settings
    $settings = U::getSettings();
    $key = U::SETTING_NAME;
    $group_key = U::GROUP_SETTING_NAME;
    $defaults = [];
    $group_settings = $settings[$group_key] ? $settings[$group_key] : [];
    $groups = $this->getGroups();
    $map = [
      'group_title' => 'title',
      'group_description' => 'description',
    ];
    foreach($groups as $id => $grp) {
      if (!empty($group_settings['group_' . $id])) {
        $item = $group_settings['group_' . $id];
      }
      else {
        $item = [];
      }
      // If value is missing in the setting, take the corresponding value from the
      // group.
      foreach($map as $setting_key => $group_key) {
        $item[$setting_key] = $item[$setting_key] ?? $grp['frontend_' . $group_key] ?? $grp[$group_key] ?? '';
      }
      // Set default weight.
      if (empty($item['group_weight'])) {
        $item['group_weight'] = 0;
      }
      // Add id  as fallback sort value.
      $item['id'] = $id;
      $group_settings['group_' . $id] = $item;
    }
    // Flatten to fit the form structure.
    if (isset($settings[$key]) && isset($group_settings)) {
      $defaults = array_merge($settings[$key], $group_settings);
    }
    return $defaults;
  }

  /**
   * Gets public groups.
   */
  function getGroups() {
    if (!$this->groups) {
      $groups = U::getGroups();
      $this->groups = U::sortGroups($groups, [
        'group_enable' => 'desc',
        'group_weight' => 'asc',
        'group_title' => 'asc',
      ]);
    }
    return $this->groups;
  }

  public function postProcess() {
    $values = $this->exportValues();
    parent::postProcess();
    $groupContainers = $this->groupContainerNames;
    // Save values to settings except for groups.
    $settingsElements = array_diff($this->getRenderableElementNames(), $groupContainers);

    // Purify HTML.
    foreach ($values as $key => $value) {
      $idx = !empty($this->_elementIndex[$key]) ? $this->_elementIndex[$key] : NULL;
      if ($idx && !empty($this->_elements[$idx]->_type) && $this->_elements[$idx]->_type == 'textarea') {
        $values[$key] = CRM_Utils_String::purifyHTML($values[$key]);
      }
    }

    foreach ($settingsElements as $settingName) {
      if (isset($values[$settingName])) {
        $settings[$settingName] = $values[$settingName];
      }
    }
    $groupSettings = [];
    foreach ($groupContainers as $key) {
      if (isset($values[$key])) {
        $groupSettings[$key] = $values[$key];
      }
    }
    $save = [
      U::SETTING_NAME => $settings,
      U::GROUP_SETTING_NAME => $groupSettings,
    ];
    U::saveSettings($save);
    $url = CRM_Utils_System::url('civicrm/gdpr/dashboard', 'reset=1');
    CRM_Core_Session::setStatus('Settings Saved.', 'GDPR', 'success');
    CRM_Utils_System::redirect($url);
    CRM_Utils_System::civiExit();
  }

  public function getDefaults() {

  }

  /**
   * Get the fields/elements defined in this form.
   *
   * @return array string
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
