<?php
/**
 * @file
 * This file contains no working PHP code; it exists to provide additional
 * documentation for doxygen as well as to document hooks in the standard
 * Backdrop manner.
 */

/**
 * @defgroup bakery Bakery module integrations.
 *
 * Module integrations with the Bakery SSO.
 */

/**
 * Bakery data transmit hook invoked before data is sent to another site.
 *
 * Invoked on master during user_save before syncing to subsites. Also invoked
 * on subsite when requesting account data from master for account creation.
 *
 * @param User $account
 *   User account object.
 *
 * @return array
 *   Keyed array of data to pass along to other sites.
 */
function hook_bakery_transmit($account) {
  return array(
    'example_field' => 'example_value',
  );
}

/**
 * Bakery data receive hook invoked on response from data sync from master.
 *
 * Invoked on subsites after requesting account data from master for account
 * creation. Also invoked on subsites during account data sync from master.
 *
 * Note, callers are responsible for data validation.
 *
 * @param User $account
 *   User account object.
 * @param array $cookie
 *   Data sent from the master. Custom data sent by master's
 *   hook_bakery_transmit() will be available as top-level elements.
 */
function hook_bakery_receive($account, $cookie) {
  if (!empty($cookie['example_field'])) {
    db_update('example_table')
      ->fields(array(
        'example_field' => $cookie['example_field'],
        ))
      ->execute();
  }
}
