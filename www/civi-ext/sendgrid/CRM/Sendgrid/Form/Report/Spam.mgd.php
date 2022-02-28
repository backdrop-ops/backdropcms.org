<?php
// This file declares a managed database record of type "ReportTemplate".
// The record will be automatically inserted, updated, or deleted from the
// database as appropriate. For more details, see "hook_civicrm_managed" at:
// http://wiki.civicrm.org/confluence/display/CRMDOC42/Hook+Reference
return array (
  0 => 
  array (
    'name' => 'CRM_Sendgrid_Form_Report_Spam',
    'entity' => 'ReportTemplate',
    'params' => 
    array (
      'version' => 3,
      'label' => 'Mail Spam Report',
      'description' => 'Display contacts who reported the mailing as spam',
      'class_name' => 'CRM_Sendgrid_Form_Report_Spam',
      'report_url' => 'com.imba.sendgrid/spam',
      'component' => 'CiviMail',
    ),
  ),
);