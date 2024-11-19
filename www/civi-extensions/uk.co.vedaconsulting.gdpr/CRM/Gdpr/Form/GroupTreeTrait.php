<?php
trait CRM_Gdpr_Form_GroupTreeTrait {

  /**
   * Add the group data as a formatted array to the form.
   *
   * @param string $subType
   * @param int $gid
   * @param bool $onlySubType
   * @param bool $getCachedTree
   *
   * @return array
   * @throws \CRM_Core_Exception
   */
  public function setGroupTree($subType, $gid, $onlySubType = NULL, $getCachedTree = TRUE) {
    $singleRecord = NULL;
    if (!empty($this->_groupCount) && !empty($this->_multiRecordDisplay) && $this->_multiRecordDisplay == 'single') {
      $singleRecord = $this->_groupCount;
    }
    $mode = CRM_Utils_Request::retrieve('mode', 'String', $this);
    // when a new record is being added for multivalued custom fields.
    if (isset($this->_groupCount) && $this->_groupCount == 0 && $mode == 'add' &&
      !empty($this->_multiRecordDisplay) && $this->_multiRecordDisplay == 'single') {
      $singleRecord = 'new';
    }

    $groupTree = CRM_Core_BAO_CustomGroup::getTree($this->_type,
      NULL,
      $this->_entityId,
      $gid,
      $subType,
      $this->_subName,
      $getCachedTree,
      $onlySubType,
      FALSE,
      CRM_Core_Permission::EDIT,
      $singleRecord
    );

    if (property_exists($this, '_customValueCount') && !empty($groupTree)) {
      $this->_customValueCount = CRM_Core_BAO_CustomGroup::buildCustomDataView($this, $groupTree, TRUE, NULL, NULL, NULL, $this->_entityId);
    }
    // we should use simplified formatted groupTree
    $groupTree = CRM_Core_BAO_CustomGroup::formatGroupTree($groupTree, $this->_groupCount, $this);

    if (isset($this->_groupTree) && is_array($this->_groupTree)) {
      $keys = array_keys($groupTree);
      foreach ($keys as $key) {
        $this->_groupTree[$key] = $groupTree[$key];
      }
      return [$this, $groupTree];
    }
    else {
      $this->_groupTree = $groupTree;
      return [$this, $groupTree];
    }
  }
}
