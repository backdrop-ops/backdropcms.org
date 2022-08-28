<?php

return [
  'airmail_secretcode' => [
    'group_name' => 'Airmail Preferences',
    'group' => 'airmail',
    'name' => 'airmail_secretcode',
    'type' => 'String',
    'default' => NULL,
  ],
  'airmail_external_smtp_service' => [
    'group_name' => 'Airmail Preferences',
    'group' => 'airmail',
    'name' => 'airmail_external_smtp_service',
    'type' => 'String',
    'default' => 'ses',
  ],
  'airmail_ee_wrapunsubscribe' => [
    'group_name' => 'Airmail Preferences',
    'group' => 'airmail',
    'name' => 'airmail_ee_wrapunsubscribe',
    'type' => 'Boolean',
    'default' => FALSE,
  ],
  'airmail_ee_unsubscribe' => [
    'group_name' => 'Airmail Preferences',
    'group' => 'airmail',
    'name' => 'airmail_ee_unsubscribe',
    'type' => 'String',
    'default' => 'Emails that are not sent to subscribers (e.g. receipts, confirmations etc.) won’t have an unsubscribe link. You can block our use of this email address using the link below, but this will also prevent us sending receipts or confirmations in future.',
  ],
];

