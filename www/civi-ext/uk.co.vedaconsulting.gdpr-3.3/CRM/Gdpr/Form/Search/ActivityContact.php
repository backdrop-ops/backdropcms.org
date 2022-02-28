<?php

use CRM_Gdpr_ExtensionUtil as E;

/**
 * A custom contact search
 */
class CRM_Gdpr_Form_Search_ActivityContact extends CRM_Contact_Form_Search_Custom_Base implements CRM_Contact_Form_Search_Interface {
  function __construct(&$formValues) {
    parent::__construct($formValues);
  }

  /**
   * Prepare a set of search fields
   *
   * @param CRM_Core_Form $form modifiable
   * @return void
   */
  function buildForm(&$form) {
    CRM_Utils_System::setTitle(E::ts('GDPR - Contacts without Activities'));

    $form->add('text',
      'contact_name',
      E::ts('Contact Name'),
      TRUE
    );

    // Optionally define default search values
    $form->setDefaults([
      'contact_name' => '',
    ]);

    /**
     * if you are using the standard template, this array tells the template what elements
     * are part of the search criteria
     */
    $form->assign('elements', ['contact_name']);

    // Get GDPR settings
    $settings = CRM_Gdpr_Utils::getGDPRSettings();
    $form->assign('settings', $settings);

    // Get GDPR activity types
    $gdprActTypes = CRM_Gdpr_Utils::getGDPRActivityTypes();
    $gdprActTypesStr = implode(', ', $gdprActTypes);
    $form->assign('gdprActTypes', $gdprActTypesStr);
  }

  /**
   * Get a list of summary data points
   *
   * @return mixed; NULL or array with keys:
   *  - summary: string
   *  - total: numeric
   */
  function summary() {
    return NULL;
  }

  /**
   * Get a list of displayable columns
   *
   * @return array, keys are printable column headers and values are SQL column names
   */
  function &columns() {
    // return by reference
    $columns = [
      E::ts('Contact Id') => 'contact_id',
      E::ts('Contact Type') => 'contact_type',
      E::ts('Contact Name') => 'sort_name',
    ];
    return $columns;
  }

  /**
   * Construct a full SQL query which returns one page worth of results
   *
   * @param int $offset
   * @param int $rowcount
   * @param null $sort
   * @param bool $includeContactIDs
   * @param bool $justIDs
   * @return string, sql
   */
  function all($offset = 0, $rowcount = 0, $sort = NULL, $includeContactIDs = FALSE, $justIDs = FALSE) {
    // delegate to $this->sql(), $this->select(), $this->from(), $this->where(), etc.
    return $this->sql($this->select(), $offset, $rowcount, $sort, $includeContactIDs, NULL);
  }

  /**
   * Construct a SQL SELECT clause
   *
   * @return string, sql fragment with SELECT arguments
   */
  function select() {
    return "
      contact_a.id           as contact_id  ,
      contact_a.contact_type as contact_type,
      contact_a.sort_name    as sort_name
    ";
  }

  /**
   * Construct a SQL FROM clause
   *
   * @return string, sql fragment with FROM and JOIN clauses
   */
  function from() {
    return "
      FROM civicrm_contact contact_a
    ";
  }

  /**
   * Construct a SQL WHERE clause
   *
   * @param bool $includeContactIDs
   * @return string, sql fragment with conditional expressions
   */
  function where($includeContactIDs = FALSE) {
    $params = $gdprParams = [];

    $count  = 1;
    $clause = [];
    $where = "(1)";
    $name   = CRM_Utils_Array::value('contact_name',
      $this->_formValues
    );
    if (!empty($name)) {
      $gdprParams = ['contact_name' => $name];
    }
    $clause[] = CRM_Gdpr_Utils::getActivityContactSQL($gdprParams, FALSE, TRUE, TRUE);

    if (!empty($clause)) {
      $where .= ' AND ' . implode(' AND ', $clause);
    }

    return $this->whereClause($where, $params);
  }

  /**
   * Determine the Smarty template for the search screen
   *
   * @return string, template path (findable through Smarty template path)
   */
  function templateFile() {
    return 'CRM/Gdpr/Form/Search/ActivityContact.tpl';
  }

  /**
   * @param int $offset
   * @param int $rowcount
   * @param null $sort
   * @param bool $returnSQL Not used; included for consistency with parent; SQL is always returned
   *
   * @return string
   */
  public function contactIDs($offset = 0, $rowcount = 0, $sort = NULL, $returnSQL = TRUE) {
    return $this->all($offset, $rowcount, $sort, FALSE, TRUE);
  }

  /*
   * Functions below generally don't need to be modified
   */

  /**
   * @inheritDoc
   */
  public function count() {
    $sql = $this->all();

    $dao = CRM_Core_DAO::executeQuery($sql,
      CRM_Core_DAO::$_nullArray
    );
    return $dao->N;
  }
}
