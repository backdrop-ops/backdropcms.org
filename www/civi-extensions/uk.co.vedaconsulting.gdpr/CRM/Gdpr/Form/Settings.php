<?php

use CRM_Gdpr_ExtensionUtil as E;
require_once 'CRM/Core/Form.php';

/**
 * Form controller class
 *
 * @see http://wiki.civicrm.org/confluence/display/CRMDOC43/QuickForm+Reference
 */
class CRM_Gdpr_Form_Settings extends CRM_Core_Form {

  function preProcess() {
    //check all custom datas are being installed properly
    CRM_Core_Resources::singleton()->addStyleFile('uk.co.vedaconsulting.gdpr', 'css/gdpr.css');
    $status = CRM_Gdpr_Utils::checkIntallationIssues();
    $this->assign('statusCheck', $status);

    parent::preProcess();
  }

  function buildQuickForm() {
    CRM_Utils_System::setTitle(E::ts('GDPR - Settings'));

    $this->addEntityRef('data_officer', E::ts('Point of Contact'), [
        'create' => TRUE,
        'api' => ['extra' => ['email']],
      ], TRUE);

    // Get all activity types
    $actTypes = CRM_Gdpr_Utils::getAllActivityTypes();

    // Activity types
    $this->add(
      'select',
      'activity_type',
      E::ts('Activity Types'),
      ['' => E::ts('- select -')] + $actTypes, // list of options
      TRUE,
      ['class' => 'crm-select2 huge', 'multiple' => 'multiple',]
    );

    //Track Exports
    $this->addElement('checkbox', 'track_exports', E::ts('Do you Want to track Exports?'), NULL);

    // Get all contact types
    $contactTypes = CRM_Gdpr_Utils::getAllContactTypes($parentOnly = TRUE);
    $this->add(
      'select',
      'contact_type',
      E::ts('Contact Types'),
      ['' => E::ts('- select -')] + $contactTypes, // list of options
      TRUE,
      ['class' => 'crm-select2 huge', 'multiple' => 'multiple',]
    );

    // Forget me action
    $this->add('text', 'forgetme_name', E::ts('Forgetme contact name'));
    $this->add('text', 'forgetme_email', E::ts('Forgetme e-mail'));

    // Activity types
    $this->add(
      'select',
      'forgetme_activity_type',
      E::ts('Delete Activities of Types'),
      ['' => E::ts('- select -')] + $actTypes, // list of options
      FALSE,
      ['class' => 'crm-select2 huge', 'multiple' => 'multiple',]
    );

    $customGroups = CRM_Gdpr_Utils::getCustomGroups();
    $this->add(
      'select',
      'forgetme_custom_groups',
      E::ts('Custom groups'),
      ['' => E::ts('- select -')] + $customGroups,
      FALSE,
      ['class' => 'crm-select2 huge', 'multiple' => 'multiple',]
    );

    //Email to Point of Contact/DPO when someone access forget me.
    $this->add('checkbox', 'email_to_dpo', E::ts('Email the Point of Contact / DPO?'));
    $this->add('text', 'email_dpo_subject', E::ts('Email Subject'));

    $this->add(
      'text',
      'activity_period',
      E::ts('Period'),
      ['size' => 4],
      TRUE
    );

    $months = range(6, 60, 6);
    $slaPeriodOptions = array_combine($months, $months);
    // SLA Acceptance settings.
    $this->add(
      'text',
      'sla_page_title',
      E::ts('Page title')
    );
    $this->add(
      'select',
      'sla_period',
      E::ts('Acceptance period (months)'),
      ['' => E::ts('- select -')] + $slaPeriodOptions, // list of options
      TRUE,
      ['class' => 'crm-select2']
    );
    $dataPolicyOptions = [
      '1' => E::ts('File Upload'),
      '2' => E::ts('Web page link'),
    ];
    $this->addRadio('sla_data_policy_option',
      ts('Data Policy options'),
      $dataPolicyOptions,
      [],
      '&nbsp;', FALSE
    );
    $this->add(
      'file',
      'sla_tc_upload',
      E::ts('Data Policy file')
    );
    $this->add(
      'text',
      'sla_tc_link',
      E::ts('Data Policy link'),
      ['class' => 'huge']
    );
    $this->add(
      'checkbox',
      'sla_tc_new_version',
      E::ts('This is a new version of the document.')
    );
    $this->add(
      'hidden',
      'sla_tc_version'
    );
    $this->add(
      'text',
      'sla_link_label',
      E::ts('Link Label')
    );
    $this->add(
      'hidden',
      'sla_tc'
    );
    $this->add(
      'text',
      'sla_checkbox_text',
      E::ts('Checkbox text')
    );
    $this->add(
      'textarea',
      'sla_agreement_text',
      E::ts('Introductory text'),
      ['cols' => 50]
    );
    // Entity (Event/Contribution) terms and conditions.
    $this->add(
      'advcheckbox',
      'event_tc_enable',
      E::ts('Enable Terms and Conditions for every event')
    );
    $this->add(
      'advcheckbox',
      'cp_tc_enable',
      E::ts('Enable Terms and Conditions for every Contribution Page')
    );
    //If T+C is enabled for both Events Contribution Pages they will share the
    //following settings.
    $this->add(
      'select',
      'entity_tc_position',
      E::ts('Checkbox Position'),
      [
        'customPre' => E::ts('Top profile'),
        'customPost' => E::ts('Bottom profile'),
        'formTop' => E::ts('Top of form'),
        'formBottom' => E::ts('Bottom of form')
      ]
    );
    $this->add(
      'text',
      'entity_tc_link_label',
      E::ts('Link Label')
    );
    $this->add(
      'text',
      'entity_tc_checkbox_text',
      E::ts('Checkbox text')
    );
    $this->add(
      'textarea',
      'entity_tc_intro',
      E::ts('Introduction'),
      ['cols' => 50]
    );
    $this->addRadio('entity_tc_option',
    E::ts('Terms and Conditions options'),
      $dataPolicyOptions,
      [],
      '&nbsp;', FALSE
    );
    $this->add(
      'file',
      'entity_tc_upload',
      E::ts('Default Terms and Conditions file')
    );
    $this->add(
      'text',
      'entity_tc_link',
      E::ts('Default Terms and Conditions link'),
      ['class' => 'huge']
    );
    $this->add(
      'hidden',
      'entity_tc'
    );
    $entity_tc_elements = [
      'event_tc_enable',
      'cp_tc_enable',
      'entity_tc_position',
      'entity_tc_link_label',
      'entity_tc_checkbox_text',
      'entity_tc_intro',
    ];
    $this->assign('entity_tc_elements', $entity_tc_elements);
    $this->addButtons([
      [
        'type' => 'submit',
        'name' => E::ts('Save'),
        'isDefault' => TRUE,
      ],
    ]);

    $bare_defaults = [
      'sla_link_label' => E::ts('Data Policy'),
      'sla_checkbox_text' => E::ts('I accept the Data Policy.'),
      'sla_tc_new_version' => FALSE,
      'entity_tc_link_label' => E::ts('Terms &amp; Conditions'),
      'entity_tc_checkbox_text' => E::ts('I accept the Terms &amp; Conditions'),
      'entity_tc_intro' => E::ts('Please read and accept the Terms &amp; Conditions and Data Policy.'),
    ];

    // Get GDPR settings, for setting defaults
    $defaults = CRM_Gdpr_Utils::getGDPRSettings();
    $defaults = array_merge($bare_defaults, $defaults);
    $defaults['track_exports'] = CRM_Gdpr_Utils::getItem(CRM_Gdpr_Constants::GDPR_SETTING_GROUP, 'track_exports', NULL, FALSE);
    // Set defaults
    if (!empty($defaults)) {
      $this->setDefaults($defaults);
    }
    // Pass on variables to link to terms and conditions.
    if (!empty($defaults['sla_tc']) || !empty($defaults['sla_tc_link'])) {
      switch ($defaults['sla_data_policy_option']) {
        // File uploaded
        case 1:
        default:
          $sla_tc['url'] = $defaults['sla_tc'];
          $sla_tc['name'] = basename($defaults['sla_tc']);
          break;

        // Web page link
        case 2:
          $sla_tc['url'] = $defaults['sla_tc_link'];
          break;
      }
      $this->assign('sla_data_policy_option', $defaults['sla_data_policy_option']);
      $this->assign('sla_tc_current', $sla_tc);
      $version = !empty($defaults['sla_tc_version']) ? $defaults['sla_tc_version'] : 1;
      $this->assign('sla_tc_version', $version);
      $updated = !empty($defaults['sla_tc_updated']) ? $defaults['sla_tc_updated'] : '';
      if ($updated) {
        $this->assign('sla_tc_updated', $updated);
      }
    }
    if (!empty($defaults['entity_tc'])  || !empty($defaults['entity_tc_link'])) {
      switch ($defaults['entity_tc_option']) {
        // File uploaded
        case 1:
        default:
          $entity_tc['url'] = $defaults['entity_tc'];
          $entity_tc['name'] = basename($defaults['entity_tc']);
          break;

        // Web page link
        case 2:
          $entity_tc['url'] = $defaults['entity_tc_link'];
          break;
      }
      $this->assign('entity_tc_option', $defaults['entity_tc_option']);
      $this->assign('entity_tc_current', $entity_tc);
    }
    parent::buildQuickForm();
  }

  function postProcess() {
    $values = $this->exportValues();

    $settings = [];
    $settings['data_officer'] = $values['data_officer'];
    $settings['activity_type'] = $values['activity_type'];
    $settings['activity_period'] = $values['activity_period'];
    $settings['contact_type'] = $values['contact_type'];
    $settings['forgetme_name'] = $values['forgetme_name'];
    $settings['forgetme_email'] = $values['forgetme_email'];
    $settings['forgetme_activity_type'] = $values['forgetme_activity_type'];
    $settings['forgetme_custom_groups'] = $values['forgetme_custom_groups'];
    $settings['email_to_dpo'] = isset($values['email_to_dpo']) ? $values['email_to_dpo'] : 0;
    $settings['email_dpo_subject'] = $values['email_dpo_subject'];
    $settings['sla_period'] = $values['sla_period'];
    $settings['sla_data_policy_option'] = $values['sla_data_policy_option'];
    // Privacy policy link
    if ($values['sla_data_policy_option'] == 2) {
      $settings['sla_tc_link'] = $values['sla_tc_link'];
    }
    $settings['sla_prompt'] = !empty($values['sla_prompt']) ? 1 : 0;
    $settings['sla_agreement_text'] = $values['sla_agreement_text'];
    $settings['sla_link_label'] = $values['sla_link_label'];
    $settings['sla_page_title'] = $values['sla_page_title'];
    $settings['sla_checkbox_text'] = $values['sla_checkbox_text'];
    $settings['event_tc_enable'] = $values['event_tc_enable'];
    $settings['cp_tc_enable'] = $values['cp_tc_enable'];
    $settings['entity_tc_position'] = $values['entity_tc_position'];
    $settings['entity_tc_link_label'] = $values['entity_tc_link_label'];
    $settings['entity_tc_checkbox_text'] = $values['entity_tc_checkbox_text'];
    $settings['entity_tc_intro'] = $values['entity_tc_intro'];
    $settings['entity_tc_option'] = $values['entity_tc_option'];
    // Terms and conditions link
    if ($values['entity_tc_option'] == 2) {
      $settings['entity_tc_link'] = $values['entity_tc_link'];
    }
    // Map the upload file element to setting name.
    $upload_elems = [
      'sla_tc_upload' => 'sla_tc',
      'entity_tc_upload' => 'entity_tc',
    ];
    foreach ($upload_elems as $elem => $setting) {
      $uploadFile = $this->saveTCFile($elem);
      if ($uploadFile) {
        $settings[$setting] = $uploadFile;
      } else {
        $settings[$setting] = !empty($values[$setting]) ? $values[$setting] : NULL;
      }
    }
    // Is this a new version of the global Data Policy?
    $version = isset($values['sla_tc_version']) && is_numeric($values['sla_tc_version']) ? $values['sla_tc_version'] : 1;
    if (!empty($values['sla_tc_new_version'])) {
      $version++;
      $settings['sla_tc_updated'] = date('Y-m-d');
    }
    $settings['sla_tc_version'] = $version;
    $settingsStr = serialize($settings);

    // Save the settings
    CRM_Gdpr_Utils::setItem($settingsStr, CRM_Gdpr_Constants::GDPR_SETTING_GROUP, CRM_Gdpr_Constants::GDPR_SETTING_NAME);
    $trackExports = NULL;
    if(isset($values['track_exports'])){
      $trackExports = $values['track_exports'];
    }
    CRM_Gdpr_Utils::setItem($trackExports, CRM_Gdpr_Constants::GDPR_SETTING_GROUP,'track_exports');

    $message = "GDPR settings saved.";
    $url = CRM_Utils_System::url('civicrm/gdpr/dashboard', 'reset=1');
    CRM_Core_Session::setStatus($message, 'GDPR', 'success');
    CRM_Utils_System::redirect($url);
    CRM_Utils_System::civiExit();
  }

  public static function formRule($params, $files) {

  }

  /**
   * Save an uploaded Terms and Conditions file.
   *  @return string
   *    Path of the saved file.
   */
  private function saveTCFile($element_name) {
    if (empty($this->_elementIndex[$element_name])) {
      return FALSE;
    }
    $fileElement = $this->_elements[$this->_elementIndex[$element_name]];
    if ($fileElement && !empty($fileElement->_value['name'])) {
      $config = CRM_Core_Config::singleton();
      $publicUploadDir = $config->imageUploadDir;
      $delim = '/';
      $publicUploadDir = substr($publicUploadDir, -1) == $delim ? $publicUploadDir : $publicUploadDir . $delim;
      $fileInfo = $fileElement->_value;
      $pathInfo = pathinfo($fileElement->_value['name']);
      if (empty($pathInfo['filename'])) {
        return;
      }
      // If necessary add a delta to the file name to avoid writing over an existing file.
      $delta = 0;
      $fileName = '';

      while (!$fileName) {
        $suffix = $delta ? '-' . $delta : '';
        $testName = $pathInfo['filename'] . $suffix . '.' . $pathInfo['extension'];
        if (!file_exists($publicUploadDir . $testName)) {
          $fileName = $testName;
        }
        $delta++;
      }
      // Move to public uploads directory and create file record.
      // This will be referenced in Activity custom field.
      $saved = $fileElement->moveUploadedFile($publicUploadDir, $fileName);
      if ($saved) {
        return $this->getFileUrl($publicUploadDir . $fileName);
      }
    }
  }

  /**
   * Gets the url of an uploaded file from its filesystem path.
   *
   * @param string $path
   *
   * return string
   */
  private function getFileUrl($path) {
    $config = CRM_Core_Config::singleton();
    $cmsRoot = $config->userSystem->cmsRootPath();
    if (0 === strpos($path, $cmsRoot)) {
      $url = substr($path, strlen($cmsRoot));
      return $url;
    }
  }
}
