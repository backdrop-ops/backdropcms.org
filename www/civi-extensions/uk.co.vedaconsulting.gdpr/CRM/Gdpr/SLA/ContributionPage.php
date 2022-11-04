<?php
/**
 * Helper class for Event Terms and Conditions.
 */
class CRM_Gdpr_SLA_ContributionPage extends CRM_Gdpr_SLA_Entity {

  function __construct($id) {
    parent::__construct($id, 'ContributionPage');
    $this->customGroup = 'Contribution_Page_terms_and_conditions';
    $this->activityCustomGroup = 'Contribution_terms_and_conditions_acceptance';
    $this->activityType = 'Contribution terms and conditions acceptance';
    $this->enabledSetting = 'cp_tc_enable';
  }
}
