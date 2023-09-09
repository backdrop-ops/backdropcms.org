-- Change the labels for Custom groups and fields
UPDATE civicrm_custom_group SET title = 'Data Policy' WHERE name = 'SLA_Acceptance';
UPDATE civicrm_custom_field SET label = 'Data Policy' WHERE name = 'Terms_Conditions' 
AND custom_group_id = 
(
  SELECT id FROM civicrm_custom_group 
  WHERE name = 'SLA_Acceptance'
);
-- Change activity label from 'Terms & Conditions Acceptance' to Data Policy
-- Acceptance
UPDATE civicrm_option_value SET label = 'Data Policy Acceptance' 
WHERE name = 'SLA Acceptance'
AND option_group_id = 
(
  SELECT id FROM civicrm_option_group WHERE name = 'activity_type'
);
