<?php

use CRM_Gdpr_ExtensionUtil as E;

/**
 * Form controller class
 *
 * @see https://wiki.civicrm.org/confluence/display/CRMDOC/QuickForm+Reference
 */
class CRM_Gdpr_Form_Export extends CRM_Core_Form {

  /**
   * Contact ID.
   *
   * @var int
   */
  protected $_contactID = NULL;

  /**
   * Contact Name.
   *
   * @var string
   */
  protected $_contactName = NULL;

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

    if (!$this->_contactID) {
      CRM_Core_Error::fatal(E::ts("Something went wrong. Please contact Admin."));
    }

    //Using API to update contact
    $contact = CRM_Gdpr_Utils::CiviCRMAPIWrapper('Contact', 'getsingle', ['id' => $this->_contactID]);
    $this->_contactName = $contact['display_name'];

    CRM_Utils_System::setTitle(E::ts('Export Data - ').$this->_contactName);

    $entities = self::getDataExportEntities();

    $export_group = $this->add(
      'group',
      'export_entities',
      E::ts('Select data to export *')
    );

    foreach ($entities as $entity => $entity_values) {
      $elem = HTML_QuickForm::createElement(
        'checkbox',
        $entity,
        $entity_values['label'],
        $entity_values['label'],
        ['class' => 'enable-channel']
      );
      $entity_checkboxes[] = $elem;
    }
    $export_group->setElements($entity_checkboxes);

    $this->addRadio(
      'export_format',
      E::ts('Format'),
      [
        1 => E::ts('CSV'),
        2 => E::ts('PDF'),
      ]
    );

    $this->addButtons([
      [
        'type' => 'next',
        'name' => E::ts('Export'),
        'spacing' => '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;',
        'isDefault' => TRUE,
      ],
      [
        'type' => 'cancel',
        'name' => E::ts('Cancel'),
      ],
    ]);
    // Defaults
    // Export Contact - Checked
    // Export Format - CSV
    $this->setDefaults(['export_entities[contact]' => 1, 'export_format' => 1]);
    parent::buildQuickForm();
  }

  public function addRules() {
    $this->addFormRule(['CRM_Gdpr_Form_Export', 'validateExportSelection']);
  }

  /**
   * Validation export selections.
   */
  public static function validateExportSelection($values) {
    $errors = [];
    if (!isset($values['export_entities']) || empty($values['export_entities'])) {
      $errors['export_entities'] = E::ts('Select entities to export.');
    }
    return $errors;
  }

  public function postProcess() {

    if (!$this->_contactID) {
      CRM_Core_Error::fatal(E::ts("Something went wrong. Please contact Admin."));
    }

    $entities = self::getDataExportEntities();
    $extraFields = self::getExtraFieldsMapping();

    // final export data array
    $exportData = $pdfColumn = [];
    $values = $this->exportValues();
    foreach ($values['export_entities'] as $entity => $value) {

      // GDPR data
      // We will process this separately as GDPR data needs to be computed
      if ($entity == 'gdpr') {
        self::getGDPRData($this->_contactID, $values['export_format'], $exportData, $pdfColumn);
        continue;
      }

      // get exportable fields for core entity
      if (isset($entities[$entity]['bao']) && method_exists($entities[$entity]['bao'], $entities[$entity]['method'])) {
        $exportableFields = call_user_func($entities[$entity]['bao'].'::'.$entities[$entity]['method']);
      }

      // get exportable fields for custom group
      if (isset($entities[$entity]['is_custom_group']) && isset($entities[$entity]['custom_group_id'])) {
        $exportableFields = self::exportableCustomFieldsForGroup($entities[$entity]['custom_group_id']);
      }

      // Do not continue if we do know the fields to export
      if (empty($exportableFields)) {
        continue;
      }

      // We need to change certain field names for case record
      // to fetch correct data via API
      if ($entity == 'case') {
        $exportableFields['case_type']['name'] = 'case_type_id';
        $exportableFields['case_status']['name'] = 'status_id';
      }

      // Prepare header and fields for which values to be fetched
      $entityHeader = $entityFields = $emptyRow = [];
      $entityFields = $customDataColumns = [];
      foreach ($exportableFields as $field_key => $field_value) {
        // Check if we have a name for exportable field
        if (empty($field_value['name'])) {
          continue;
        }

        // Event title should event_title not title
        //if ($entity == 'participant' && $field_value['name'] == 'title') {
        //  $field_value['name'] = 'event_title';
        //}

        $entityHeader[$field_value['name']] = $field_value['title'];
        $entityFields[] = $field_value['name'];
        if (isset($field_value['column_name'])) {
          $customDataColumns[$field_value['name']] = $field_value['column_name'];
        }
        $emptyRow[$field_value['name']] = '';
        // Get values for pseudoconstant, if available
        if (isset($field_value['pseudoconstant'])) {
          $entities[$entity]['pseudoconstant'][$field_value['name']] = $entities[$entity]['bao']::buildOptions($field_value['name']);
        }

        // pseudoconstant option only available for core fields
        // we need to prepare the option values for custom fields
        if (substr($field_value['name'], 0, 7) == 'custom_') {
          // Get custom field details
          $cfResult = CRM_Gdpr_Utils::CiviCRMAPIWrapper('CustomField', 'get', [
            'sequential' => 1,
            'id' => $field_value['custom_field_id'],
          ]);
          $cfDetails = $cfResult['values'][0];

          // Check if this custom field with select/checkbox/radio, etc
          if (isset($cfDetails['option_group_id']) && !empty($cfDetails['option_group_id'])) {
            $entities[$entity]['pseudoconstant'][$field_value['name']] = self::getOptionValuesArray($cfDetails['option_group_id']);
          }
        }

        if ($entity == 'case') {
          $entities[$entity]['pseudoconstant']['case_type_id'] = CRM_Case_PseudoConstant::caseType();
          $entities[$entity]['pseudoconstant']['status_id'] = CRM_Case_PseudoConstant::caseStatus();
        }
      }

      // Clean fields array
      $entityFields = array_filter($entityFields);

      // Get Custom Data via SQL
      if (isset($entities[$entity]['is_custom_group']) && isset($entities[$entity]['custom_group_id'])) {
        $customDataColumns = array_filter($customDataColumns);
        $entityData = self::getCustomDataForEntity($entities[$entity], $this->_contactID, $entityFields, $customDataColumns);
      } else { // Get Core Data via API
        // Get data for the entity via API
        $entityParams = [
          'sequential' => 1,
          $entities[$entity]['search_field'] => $this->_contactID,
          'return' => $entityFields,
          'options' => ['limit' => 0],
        ];

        // Add 'is_test' flag for entities which support test records
        if ($entities[$entity]['exclude_test']) {
          $entityParams['is_test'] = 0;
        }

        // Skip sending return parameter to Case API, as it returns only limited information
        if ($entity == 'case') {
          unset($entityParams['return']);
        }
        $entityData = CRM_Gdpr_Utils::CiviCRMAPIWrapper($entity, 'get', $entityParams);
      }

      // Add header only if we have data for the entity
      if (!empty($entityData['values'])) {
        // If PDF, we flag this header/column name array, so that we can print as table data
        if ($values['export_format'] == 2) {
          $pdfColumn[$entity] = $entityHeader;
        }
        // If CSV, we have entity/header/column name as row
        else {
          // Add entity/header/labels to final export data array
          $exportData[$entity][] = [$entities[$entity]['label']];
          $exportData[$entity][] = $entityHeader;
        }
      }

      foreach ($entityData['values'] as $row_key => $row_value) {
        $exportRow = $emptyRow;

        // Replace tokens for contact, ex: postal greeting, addressee
        if ($entity == 'contact') {
          $row_value['email_greeting'] = $row_value['email_greeting_display'];
          $row_value['postal_greeting'] = $row_value['postal_greeting_display'];
          $row_value['addressee'] = $row_value['addressee_display'];
        }

        // We need to get event details, for participant entity
        if ($entity == 'participant' && !empty($row_value['event_id'])) {
          $eventData = CRM_Gdpr_Utils::CiviCRMAPIWrapper('Event', 'getsingle', [
            'id' => $row_value['event_id'],
          ]);
          $participantFields = [
            'title',
            'start_date',
            'end_date',
          ];
          foreach ($participantFields as $participantField) {
            if (isset($eventData[$participantField])) {
              $row_value[$participantField] = $eventData[$participantField];
            }
          }
        }

        // Add 'contact_id_a_name' for relationship data
        if ($entity == 'relationship') {
          if ($row_value['contact_id_a'] == $this->_contactID) {
            $row_value['contact_id_a'] = $this->_contactName;
            $row_value['contact_id_b'] = $row_value['display_name'];
          } else {
            $row_value['contact_id_a'] = $row_value['display_name'];
            $row_value['contact_id_b'] = $this->_contactName;
          }
        }

        // Ignore 'Bulk Email' activities
        if ($entity == 'activity') {
          $bulkActivityTypeID = CRM_Core_PseudoConstant::getKey('CRM_Activity_BAO_Activity', 'activity_type_id', 'Bulk Email');
          if ($row_value['activity_type_id'] == $bulkActivityTypeID) {
            continue;
          }
        }

        foreach ($row_value as $entity_field_key => $entity_field_value) {
          // Check if the values is for the expected exportable fields
          if (!isset($exportRow[$entity_field_key])) {
            continue;
          }

          // Check if this is pseudoconstant (select/checkbox/radio, etc)
          if (isset($entities[$entity]['pseudoconstant'][$entity_field_key])) {

            $options = $entities[$entity]['pseudoconstant'][$entity_field_key];

            // Multi-value select/checkbox/radio, etc
            if (is_array($entity_field_value)) {
              $multivalue_data = [];
              foreach ($entity_field_value as $multivalue) {
                if (isset($options[$multivalue])) {
                  $multivalue_data[] = $options[$multivalue];
                }
              }
              $exportRow[$entity_field_key] = @implode(CRM_Core_DAO::VALUE_SEPARATOR, $multivalue_data);
            } else if (isset($options[$entity_field_value])){
              // Single value select/checkbox/radio, etc
              //$exportRow[$entity_field_key] = $options[$entity_field_value];
              if ($values['export_format'] == 1) {
                $exportRow[$entity_field_key] = '"'.$options[$entity_field_value].'"';
              }
              else {
                $exportRow[$entity_field_key] = $options[$entity_field_value];
              }
            }
          } else {
            // This is text field or similar
            // FIXME: If this is array, convert it into string
            if (is_array($entity_field_value)) {
              $entity_field_value = @implode(',', $entity_field_value);
            }
            // Strip html tags if found in any strings
            $entity_field_value = strip_tags($entity_field_value);
            // Enclose text string in quotes if CSV
            // as data with comma will break CSV format
            if ($values['export_format'] == 1) {
              $exportRow[$entity_field_key] = '"'.$entity_field_value.'"';
            }
            else {
              $exportRow[$entity_field_key] = $entity_field_value;
            }
          }

          // Check if we need to update other fields in the expected exportable data
          // Example: activity_type is returned as exportable field by core BAO
          // But API does not return any value activity_type
          // API returns data for activity_type_id
          if (isset($extraFields[$entity]) && !empty($extraFields[$entity])) {
            foreach ($extraFields[$entity] as $efKey => $efSwitch) {
              $exportRow[$efKey] = $exportRow[$efSwitch];
            }
          }
        }

        // Add row to final data export array
        $exportData[$entity][] = $exportRow;
      }
    }

    // Show message, if no data to print
    if (empty($exportData)) {
      $url = CRM_Utils_System::url('civicrm/gdpr/export', 'reset=1&cid='.$this->_contactID);
      CRM_Core_Session::setStatus(E::ts('No data available to export.'), E::ts('Export Data'), 'warning');
      CRM_Utils_System::redirect($url);
      CRM_Utils_System::civiExit();
    }

    // Export data to file, default to PDF for data protection
    //print_r ($exportData);exit;
    $contactName = CRM_Utils_String::munge($this->_contactName, '_', 64);
    switch ($values['export_format']) {
      case 1: // CSV
        self::export2csv($exportData, $contactName);
        break;

      case 2: // PDF
      default:
        self::export2pdf($exportData, $contactName, $pdfColumn);
        break;
    }

    return;
  }

  /**
   *
   * Get Option Values for a given Option Group ID
   *
   * @param int $optionGroupID
   *
   * @return array
   */
  private function getOptionValuesArray($optionGroupID) {
    $dao = new CRM_Core_DAO_OptionValue();
    $dao->option_group_id = $optionGroupID;
    $dao->find();

    $optionValues = [];
    while ($dao->fetch()) {
      $optionValues[$dao->value] = $dao->label;
    }

    return $optionValues;
  }

  /**
   *
   * Make CSV file content and return as string.
   *
   * @param array $rows
   *
   * @return string
   */
  private function makeCsv($data) {
    $config = CRM_Core_Config::singleton();
    $csv = '';
    // Add the data as rows
    foreach ($data as $entity => $rows) {
      foreach ($rows as $row) {
        $csv .= implode($config->fieldSeparator,
          $row
        ) . "\r\n";
      }
    }

    return $csv;
  }

  /**
   * Export to CSV
   * @param $rows
   */
  private function export2csv(&$rows, $name) {
    //Mark as a CSV file.
    CRM_Utils_System::setHttpHeader('Content-Type', 'text/csv');

    //Force a download and name the file using the current timestamp.
    $datetime = date('Ymd-Gi', $_SERVER['REQUEST_TIME']);
    CRM_Utils_System::setHttpHeader('Content-Disposition', 'attachment; filename=Export_Data_' . $name . '_' . $datetime . '.csv');
    echo self::makeCsv($rows);
    CRM_Utils_System::civiExit();
  }

  /**
   * Export to PDF
   * @param $rows
   */
  private function export2pdf(&$data, $name, $pdfColumn) {

    // As we cannot fit rows in pdf file
    // we print as tables, i.e each row is a table
    // with header/column name in first table column
    // data in second table column
    $html = [];
    // Get entity names
    $entities = self::getDataExportEntities();

    foreach ($data as $entity => $rows) {
      // Display entity name as section
      $entityName = $entities[$entity]['label'];
      $html[] = "<table width='100%' border='1' cellspacing='0' cellpadding='5' style='table-layout:fixed;'>";
      $html[] = "<tr><td colspan='2' style='background-color: #bcbcbc;'><b>{$entityName}</b></td>";
      //$html[] = "</table>";

      // Prepare header/column name to display in first column
      $columnNames = $pdfColumn[$entity];
      //$html[] = "<table width='100%' border='1' cellspacing='0' cellpadding='5'>";
      foreach ($rows as $row) {
        foreach ($row as $key => $data) {
          if (trim($data) == '') {
            continue;
          }
          $columnName = $columnNames[$key];
          $html[] = "<tr>";
          $html[] = "<td width='25%' style='background-color: #bcbcbc;'><b>{$columnName}</b></td><td width='75%'>{$data}</td>";
          $html[] = "</tr>";
        }
      }
      $html[] = "</table>";
      $html[] = "<br />";
    }

    $htmlStr = @implode('', $html);

    // get default PDF format
    $defaultFormat = CRM_Core_BAO_PdfFormat::getDefaultValues();

    //Force a download and name the file using the current timestamp.
    $datetime = date('Ymd-Gi', $_SERVER['REQUEST_TIME']);
    $fileName = 'Export_Data_' . $name . '_' . $datetime . '.pdf';
    CRM_Utils_PDF_Utils::html2pdf($htmlStr, $fileName, FALSE, $defaultFormat);
    CRM_Utils_System::civiExit();
  }

  /**
   * Get entities for data export.
   * @return array
   */
  public static function getDataExportEntities() {
    // Core component
    $exportOptions = [
      'contact' => [
        'label' => E::ts('Contact'),
        'bao' => 'CRM_Contact_BAO_Contact',
        'method' => 'exportableFields',
        'search_field' => 'id',
        'exclude_test' => 0,
      ],
      'activity' => [
        'label' => E::ts('Activity'),
        'bao' => 'CRM_Activity_BAO_Activity',
        'method' => 'exportableFields',
        'search_field' => 'target_contact_id',
        'exclude_test' => 1,
      ],
      'relationship' => [
        'label' => E::ts('Relationship'),
        'bao' => 'CRM_Contact_DAO_Relationship',
        'method' => 'fields',
        'search_field' => 'contact_id',
        'exclude_test' => 0,
      ],
    ];

    // Check if other components are enabled and include them in export options
    $compInfo = CRM_Core_Component::getEnabledComponents();
    if (isset($compInfo['CiviContribute'])) {
      $exportOptions['contribution'] = [
        'label' => E::ts('Contribution'),
        'bao' => 'CRM_Contribute_BAO_Contribution',
        'method' => 'exportableFields',
        'search_field' => 'contact_id',
        'exclude_test' => 1,
      ];
    }

    if (isset($compInfo['CiviMember'])) {
      $exportOptions['membership'] = [
        'label' => E::ts('Membership'),
        'bao' => 'CRM_Member_BAO_Membership',
        'method' => 'exportableFields',
        'search_field' => 'contact_id',
        'exclude_test' => 1,
      ];
    }

    if (isset($compInfo['CiviEvent'])) {
      $exportOptions['participant'] = [
        'label' => E::ts('Participant'),
        'bao' => 'CRM_Event_BAO_Participant',
        'method' => 'exportableFields',
        'search_field' => 'contact_id',
        'exclude_test' => 1,
      ];
    }

    if (isset($compInfo['CiviCase'])) {
      $exportOptions['case'] = [
        'label' => E::ts('Case'),
        'bao' => 'CRM_Case_BAO_Case',
        'method' => 'exportableFields',
        'search_field' => 'contact_id',
        'exclude_test' => 0,
      ];
    }

    // Check if GA extension is installed
    if(CRM_Extension_System::singleton()->getMapper()->isActiveModule('civigiftaid')) {
      // Get custom group id
      $CGResult = civicrm_api3('CustomGroup', 'getsingle', [
        'sequential' => 1,
        'name' => "Gift_Aid_Declaration",
      ]);
      $exportOptions['civigiftaid'] = [
        'label' => E::ts('Gift Aid Declaration'),
        'is_custom_group' => 1,
        'custom_group_id' => $CGResult['id'],
        'entity' => 'Contact',
        'search_field' => 'id',
        'exclude_test' => 0,
        'table_name' => $CGResult['table_name'],
      ];
    }

    // GDPR data
    $exportOptions['gdpr'] = [
      'label' => E::ts('GDPR'),
      'is_custom_group' => 1,
      'custom_group_id' => '',
      'entity' => 'Contact',
      'search_field' => 'id',
      'exclude_test' => 0,
      'table_name' => '',
    ];

    return $exportOptions;
  }

  /**
   * Get exportable fields for custom group
   * assuming all fields are exportable in custom groups
   *
   * @return array
   */
  public static function exportableCustomFieldsForGroup($id) {
    $result = civicrm_api3('CustomField', 'get', [
      'sequential' => 1,
      'custom_group_id' => $id,
    ]);

    $fields = [];
    foreach ($result['values'] as $key => $value) {
      $field = [
        'custom_field_id' => $value['id'],
        'name' => 'custom_'.$value['id'],
        'column_name' => $value['column_name'],
        'title' => $value['label'],
      ];
      if (isset($value['option_group_id']) && !empty($value['option_group_id'])) {
        $field['option_group_id'] = $value['option_group_id'];
      }
      $fields[] = $field;
    }

    return $fields;
  }

  /**
   * Get custom data for any entity
   * Using SQL to get custom data
   * as API does not return multi-row custom daya
   *
   * @return array
   */
  public static function getCustomDataForEntity($entity, $entityId, $entityFields, $columns) {
    if (empty($entity) || empty($entityId)) {
      return;
    }

    $selectColumns = implode(',', array_values($columns));

    // Using SQL to get custom data
    // as multi-row custom data are not returned via API
    $tableName = $entity['table_name'];
    $sql = "SELECT {$selectColumns} FROM {$tableName} WHERE entity_id = %1";
    $params = array(1 => array($entityId, 'Integer'));
    $dao = CRM_Core_DAO::executeQuery($sql, $params);
    $customData = [];
    while($dao->fetch()) {
      $customRow = [];
      foreach ($columns as $key => $value) {
        $customRow[$key] = $dao->{$value};
      }
      $customData[] = $customRow;
    }

    return['values' => $customData];
  }

  /**
   * GDPR data for export
   * We will process this separately as GDPR data needs to be computed
   *
   * @return array
   */
  public static function getGDPRData($contactId, $exportFormat, &$exportData, &$pdfColumn) {
    if (empty($contactId)) {
      return;
    }

    if ($exportFormat == 2) {
      $pdfColumn['gdpr'] = ['title' => 'Subject', 'details' => 'Status', 'date' => 'Date'];
    } else {
      $exportData['gdpr'][] = ['GDPR'];
      $exportData['gdpr'][] = ['title' => 'Subject', 'details' => 'Status', 'date' => 'Date'];
    }

    //$gdprRows[] = ['Subject', 'Status', 'Date'];
    $gdpr = new CRM_Gdpr_Page_Tab();
    $exportData['gdpr'][] = $gdpr->getCommunicationsPreferencesDetails($contactId);
    $exportData['gdpr'][] = $gdpr->getDataPolicyDetails($contactId);
    $groupSubscriptions = CRM_Gdpr_Utils::getallGroupSubscription($contactId);
    if (!empty($groupSubscriptions)) {
      $exportData['gdpr'][] = ['Group Subscription Log'];
      if ($exportFormat == 1) {
        $exportData['gdpr'][] = ['Group Title', 'Status', 'Date'];
      }

      foreach ($groupSubscriptions as $key => $value) {
        $exportData['gdpr'][] = ['title' => $value['title'], 'details' => $value['status'], 'date' => $value['date']];
      }
    }
  }

  /**
   * Get field key switch mapping so we can get correct data from API results
   *
   * Example: activity_type is returned as exportable field by core BAO
   * But API does not return any value activity_type
   * API returns data for activity_type_id
   *
   * @return array
   */
  public static function getExtraFieldsMapping() {
    $extraFieldsMapping = [
      'activity' => [
        'activity_type' => 'activity_type_id',
        'activity_status' => 'status_id',
      ],
    ];

    return $extraFieldsMapping;
  }
}
