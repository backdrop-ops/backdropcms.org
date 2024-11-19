<?php

use CRM_Gdpr_ExtensionUtil as E;

/**
 * Form controller class
 *
 * @see https://wiki.civicrm.org/confluence/display/CRMDOC/QuickForm+Reference
 */
class CRM_Gdpr_Form_ManageEvent_TermsAndConditions extends CRM_Event_Form_ManageEvent {
  use CRM_Gdpr_Form_GroupTreeTrait;

  /**
   * We use a custom field group to store values for this event.
   */
  private $groupId = NULL;

  private $uploadElement = 'Terms_and_Conditions_File_Upload';

  public function preProcess() {
    parent::preProcess();
    $this->_type = $this->_cdType = 'Event';
    $this->_groupCount = 1;
    $this->_id = $this->id = $this->_entityId = CRM_Utils_Request::retrieve('id', 'Positive');
    $group_id = $this->getGroupId();
    $this->_subName = '';
    $this->_onlySubtype = FALSE;
    $this->assign('cdType', FALSE);
    $this->assign('cgCount', $this->_groupCount);
    $this->setGroupTree('', $group_id, $this->_onlySubtype);
  }

  /**
   * Get the Id of the custom group for Event terms and conditions.
   */
  private function getGroupId() {
    if (!$this->groupId)
      $result = civicrm_api3('CustomGroup', 'get', [
        'sequential' => 1,
        'name' => 'Event_terms_and_conditions',
      ]);
    if (!empty($result['values'][0])) {
      $this->groupId = $result['values'][0]['id'];
    }
    return $this->groupId;
  }

  /**
   * Gets Custom field from the group tree by name.
   */
  private function getFieldByName($field_name) {
    static $fields = [];
    if (empty($fields)) {
      $tree = $this->_groupTree;
      $group = reset($tree);
      if (!empty($group['fields'])) {
        foreach ($group['fields'] as $fid => $field) {
          if (!$field) {
            continue;
          }
          if (!empty($field['name'])) {
            $fields[$field['name']] = $field;
          }
          else {
            // CiviCRM < 4.7.21 do not include name property
            // in the tree. We need to get more data.
            $api_field = $this->lookupFieldById($fid);
            if (!empty($api_field['name'])) {
              $field = array_merge($api_field, $field);
              $fields[$field['name']] = $field;
            }
          }
        }
      }
    }
    return $fields[$field_name] ? $fields[$field_name] : [];
  }

  /**
   * Fetches a Custom Field definition from the API.
   */
  private function lookupFieldById($field_id) {
    static $fields = [];
    if (!$fields) {
      $results = civicrm_api3('CustomField', 'get', [
        'custom_group_id' => $this->groupId,
        'sequential' => 0,
      ]);
      if (!empty($results['values'])) {
        $fields = $results['values'];
      }
    }
    return isset($fields[$field_id]) ? $fields[$field_id] : [];
  }


  /**
   * Gets the element for a custom field by the name of the field.
   *
   * @param
   *  Name of the custom field. Note the group is already known, so uniqueness
   *  is preserved.
   */
  private function getElementByFieldName($field_name) {
    $field = $this->getFieldByName($field_name);
    $element = [];
    if ($field) {
      $element = $this->getElement($field['element_name']);
    }
    return $element;
  }

  /**
   * Set defaults.
   *
   * @return array
   */
  public function setDefaultValues() {
    $defaults = CRM_Custom_Form_CustomData::setDefaultValues($this);
    // Fill in missing values with defaults from the field settings.
    $tree = $this->_groupTree;
    $group = reset($tree);
    if (!empty($group['fields'])) {
      foreach ($group['fields'] as $fid => $field) {
        if (empty($defaults[$field['element_name']]) && !empty($field['default_value'])) {
          $fields[$field['name']] = $field;
          $defaults[$field['element_name']] = $field['default_value'];
        }
      }
    }
    return $defaults;
  }

  /**
   * Build quick form.
   */
  public function buildQuickForm() {
    CRM_Core_BAO_CustomGroup::buildQuickForm($this, $this->_groupTree);

    // Add a file upload element for the terms and conditions file.
    $tc_field = $this->getFieldByName('Terms_and_Conditions_File');
    if ($tc_field) {
      $upload_name = $this->uploadElement;
      $upload = $this->add(
        'file',
        $upload_name,
        'Terms &amp; Conditions File'
      );
      $tc_current = [
        'url' => $tc_field['element_value'] ?? '',
        'label' => basename($tc_field['element_value'] ?? ''),
      ];
      $tc_value = $tc_field['element_value'] ?? '';
      // Provide some variables so the template can display the upload field in
      // place of the link field.
      $this->assign('terms_conditions_link_element_name', $tc_field['element_name']);
      $this->assign('terms_conditions_file_element_name', $upload_name);
      $this->assign('terms_conditions_current', $tc_current);
    }
    // Set the size of text elements.
    foreach ($this->_elements as $element) {
      if ($element->getType() == 'text') {
        $element->setSize(60);
      }
    }
    $this->assignDescriptions();
    $this->assign('elementNames', $this->getRenderableElementNames());
    parent::buildQuickForm();
  }

  /**
   * Checks for a version.
   *
   * @param string $version
   */
  private function versionIs($version) {
    return 0 === strpos(CRM_Utils_System::version(), $version);
  }

  public function postProcess() {
    $params = $this->controller->exportValues($this->_name);
    $file_url = $this->saveTCFile();
    if ($file_url) {
    $tc_field = $this->getFieldByName('Terms_and_Conditions_File');
      $params[$tc_field['element_name']] = $file_url;
    }
    if ($this->versionIs('4.6')) {
      $customFields = CRM_Core_BAO_CustomField::getFields('Event', FALSE, FALSE,
          CRM_Utils_Array::value('event_type_id', $params)
      );
      $params['custom'] = CRM_Core_BAO_CustomField::postProcess(
        $params,
        $customFields,
        $this->_id,
        'Event'
      );
      CRM_Core_BAO_CustomValueTable::store($params['custom'], 'civicrm_event', $this->id);
    }
    else {
      // 4.7
      $params['custom'] = CRM_Core_BAO_CustomField::postProcess($params,
        $this->_id,
        'Event'
      );
      CRM_Core_BAO_CustomValueTable::store($params['custom'], 'civicrm_event', $this->id);
    }
    $this->preventAjaxSubmit();
    parent::endPostProcess();
  }

  /**
   * Assigns template variable descriptions with the preHelp text of the field.
   */
  private function assignDescriptions() {
    $tree = $this->_groupTree;
    $group = reset($tree);
    $descriptions = [];
    if (!empty($group['fields'])) {
      foreach ($group['fields'] as $fid => $field) {
        $descriptions[$field['element_name']] = !empty($field['help_pre']) ?  $field['help_pre'] : '';
      }
    }
    $this->assign('descriptions', $descriptions);
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

  /**
   * Save an uploaded Terms and Conditions file.
   *  @return string
   *    Path of the saved file.
   */
  private function saveTCFile() {
    $fileElement = $this->getElement($this->uploadElement);
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
