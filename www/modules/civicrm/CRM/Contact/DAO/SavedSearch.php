<?php

/**
 * @package CRM
 * @copyright CiviCRM LLC https://civicrm.org/licensing
 *
 * Generated from xml/schema/CRM/Contact/SavedSearch.xml
 * DO NOT EDIT.  Generated by CRM_Core_CodeGen
 * (GenCodeChecksum:bca384578ea59c0cf4f0a80617c788fe)
 */

/**
 * Database access object for the SavedSearch entity.
 */
class CRM_Contact_DAO_SavedSearch extends CRM_Core_DAO {
  const EXT = 'civicrm';
  const TABLE_ADDED = '1.1';

  /**
   * Static instance to hold the table name.
   *
   * @var string
   */
  public static $_tableName = 'civicrm_saved_search';

  /**
   * Should CiviCRM log any modifications to this table in the civicrm_log table.
   *
   * @var bool
   */
  public static $_log = FALSE;

  /**
   * Saved Search ID
   *
   * @var int
   */
  public $id;

  /**
   * Unique name of saved search
   *
   * @var string
   */
  public $name;

  /**
   * Administrative label for search
   *
   * @var string
   */
  public $label;

  /**
   * Submitted form values for this search
   *
   * @var text
   */
  public $form_values;

  /**
   * Foreign key to civicrm_mapping used for saved search-builder searches.
   *
   * @var int
   */
  public $mapping_id;

  /**
   * Foreign key to civicrm_option value table used for saved custom searches.
   *
   * @var int
   */
  public $search_custom_id;

  /**
   * Entity name for API based search
   *
   * @var string
   */
  public $api_entity;

  /**
   * Parameters for API based search
   *
   * @var text
   */
  public $api_params;

  /**
   * FK to contact table.
   *
   * @var int
   */
  public $created_id;

  /**
   * FK to contact table.
   *
   * @var int
   */
  public $modified_id;

  /**
   * Optional date after which the search is not needed
   *
   * @var timestamp
   */
  public $expires_date;

  /**
   * When the search was created.
   *
   * @var timestamp
   */
  public $created_date;

  /**
   * When the search was last modified.
   *
   * @var timestamp
   */
  public $modified_date;

  /**
   * @var text
   */
  public $description;

  /**
   * Class constructor.
   */
  public function __construct() {
    $this->__table = 'civicrm_saved_search';
    parent::__construct();
  }

  /**
   * Returns localized title of this entity.
   *
   * @param bool $plural
   *   Whether to return the plural version of the title.
   */
  public static function getEntityTitle($plural = FALSE) {
    return $plural ? ts('Saved Searches') : ts('Saved Search');
  }

  /**
   * Returns foreign keys and entity references.
   *
   * @return array
   *   [CRM_Core_Reference_Interface]
   */
  public static function getReferenceColumns() {
    if (!isset(Civi::$statics[__CLASS__]['links'])) {
      Civi::$statics[__CLASS__]['links'] = static::createReferenceColumns(__CLASS__);
      Civi::$statics[__CLASS__]['links'][] = new CRM_Core_Reference_Basic(self::getTableName(), 'mapping_id', 'civicrm_mapping', 'id');
      Civi::$statics[__CLASS__]['links'][] = new CRM_Core_Reference_Basic(self::getTableName(), 'created_id', 'civicrm_contact', 'id');
      Civi::$statics[__CLASS__]['links'][] = new CRM_Core_Reference_Basic(self::getTableName(), 'modified_id', 'civicrm_contact', 'id');
      CRM_Core_DAO_AllCoreTables::invoke(__CLASS__, 'links_callback', Civi::$statics[__CLASS__]['links']);
    }
    return Civi::$statics[__CLASS__]['links'];
  }

  /**
   * Returns all the column names of this table
   *
   * @return array
   */
  public static function &fields() {
    if (!isset(Civi::$statics[__CLASS__]['fields'])) {
      Civi::$statics[__CLASS__]['fields'] = [
        'id' => [
          'name' => 'id',
          'type' => CRM_Utils_Type::T_INT,
          'title' => ts('Saved Search ID'),
          'description' => ts('Saved Search ID'),
          'required' => TRUE,
          'where' => 'civicrm_saved_search.id',
          'table_name' => 'civicrm_saved_search',
          'entity' => 'SavedSearch',
          'bao' => 'CRM_Contact_BAO_SavedSearch',
          'localizable' => 0,
          'html' => [
            'type' => 'Number',
          ],
          'readonly' => TRUE,
          'add' => '1.1',
        ],
        'name' => [
          'name' => 'name',
          'type' => CRM_Utils_Type::T_STRING,
          'title' => ts('Saved Search Name'),
          'description' => ts('Unique name of saved search'),
          'maxlength' => 255,
          'size' => CRM_Utils_Type::HUGE,
          'where' => 'civicrm_saved_search.name',
          'default' => NULL,
          'table_name' => 'civicrm_saved_search',
          'entity' => 'SavedSearch',
          'bao' => 'CRM_Contact_BAO_SavedSearch',
          'localizable' => 0,
          'html' => [
            'type' => 'Text',
          ],
          'add' => '1.0',
        ],
        'label' => [
          'name' => 'label',
          'type' => CRM_Utils_Type::T_STRING,
          'title' => ts('Saved Search Label'),
          'description' => ts('Administrative label for search'),
          'maxlength' => 255,
          'size' => CRM_Utils_Type::HUGE,
          'where' => 'civicrm_saved_search.label',
          'default' => NULL,
          'table_name' => 'civicrm_saved_search',
          'entity' => 'SavedSearch',
          'bao' => 'CRM_Contact_BAO_SavedSearch',
          'localizable' => 0,
          'html' => [
            'type' => 'Text',
            'label' => ts("Label"),
          ],
          'add' => '5.32',
        ],
        'form_values' => [
          'name' => 'form_values',
          'type' => CRM_Utils_Type::T_TEXT,
          'title' => ts('Submitted Form Values'),
          'description' => ts('Submitted form values for this search'),
          'import' => TRUE,
          'where' => 'civicrm_saved_search.form_values',
          'export' => TRUE,
          'table_name' => 'civicrm_saved_search',
          'entity' => 'SavedSearch',
          'bao' => 'CRM_Contact_BAO_SavedSearch',
          'localizable' => 0,
          'serialize' => self::SERIALIZE_PHP,
          'add' => '1.1',
        ],
        'mapping_id' => [
          'name' => 'mapping_id',
          'type' => CRM_Utils_Type::T_INT,
          'title' => ts('Mapping ID'),
          'description' => ts('Foreign key to civicrm_mapping used for saved search-builder searches.'),
          'where' => 'civicrm_saved_search.mapping_id',
          'table_name' => 'civicrm_saved_search',
          'entity' => 'SavedSearch',
          'bao' => 'CRM_Contact_BAO_SavedSearch',
          'localizable' => 0,
          'FKClassName' => 'CRM_Core_DAO_Mapping',
          'html' => [
            'label' => ts("Mapping"),
          ],
          'add' => '1.5',
        ],
        'search_custom_id' => [
          'name' => 'search_custom_id',
          'type' => CRM_Utils_Type::T_INT,
          'title' => ts('Option Value ID'),
          'description' => ts('Foreign key to civicrm_option value table used for saved custom searches.'),
          'where' => 'civicrm_saved_search.search_custom_id',
          'table_name' => 'civicrm_saved_search',
          'entity' => 'SavedSearch',
          'bao' => 'CRM_Contact_BAO_SavedSearch',
          'localizable' => 0,
          'add' => '2.0',
        ],
        'api_entity' => [
          'name' => 'api_entity',
          'type' => CRM_Utils_Type::T_STRING,
          'title' => ts('Entity Name'),
          'description' => ts('Entity name for API based search'),
          'maxlength' => 255,
          'size' => CRM_Utils_Type::HUGE,
          'where' => 'civicrm_saved_search.api_entity',
          'table_name' => 'civicrm_saved_search',
          'entity' => 'SavedSearch',
          'bao' => 'CRM_Contact_BAO_SavedSearch',
          'localizable' => 0,
          'pseudoconstant' => [
            'callback' => 'CRM_Contact_BAO_SavedSearch::getApiEntityOptions',
          ],
          'add' => '5.24',
        ],
        'api_params' => [
          'name' => 'api_params',
          'type' => CRM_Utils_Type::T_TEXT,
          'title' => ts('API Parameters'),
          'description' => ts('Parameters for API based search'),
          'where' => 'civicrm_saved_search.api_params',
          'table_name' => 'civicrm_saved_search',
          'entity' => 'SavedSearch',
          'bao' => 'CRM_Contact_BAO_SavedSearch',
          'localizable' => 0,
          'serialize' => self::SERIALIZE_JSON,
          'add' => '5.24',
        ],
        'created_id' => [
          'name' => 'created_id',
          'type' => CRM_Utils_Type::T_INT,
          'title' => ts('Created By Contact ID'),
          'description' => ts('FK to contact table.'),
          'where' => 'civicrm_saved_search.created_id',
          'table_name' => 'civicrm_saved_search',
          'entity' => 'SavedSearch',
          'bao' => 'CRM_Contact_BAO_SavedSearch',
          'localizable' => 0,
          'FKClassName' => 'CRM_Contact_DAO_Contact',
          'html' => [
            'label' => ts("Created By"),
          ],
          'readonly' => TRUE,
          'add' => '5.36',
        ],
        'modified_id' => [
          'name' => 'modified_id',
          'type' => CRM_Utils_Type::T_INT,
          'title' => ts('Modified By Contact ID'),
          'description' => ts('FK to contact table.'),
          'where' => 'civicrm_saved_search.modified_id',
          'table_name' => 'civicrm_saved_search',
          'entity' => 'SavedSearch',
          'bao' => 'CRM_Contact_BAO_SavedSearch',
          'localizable' => 0,
          'FKClassName' => 'CRM_Contact_DAO_Contact',
          'html' => [
            'label' => ts("Modified By"),
          ],
          'readonly' => TRUE,
          'add' => '5.36',
        ],
        'expires_date' => [
          'name' => 'expires_date',
          'type' => CRM_Utils_Type::T_TIMESTAMP,
          'title' => ts('Search Expiry Date'),
          'description' => ts('Optional date after which the search is not needed'),
          'required' => FALSE,
          'where' => 'civicrm_saved_search.expires_date',
          'table_name' => 'civicrm_saved_search',
          'entity' => 'SavedSearch',
          'bao' => 'CRM_Contact_BAO_SavedSearch',
          'localizable' => 0,
          'add' => '5.36',
        ],
        'created_date' => [
          'name' => 'created_date',
          'type' => CRM_Utils_Type::T_TIMESTAMP,
          'title' => ts('Created Date'),
          'description' => ts('When the search was created.'),
          'required' => TRUE,
          'where' => 'civicrm_saved_search.created_date',
          'default' => 'CURRENT_TIMESTAMP',
          'table_name' => 'civicrm_saved_search',
          'entity' => 'SavedSearch',
          'bao' => 'CRM_Contact_BAO_SavedSearch',
          'localizable' => 0,
          'readonly' => TRUE,
          'add' => '5.36',
        ],
        'modified_date' => [
          'name' => 'modified_date',
          'type' => CRM_Utils_Type::T_TIMESTAMP,
          'title' => ts('Modified Date'),
          'description' => ts('When the search was last modified.'),
          'required' => TRUE,
          'where' => 'civicrm_saved_search.modified_date',
          'default' => 'CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP',
          'table_name' => 'civicrm_saved_search',
          'entity' => 'SavedSearch',
          'bao' => 'CRM_Contact_BAO_SavedSearch',
          'localizable' => 0,
          'readonly' => TRUE,
          'add' => '5.36',
        ],
        'description' => [
          'name' => 'description',
          'type' => CRM_Utils_Type::T_TEXT,
          'title' => ts('Saved Search Description'),
          'rows' => 2,
          'cols' => 60,
          'where' => 'civicrm_saved_search.description',
          'table_name' => 'civicrm_saved_search',
          'entity' => 'SavedSearch',
          'bao' => 'CRM_Contact_BAO_SavedSearch',
          'localizable' => 0,
          'html' => [
            'type' => 'TextArea',
          ],
          'add' => '5.36',
        ],
      ];
      CRM_Core_DAO_AllCoreTables::invoke(__CLASS__, 'fields_callback', Civi::$statics[__CLASS__]['fields']);
    }
    return Civi::$statics[__CLASS__]['fields'];
  }

  /**
   * Return a mapping from field-name to the corresponding key (as used in fields()).
   *
   * @return array
   *   Array(string $name => string $uniqueName).
   */
  public static function &fieldKeys() {
    if (!isset(Civi::$statics[__CLASS__]['fieldKeys'])) {
      Civi::$statics[__CLASS__]['fieldKeys'] = array_flip(CRM_Utils_Array::collect('name', self::fields()));
    }
    return Civi::$statics[__CLASS__]['fieldKeys'];
  }

  /**
   * Returns the names of this table
   *
   * @return string
   */
  public static function getTableName() {
    return self::$_tableName;
  }

  /**
   * Returns if this table needs to be logged
   *
   * @return bool
   */
  public function getLog() {
    return self::$_log;
  }

  /**
   * Returns the list of fields that can be imported
   *
   * @param bool $prefix
   *
   * @return array
   */
  public static function &import($prefix = FALSE) {
    $r = CRM_Core_DAO_AllCoreTables::getImports(__CLASS__, 'saved_search', $prefix, []);
    return $r;
  }

  /**
   * Returns the list of fields that can be exported
   *
   * @param bool $prefix
   *
   * @return array
   */
  public static function &export($prefix = FALSE) {
    $r = CRM_Core_DAO_AllCoreTables::getExports(__CLASS__, 'saved_search', $prefix, []);
    return $r;
  }

  /**
   * Returns the list of indices
   *
   * @param bool $localize
   *
   * @return array
   */
  public static function indices($localize = TRUE) {
    $indices = [
      'UI_name' => [
        'name' => 'UI_name',
        'field' => [
          0 => 'name',
        ],
        'localizable' => FALSE,
        'unique' => TRUE,
        'sig' => 'civicrm_saved_search::1::name',
      ],
    ];
    return ($localize && !empty($indices)) ? CRM_Core_DAO_AllCoreTables::multilingualize(__CLASS__, $indices) : $indices;
  }

}