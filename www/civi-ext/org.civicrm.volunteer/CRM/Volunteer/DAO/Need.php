<?php

/**
 * @package CRM
 * @copyright CiviCRM LLC https://civicrm.org/licensing
 *
 * Generated from org.civicrm.volunteer/xml/schema/CRM/Volunteer/Need.xml
 * DO NOT EDIT.  Generated by CRM_Core_CodeGen
 * (GenCodeChecksum:3262fa4d238d77b3b2bdcf016f69df86)
 */
use CRM_Volunteer_ExtensionUtil as E;

/**
 * Database access object for the Need entity.
 */
class CRM_Volunteer_DAO_Need extends CRM_Core_DAO {
  const EXT = E::LONG_NAME;
  const TABLE_ADDED = '4.4';

  /**
   * Static instance to hold the table name.
   *
   * @var string
   */
  public static $_tableName = 'civicrm_volunteer_need';

  /**
   * Should CiviCRM log any modifications to this table in the civicrm_log table.
   *
   * @var bool
   */
  public static $_log = TRUE;

  /**
   * Need Id
   *
   * @var int
   */
  public $id;

  /**
   * FK to civicrm_volunteer_project table which contains entity_table + entity for each volunteer project (initially civicrm_event + eventID).
   *
   * @var int
   */
  public $project_id;

  /**
   * @var datetime
   */
  public $start_time;

  /**
   * Used for specifying fuzzy dates, e.g., I have a need for 3 hours of volunteer work to be completed between 12/01/2015 and 12/31/2015.
   *
   * @var datetime
   */
  public $end_time;

  /**
   * Length in minutes of this volunteer time slot.
   *
   * @var int
   */
  public $duration;

  /**
   * Boolean indicating whether or not the time and role are flexible. Activities linked to a flexible need indicate that the volunteer is generally available.
   *
   * @var bool
   */
  public $is_flexible;

  /**
   * The number of volunteers needed for this need.
   *
   * @var int
   */
  public $quantity;

  /**
   *  Indicates whether this need is offered on public volunteer signup forms. Implicit FK to option_value row in visibility option_group.
   *
   * @var int
   */
  public $visibility_id;

  /**
   * The role associated with this need. Implicit FK to option_value row in volunteer_role option_group.
   *
   * @var int
   */
  public $role_id;

  /**
   * Is this need enabled?
   *
   * @var bool
   */
  public $is_active;

  /**
   * @var timestamp
   */
  public $created;

  /**
   * @var timestamp
   */
  public $last_updated;

  /**
   * Class constructor.
   */
  public function __construct() {
    $this->__table = 'civicrm_volunteer_need';
    parent::__construct();
  }

  /**
   * Returns localized title of this entity.
   *
   * @param bool $plural
   *   Whether to return the plural version of the title.
   */
  public static function getEntityTitle($plural = FALSE) {
    return $plural ? E::ts('Volunteer Needs') : E::ts('Volunteer Need');
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
      Civi::$statics[__CLASS__]['links'][] = new CRM_Core_Reference_Basic(self::getTableName(), 'project_id', 'civicrm_volunteer_project', 'id');
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
          'title' => E::ts('CiviVolunteer Need ID'),
          'description' => E::ts('Need Id'),
          'required' => TRUE,
          'where' => 'civicrm_volunteer_need.id',
          'table_name' => 'civicrm_volunteer_need',
          'entity' => 'Need',
          'bao' => 'CRM_Volunteer_DAO_Need',
          'localizable' => 0,
          'readonly' => TRUE,
          'add' => '4.4',
        ],
        'project_id' => [
          'name' => 'project_id',
          'type' => CRM_Utils_Type::T_INT,
          'description' => E::ts('FK to civicrm_volunteer_project table which contains entity_table + entity for each volunteer project (initially civicrm_event + eventID).'),
          'required' => FALSE,
          'where' => 'civicrm_volunteer_need.project_id',
          'table_name' => 'civicrm_volunteer_need',
          'entity' => 'Need',
          'bao' => 'CRM_Volunteer_DAO_Need',
          'localizable' => 0,
          'FKClassName' => 'CRM_Volunteer_DAO_Project',
          'add' => '4.4',
        ],
        'start_time' => [
          'name' => 'start_time',
          'type' => CRM_Utils_Type::T_DATE + CRM_Utils_Type::T_TIME,
          'title' => E::ts('Start Date and Time'),
          'where' => 'civicrm_volunteer_need.start_time',
          'table_name' => 'civicrm_volunteer_need',
          'entity' => 'Need',
          'bao' => 'CRM_Volunteer_DAO_Need',
          'localizable' => 0,
          'add' => '4.4',
        ],
        'end_time' => [
          'name' => 'end_time',
          'type' => CRM_Utils_Type::T_DATE + CRM_Utils_Type::T_TIME,
          'title' => E::ts('End Date and Time'),
          'description' => E::ts('Used for specifying fuzzy dates, e.g., I have a need for 3 hours of volunteer work to be completed between 12/01/2015 and 12/31/2015.'),
          'where' => 'civicrm_volunteer_need.end_time',
          'table_name' => 'civicrm_volunteer_need',
          'entity' => 'Need',
          'bao' => 'CRM_Volunteer_DAO_Need',
          'localizable' => 0,
          'add' => '4.4',
        ],
        'duration' => [
          'name' => 'duration',
          'type' => CRM_Utils_Type::T_INT,
          'title' => E::ts('Duration'),
          'description' => E::ts('Length in minutes of this volunteer time slot.'),
          'where' => 'civicrm_volunteer_need.duration',
          'table_name' => 'civicrm_volunteer_need',
          'entity' => 'Need',
          'bao' => 'CRM_Volunteer_DAO_Need',
          'localizable' => 0,
          'add' => '4.4',
        ],
        'is_flexible' => [
          'name' => 'is_flexible',
          'type' => CRM_Utils_Type::T_BOOLEAN,
          'title' => E::ts('Flexible'),
          'description' => E::ts('Boolean indicating whether or not the time and role are flexible. Activities linked to a flexible need indicate that the volunteer is generally available.'),
          'required' => TRUE,
          'where' => 'civicrm_volunteer_need.is_flexible',
          'default' => '0',
          'table_name' => 'civicrm_volunteer_need',
          'entity' => 'Need',
          'bao' => 'CRM_Volunteer_DAO_Need',
          'localizable' => 0,
          'add' => '4.4',
        ],
        'quantity' => [
          'name' => 'quantity',
          'type' => CRM_Utils_Type::T_INT,
          'title' => E::ts('Quantity'),
          'description' => E::ts('The number of volunteers needed for this need.'),
          'where' => 'civicrm_volunteer_need.quantity',
          'default' => 'NULL',
          'table_name' => 'civicrm_volunteer_need',
          'entity' => 'Need',
          'bao' => 'CRM_Volunteer_DAO_Need',
          'localizable' => 0,
          'add' => '4.4',
        ],
        'visibility_id' => [
          'name' => 'visibility_id',
          'type' => CRM_Utils_Type::T_INT,
          'title' => E::ts('Visibility'),
          'description' => E::ts(' Indicates whether this need is offered on public volunteer signup forms. Implicit FK to option_value row in visibility option_group.'),
          'where' => 'civicrm_volunteer_need.visibility_id',
          'default' => 'NULL',
          'table_name' => 'civicrm_volunteer_need',
          'entity' => 'Need',
          'bao' => 'CRM_Volunteer_DAO_Need',
          'localizable' => 0,
          'pseudoconstant' => [
            'optionGroupName' => 'visibility',
            'optionEditPath' => 'civicrm/admin/options/visibility',
          ],
          'add' => '4.4',
        ],
        'role_id' => [
          'name' => 'role_id',
          'type' => CRM_Utils_Type::T_INT,
          'title' => E::ts('Role'),
          'description' => E::ts('The role associated with this need. Implicit FK to option_value row in volunteer_role option_group.'),
          'where' => 'civicrm_volunteer_need.role_id',
          'default' => 'NULL',
          'table_name' => 'civicrm_volunteer_need',
          'entity' => 'Need',
          'bao' => 'CRM_Volunteer_DAO_Need',
          'localizable' => 0,
          'pseudoconstant' => [
            'optionGroupName' => 'volunteer_role',
            'optionEditPath' => 'civicrm/admin/options/volunteer_role',
          ],
          'add' => '4.4',
        ],
        'is_active' => [
          'name' => 'is_active',
          'type' => CRM_Utils_Type::T_BOOLEAN,
          'title' => E::ts('Enabled'),
          'description' => E::ts('Is this need enabled?'),
          'required' => TRUE,
          'where' => 'civicrm_volunteer_need.is_active',
          'default' => '1',
          'table_name' => 'civicrm_volunteer_need',
          'entity' => 'Need',
          'bao' => 'CRM_Volunteer_DAO_Need',
          'localizable' => 0,
          'add' => '4.4',
        ],
        'created' => [
          'name' => 'created',
          'type' => CRM_Utils_Type::T_TIMESTAMP,
          'title' => E::ts('Date of Creation'),
          'where' => 'civicrm_volunteer_need.created',
          'default' => 'CURRENT_TIMESTAMP',
          'table_name' => 'civicrm_volunteer_need',
          'entity' => 'Need',
          'bao' => 'CRM_Volunteer_DAO_Need',
          'localizable' => 0,
          'add' => NULL,
        ],
        'last_updated' => [
          'name' => 'last_updated',
          'type' => CRM_Utils_Type::T_TIMESTAMP,
          'title' => E::ts('Date of Last Update'),
          'where' => 'civicrm_volunteer_need.last_updated',
          'default' => 'CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP',
          'table_name' => 'civicrm_volunteer_need',
          'entity' => 'Need',
          'bao' => 'CRM_Volunteer_DAO_Need',
          'localizable' => 0,
          'add' => NULL,
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
    $r = CRM_Core_DAO_AllCoreTables::getImports(__CLASS__, 'volunteer_need', $prefix, []);
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
    $r = CRM_Core_DAO_AllCoreTables::getExports(__CLASS__, 'volunteer_need', $prefix, []);
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
    $indices = [];
    return ($localize && !empty($indices)) ? CRM_Core_DAO_AllCoreTables::multilingualize(__CLASS__, $indices) : $indices;
  }

}