<?php

/**
 * @package CRM
 * @copyright CiviCRM LLC https://civicrm.org/licensing
 *
 * Generated from xml/schema/CRM/Core/OptionGroup.xml
 * DO NOT EDIT.  Generated by CRM_Core_CodeGen
 * (GenCodeChecksum:5f7256e2bd9f6f3c96ea39c8642dcafb)
 */

/**
 * Database access object for the OptionGroup entity.
 */
class CRM_Core_DAO_OptionGroup extends CRM_Core_DAO {
  const EXT = 'civicrm';
  const TABLE_ADDED = '1.5';

  /**
   * Static instance to hold the table name.
   *
   * @var string
   */
  public static $_tableName = 'civicrm_option_group';

  /**
   * Should CiviCRM log any modifications to this table in the civicrm_log table.
   *
   * @var bool
   */
  public static $_log = TRUE;

  /**
   * Option Group ID
   *
   * @var int
   */
  public $id;

  /**
   * Option group name. Used as selection key by class properties which lookup options in civicrm_option_value.
   *
   * @var string
   */
  public $name;

  /**
   * Option Group title.
   *
   * @var string
   */
  public $title;

  /**
   * Option group description.
   *
   * @var string
   */
  public $description;

  /**
   * Option group description.
   *
   * @var string
   */
  public $data_type;

  /**
   * Is this a predefined system option group (i.e. it can not be deleted)?
   *
   * @var bool
   */
  public $is_reserved;

  /**
   * Is this option group active?
   *
   * @var bool
   */
  public $is_active;

  /**
   * A lock to remove the ability to add new options via the UI.
   *
   * @var bool
   */
  public $is_locked;

  /**
   * Class constructor.
   */
  public function __construct() {
    $this->__table = 'civicrm_option_group';
    parent::__construct();
  }

  /**
   * Returns localized title of this entity.
   *
   * @param bool $plural
   *   Whether to return the plural version of the title.
   */
  public static function getEntityTitle($plural = FALSE) {
    return $plural ? ts('Option Groups') : ts('Option Group');
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
          'title' => ts('Option Group ID'),
          'description' => ts('Option Group ID'),
          'required' => TRUE,
          'where' => 'civicrm_option_group.id',
          'table_name' => 'civicrm_option_group',
          'entity' => 'OptionGroup',
          'bao' => 'CRM_Core_BAO_OptionGroup',
          'localizable' => 0,
          'html' => [
            'type' => 'Number',
          ],
          'readonly' => TRUE,
          'add' => '1.5',
        ],
        'name' => [
          'name' => 'name',
          'type' => CRM_Utils_Type::T_STRING,
          'title' => ts('Option Group Name'),
          'description' => ts('Option group name. Used as selection key by class properties which lookup options in civicrm_option_value.'),
          'required' => TRUE,
          'maxlength' => 64,
          'size' => CRM_Utils_Type::BIG,
          'where' => 'civicrm_option_group.name',
          'table_name' => 'civicrm_option_group',
          'entity' => 'OptionGroup',
          'bao' => 'CRM_Core_BAO_OptionGroup',
          'localizable' => 0,
          'add' => '1.5',
        ],
        'title' => [
          'name' => 'title',
          'type' => CRM_Utils_Type::T_STRING,
          'title' => ts('Option Group title'),
          'description' => ts('Option Group title.'),
          'maxlength' => 255,
          'size' => CRM_Utils_Type::HUGE,
          'where' => 'civicrm_option_group.title',
          'table_name' => 'civicrm_option_group',
          'entity' => 'OptionGroup',
          'bao' => 'CRM_Core_BAO_OptionGroup',
          'localizable' => 1,
          'add' => '1.5',
        ],
        'description' => [
          'name' => 'description',
          'type' => CRM_Utils_Type::T_STRING,
          'title' => ts('Option Group Description'),
          'description' => ts('Option group description.'),
          'maxlength' => 255,
          'size' => CRM_Utils_Type::HUGE,
          'where' => 'civicrm_option_group.description',
          'table_name' => 'civicrm_option_group',
          'entity' => 'OptionGroup',
          'bao' => 'CRM_Core_BAO_OptionGroup',
          'localizable' => 1,
          'add' => '1.5',
        ],
        'data_type' => [
          'name' => 'data_type',
          'type' => CRM_Utils_Type::T_STRING,
          'title' => ts('Data Type for this option group'),
          'description' => ts('Option group description.'),
          'maxlength' => 128,
          'size' => CRM_Utils_Type::HUGE,
          'where' => 'civicrm_option_group.data_type',
          'table_name' => 'civicrm_option_group',
          'entity' => 'OptionGroup',
          'bao' => 'CRM_Core_BAO_OptionGroup',
          'localizable' => 0,
          'pseudoconstant' => [
            'callback' => 'CRM_Utils_Type::dataTypes',
          ],
          'add' => '4.7',
        ],
        'is_reserved' => [
          'name' => 'is_reserved',
          'type' => CRM_Utils_Type::T_BOOLEAN,
          'title' => ts('Option Group Is Reserved?'),
          'description' => ts('Is this a predefined system option group (i.e. it can not be deleted)?'),
          'required' => TRUE,
          'where' => 'civicrm_option_group.is_reserved',
          'default' => '1',
          'table_name' => 'civicrm_option_group',
          'entity' => 'OptionGroup',
          'bao' => 'CRM_Core_BAO_OptionGroup',
          'localizable' => 0,
          'add' => '1.5',
        ],
        'is_active' => [
          'name' => 'is_active',
          'type' => CRM_Utils_Type::T_BOOLEAN,
          'title' => ts('Option Group Is Active?'),
          'description' => ts('Is this option group active?'),
          'required' => TRUE,
          'where' => 'civicrm_option_group.is_active',
          'default' => '1',
          'table_name' => 'civicrm_option_group',
          'entity' => 'OptionGroup',
          'bao' => 'CRM_Core_BAO_OptionGroup',
          'localizable' => 0,
          'add' => '1.5',
        ],
        'is_locked' => [
          'name' => 'is_locked',
          'type' => CRM_Utils_Type::T_BOOLEAN,
          'title' => ts('Option Group Is Locked'),
          'description' => ts('A lock to remove the ability to add new options via the UI.'),
          'required' => TRUE,
          'where' => 'civicrm_option_group.is_locked',
          'default' => '0',
          'table_name' => 'civicrm_option_group',
          'entity' => 'OptionGroup',
          'bao' => 'CRM_Core_BAO_OptionGroup',
          'localizable' => 0,
          'add' => '4.5',
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
    return CRM_Core_DAO::getLocaleTableName(self::$_tableName);
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
    $r = CRM_Core_DAO_AllCoreTables::getImports(__CLASS__, 'option_group', $prefix, []);
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
    $r = CRM_Core_DAO_AllCoreTables::getExports(__CLASS__, 'option_group', $prefix, []);
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
        'sig' => 'civicrm_option_group::1::name',
      ],
    ];
    return ($localize && !empty($indices)) ? CRM_Core_DAO_AllCoreTables::multilingualize(__CLASS__, $indices) : $indices;
  }

}