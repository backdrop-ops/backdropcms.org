<?php

/**
 * @file
 * Mailchimp module Drush integration
 */

/**
 * Implements hook_drush_command().
 */
function mailchimp_drush_command() {
  $items = array();

  $items['mailchimp-cron'] = array(
    'callback' => '_mailchimp_cron_batch',
    'description' => "Trigger Mailchimp cron task",
    'examples' => array(
      'drush mailchimp-cron 5000' => 'Run Mailchimp cron with a batch size of 5000.',
      'drush mailchimp-cron' => 'Run a Mailchimp cron task.',
    ),
  );

  return $items;
}

/**
 * Callback function to run cron.
 * 
 * @int null $temp_batchsize
 */
function _mailchimp_cron_batch($temp_batchsize = NULL) {
  $config = config('mailchimp.settings');
  $stored_batchsize = NULL;
  if (!is_null($temp_batchsize)) {
    $stored_batchsize = $config->get('batch_limit');
    $config->set('batch_limit', $temp_batchsize);
  }
  mailchimp_cron();
  if (!is_null($stored_batchsize)) {
    $config->set('batch_limit', $stored_batchsize);
  }
  $config->save();
}
