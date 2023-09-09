-- Remove  data created by the gdpr extension --

-- Remove custom fields and values.

-- Group: SLA_Acceptance
 DELETE FROM `civicrm_custom_field` where `custom_group_id` = 
  (SELECT id FROM `civicrm_custom_group` WHERE `name`  = 'SLA_Acceptance');

 DELETE FROM `civicrm_custom_group` WHERE `name` = 'SLA_Acceptance';

 DROP TABLE IF EXISTS `civicrm_value_sla_acceptance_4`;

-- Group: Contribution_Page_terms_and_conditions
 DELETE FROM `civicrm_custom_field` where `custom_group_id` = 
  (SELECT id FROM `civicrm_custom_group` WHERE `name`  = 'Contribution_Page_terms_and_conditions');

 DELETE FROM `civicrm_custom_group` WHERE `name` = 'Contribution_Page_terms_and_conditions';

 DROP TABLE IF EXISTS `civicrm_value_contribution_page_terms_and_conditions_7`;

-- Group: Contribution_terms_and_conditions_acceptance
 DELETE FROM `civicrm_custom_field` where `custom_group_id` = 
  (SELECT id FROM `civicrm_custom_group` WHERE `name`  = 'Contribution_terms_and_conditions_acceptance');

 DELETE FROM `civicrm_custom_group` WHERE `name` = 'Contribution_terms_and_conditions_acceptance';

 DROP TABLE IF EXISTS `civicrm_value_contribution_terms_and_conditions_acceptan_8`;

 -- Group: Event_terms_and_conditions
DELETE FROM `civicrm_custom_field` where `custom_group_id` = 
  (SELECT id FROM `civicrm_custom_group` WHERE `name`  = 'Event_terms_and_conditions');

 DELETE FROM `civicrm_custom_group` WHERE `name` = 'Event_terms_and_conditions';

 DROP TABLE IF EXISTS `civicrm_value_event_terms_and_conditions_7`;

-- Group: Event terms and conditions acceptance
DELETE FROM `civicrm_custom_field` where `custom_group_id` = 
  (SELECT id FROM `civicrm_custom_group` WHERE `name`  = 'Event_terms_and_conditions_acceptance');

 DELETE FROM `civicrm_custom_group` WHERE `name` = 'Event_terms_and_conditions_acceptance';

 DROP TABLE IF EXISTS `civicrm_value_event_terms_and_conditions_acceptance_9`;

-- Delete option groups and values 
-- comm_pref_options
DELETE FROM civicrm_option_value WHERE option_group_id = 
  (SELECT `id` 
    FROM civicrm_option_group 
    WHERE `name` = 'comm_pref_options'
  );
DELETE FROM `civicrm_option_group` WHERE `name` = 'comm_pref_options';

-- checkbox_position_20180311180849
DELETE FROM civicrm_option_value WHERE option_group_id = 
  (SELECT `id` 
    FROM civicrm_option_group 
    WHERE `name` = 'checkbox_position_20180311180849'
  );
DELETE FROM `civicrm_option_group` WHERE `name` = 'checkbox_position_20180311180849';

-- activity type option values

DELETE FROM civicrm_option_value WHERE 
  `name` IN (
    'GDPR_FORGET_ME', 
    'SLA Acceptance', 
    'Update_Communication_Preferences', 
    'Event Terms and Conditions Acceptance',
    'Contribution Terms and Conditions Acceptance'
  )
  AND option_group_id = 
  (SELECT `id` 
    FROM civicrm_option_group 
    WHERE `name` = 'activity_type'
  );

-- Delete settings
DELETE FROM `civicrm_setting` 
  WHERE `name` = 'gdpr_communications_preferences_group_settings'
  OR `name` = 'gdpr_communications_preferences_settings'
  OR `name` = 'gdpr_settings';

-- End removal of custom data --  
