<?php
use CRM_Gdpr_ExtensionUtil as E;

/**
 * @file
 *  Base class for Terms and Conditions relating to a particular entity.
 */
class CRM_Gdpr_SLA_Entity {

  protected $id = NULL;

  protected $entity = [];

  protected $type = '';

  protected $customGroup = '';

  protected $customFields = [];

  protected $activityType = '';

  protected $activityCustomGroup = '';

  /**
   * Name in the settings to look up whether Terms & Conditions
   * behaviour is globally enabled for this type.
   */
  protected $enabledSetting = '';
  protected $urlSetting = '';

  /**
   * Default values are provided in the GDPR settings.
   */
  protected $settings = [];

  /**
   * Determines if this entity has terms and conditions enabled.
   *
   * @param bool $useDefault
   *  If true, will fall back to the defaults for the entity type from the gdpr settings if terms and conditions are not set, otherwise will
   *  only use the settings for this particular entity.
   */
  public function isEnabled($useDefault = FALSE) {
    $entityEnabled = $this->getValue('Enable_terms_and_Conditions_Acceptance');
    // If TC explicitlty disabled for this entity, return false.
    if ($entityEnabled === '0') {
      return FALSE;
    }
    $typeEnabled = FALSE;
    // Determine whether to fall back to global settings.
    if ($useDefault && $this->enabledSetting) {
      $typeEnabled = $this->getSetting($this->enabledSetting);
    }
    return $entityEnabled || $typeEnabled;
  }

  public function getCheckboxPosition() {
    return $this->getValue('Checkbox_Position');
  }

  public function getCheckboxText() {
    return $this->getValue('Checkbox_text');
  }

  public function getIntroduction() {
    return $this->getValue('Introduction');
  }

  public function getUrl($useDefault = FALSE) {
    $url = $this->getValue('Terms_and_Conditions_File');
    if (!$useDefault) {
      return $url;
    }
    return $this->getSetting('entity_tc');
  }

  /**
   * @return array
   */
  public function getLinks() {
    $links = [];
    $url = $this->getUrl();
    $label = $this->getValue('Link_Label');
    if ($url) {
      $links['entity'] = [
        'url' => $url,
        'label' => $label,
      ];
    }
    $global_link_url = CRM_Gdpr_SLA_Utils::getTermsConditionsUrl();
    $global_link_label = CRM_Gdpr_SLA_Utils::getLinkLabel();
    if ($global_link_url) {
      $links['global'] = [
        'url' => $global_link_url,
        'label' => $global_link_label,
      ];
    }
    return $links;
  }

  /**
   * @param string $field_name
   *
   * @return mixed|void
   */
  public function getValue($field_name) {
    $fields = $this->getCustomFields($this->customGroup);
    $field = !empty($fields[$field_name]) ? $fields[$field_name] : [];
    if (!$field) {
      return;
    }
    $fid = $field['id'];
    $key = 'custom_' . $fid;
    $entity = $this->getEntity();
    if ($fid && $entity && isset($entity[$key])) {
      return $entity[$key];
    }
  }

  protected function getCustomFields($group) {
    if (empty($this->customFields[$group])) {
      $result = civicrm_api3('CustomField', 'get', [
        'sequential' => 1,
        'custom_group_id' => $group,
      ]);
      if (!empty($result['values'])) {
        $fields = [];
        //Index them by name for easier lookups.
        foreach ($result['values'] as $value) {
          $fields[$value['name']] = $value;
        }
        $this->customFields[$group] = $fields;
      }
    }
    return $this->customFields[$group];
  }

  public function __construct($id, $type) {
    $this->id = $id;
    $this->type = $type;
    $this->settings = CRM_Gdpr_Utils::getGDPRSettings();
  }

  public function getEntity() {
    if (!$this->entity) {
      $params = [
        'sequential' => FALSE,
        'id' => $this->id,
      ];
      $result = civicrm_api3($this->type, 'get', $params);
      if (!empty($result['values'][$this->id])) {
        $this->entity = $result['values'][$this->id];
      }
    }
    return $this->entity;
  }

  /**
   * Adds Terms & Conditions checkboxes to a form.
   *
   * @param CRM_Core_Form $form
   */
  public function addField($form) {
    $settings = $this->settings;
    // Enabled just for this entity.
    if ($this->isEnabled()) {
      $intro = $this->getIntroduction();
      $links = $this->getLinks();
      $position = $this->getCheckboxPosition();
      $checkboxText = $this->getCheckboxText();
    }
    elseif (!empty($settings['entity_tc']) || !empty($settings['entity_tc_link'])) {
      // If enabled for the type, use the defaults.
      if ($this->isEnabled(TRUE)) {
        // Use sitewide defaults for terms and conditions.
        $intro = $settings['entity_tc_intro'];
        $position = $settings['entity_tc_position'];
        $checkboxText = $settings['entity_tc_checkbox_text'];
        $links = $this->getLinks();
        $links['entity']['label'] = $settings['entity_tc_link_label'];
        if (array_key_exists('entity_tc_option', $settings)) {
          switch ($settings['entity_tc_option']) {
            // File uploaded
            case 1:
            default:
              $links['entity']['url'] = $settings['entity_tc'];
              break;

            // Web page link
            case 2:
              $links['entity']['url'] = $settings['entity_tc_link'];
              break;
          }
        }
      }
    }

    if (!empty($links['entity'])) {
      $form->add('checkbox', 'accept_entity_tc', $checkboxText, NULL, TRUE);
    }

    if (!empty($links['global'])) {
      $form->add('checkbox', 'accept_tc', CRM_Gdpr_SLA_Utils::getCheckboxText(), NULL, TRUE);
    }
    if (!empty($links)) {
      $tc_vars = [
        'element' => 'accept_tc',
        'links' => $links,
        'intro' => $intro,
        'position' => $position,
      ];
      $form->assign('terms_conditions', $tc_vars);
      CRM_Core_Region::instance('page-body')->add([
        'template' => "CRM/Gdpr/TermsConditionsField.tpl",
      ]);
    }
  }

  /**
   * Get a value from the GDPR settings.
   */
  private function getSetting($settingName) {
    if (isset($this->settings[$settingName])) {
      return $this->settings[$settingName];
    }
  }

  public function recordAcceptance($contactId = NULL) {
    $contactId = $contactId ? $contactId : CRM_Core_Session::singleton()->getLoggedInContactID();
    $fields = $this->getCustomFields($this->activityCustomGroup);
    $url = $this->getUrl();
    $entity = $this->getEntity();
    $source = $this->type . ': ' . $entity['title'] . ' (' . $entity['id'] . ')';
    $params = [
      'source_contact_id' => $contactId,
      'target_id' => $contactId,
      'subject' => $this->type . E::ts(' Terms and Conditions accepted'),
      'status_id' => 'Completed',
      'activity_type_id' => $this->activityType,
      'custom_' . $fields['Terms_Conditions']['id'] => $url,
      'custom_' . $fields['Source']['id'] => $source,
    ];
    $result = civicrm_api3('Activity', 'create', $params);
  }

}
