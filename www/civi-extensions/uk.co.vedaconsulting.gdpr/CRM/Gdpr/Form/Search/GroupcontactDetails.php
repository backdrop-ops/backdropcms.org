<?php

use CRM_Gdpr_ExtensionUtil as E;

/**
 * A custom contact search
 */
class CRM_Gdpr_Form_Search_GroupcontactDetails extends CRM_Contact_Form_Search_Custom_Base implements CRM_Contact_Form_Search_Interface {
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
    CRM_Utils_System::setTitle(E::ts('Search Group Subscription by Date Range'));

    //Name or Email
    $form->addElement(
      'text',
      'sort_name',
      E::ts('Name'),
      CRM_Core_DAO::getAttribute('CRM_Contact_DAO_Contact', 'sort_name')
    );

    //Contact Field
    $contactTypes = ['' => E::ts('- any contact type -')] + CRM_Contact_BAO_ContactType::getSelectElements();
    $form->add('select', 'contact_type',
      E::ts('is...'),
      $contactTypes,
      FALSE,
      ['class' => 'crm-select2']
    );

    //Groups
    // Arrange groups into hierarchical listing (child groups follow their parents and have indentation spacing in title)
    $allGroups = CRM_Core_PseudoConstant::group();
    $groupHierarchy = CRM_Contact_BAO_Group::getGroupsHierarchy($allGroups, NULL, '&nbsp;&nbsp;', TRUE);
    $groupSelect = ['' => E::ts('- select group -')] + $groupHierarchy;

    $form->add('select', 'group_id',
      E::ts('in...'),
      $groupSelect,
      TRUE,
      ['class' => 'crm-select2']
    );

    //Group contact status
    $group_contact_status = [];
    foreach (CRM_Core_SelectValues::groupContactStatus() as $k => $v) {
      if (!empty($k)) {
        $group_contact_status[] = $form->createElement('checkbox', $k, NULL, $v);
      }
    }
    $form->addGroup($group_contact_status,
      'group_contact_status', E::ts('Group Status')
    );

    $form->addDatePickerRange('subscription_date', E::ts('From'), FALSE, FALSE);
    $form->addElement('hidden', 'subscription_date_range_error');

    /**
     * if you are using the standard template, this array tells the template what elements
     * are part of the search criteria
     */
    $form->assign('elements', ['sort_name', 'contact_type', 'group_id', 'group_contact_status', 'subscription_date']);
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
    // return [
    //   'summary' => 'This is a summary',
    //   'total' => 50.0,
    // ];
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
      E::ts('Name') => 'sort_name',
      E::ts('Status') => 'group_contact_status',
      E::ts('Subscription Date') => 'subscription_date',
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
      contact_a.sort_name    as sort_name,
      subscription.date      as subscription_date,
      subscription.status    as group_contact_status
    ";
  }

  /**
   * Construct a SQL FROM clause
   *
   * @return string, sql fragment with FROM and JOIN clauses
   */
  function from() {
    return "
      FROM      civicrm_contact contact_a
      INNER JOIN civicrm_group_contact group_contact ON (group_contact.contact_id = contact_a.id)
      LEFT JOIN civicrm_subscription_history subscription ON (subscription.group_id = group_contact.group_id AND subscription.contact_id = group_contact.contact_id )
    ";
  }

  /**
   * Construct a SQL WHERE clause
   *
   * @param bool $includeContactIDs
   * @return string, sql fragment with conditional expressions
   */
  function where($includeContactIDs = FALSE) {
    $params = [];
    $where = "";

    $count  = 1;
    $clause = [];
    $this->_params = CRM_Contact_BAO_Query::convertFormValues($this->_formValues);

    //Filter by Name
    $name   = CRM_Utils_Array::value('sort_name', $this->_formValues);
    if ($name != NULL) {
      if (strpos($name, '%') === FALSE) {
        $name = "%{$name}%";
      }
      $params[$count] = [$name, 'String'];
      $clause[] = "contact_a.sort_name LIKE %{$count}";
      $count++;
    }

    //filter by Group
    $group_id   = CRM_Utils_Array::value('group_id', $this->_formValues);
    if ($group_id != NULL) {
      $params[$count] = [$group_id, 'Integer'];
      $clause[] = "group_contact.group_id = %{$count}";
      $count++;
    }

    //filter by group contact status
    $group_status   = CRM_Utils_Array::value('group_contact_status', $this->_formValues);
    if (!empty($group_status)) {
      $status   = '"'.implode('", "', array_keys($group_status)).'"';
      $clause[] = "group_contact.status IN ({$status})";
    }

    foreach ($this->_params as $key => $filters) {
      if ($filters[0] == 'contact_type') {
        $typeValue = '"'.implode('", "', $filters[2]).'"';
        if ($typeValue) {
          $clause[] = "contact_a.contact_type IN ({$typeValue})";
        }
      }
      if ($filters[0] == 'contact_sub_type') {
        $typeValue = '"'.implode('", "', $filters[2]).'"';
        if ($typeValue) {
          $clause[] = "contact_a.contact_sub_type IN ({$typeValue})";
        }
      }

      //Date range filter
      if ($filters[0] == 'subscription_date_relative' && !empty($filters[2])) {
        list($relativeFrom, $relativeTo) = CRM_Utils_Date::getFromTo($filters[2]);
        $clause[] = "subscription.date >= {$relativeFrom}";
        $clause[] = "subscription.date <= {$relativeTo}";
      }
      elseif ($filters[0] == 'subscription_date_low') {
        $fromDate = $filters[2];
        if ($fromDate) {
          $fromDate = date('YmdHis', strtotime($fromDate));
          $clause[] = "subscription.date >= {$fromDate}";
        }
      }
      elseif ($filters[0] == 'subscription_date_high') {
        $toDate = $filters[2];
        if ($toDate) {
          $toDate = date('YmdHis', strtotime($toDate));
          $clause[] = "subscription.date <= {$toDate}";
        }
      }
    }


    if (!empty($clause)) {
      $where .= implode(' AND ', $clause);
    }

    return $this->whereClause($where, $params);
  }

  /**
   * Determine the Smarty template for the search screen
   *
   * @return string, template path (findable through Smarty template path)
   */
  function templateFile() {
    return 'CRM/Gdpr/Form/Search/GroupcontactDetails.tpl';
  }

  /**
   * Modify the content of each row
   *
   * @param array $row modifiable SQL result row
   * @return void
   */
  function alterRow(&$row) {
    // $row['sort_name'] .= ' ( altered )';
  }
}
