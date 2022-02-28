<?php
/**
 * Helper class for Event Terms and Conditions.
 */
class CRM_Gdpr_SLA_Event extends CRM_Gdpr_SLA_Entity {

  function __construct($id) {
    parent::__construct($id, 'Event');
    $this->customGroup = 'Event_terms_and_conditions';
    $this->activityCustomGroup = 'Event_terms_and_conditions_acceptance';
    $this->activityType = 'Event terms and conditions acceptance';
    $this->enabledSetting = 'event_tc_enable';
  }
}
