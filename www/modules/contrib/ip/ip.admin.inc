<?php
/**
 * @file
 * IP Address Manager administrative functions.
 */

/**
 * Display form for details for an individual IP address.
 */
function ip_details_form($form, &$form_state, $ip) {
  backdrop_set_title(t('IP address details for @ip', array('@ip' => $ip)));
  $form = array();

  if (user_access('manage ip addresses')) {
    $form['users'] = array(
      '#type' => 'fieldset',
      '#title' => t('Users'),
    );
    $total_users = (int) db_query('
      SELECT COUNT(uid)
      FROM {ip_tracker}
      WHERE ip = :ip
      ', array(':ip' => ip2long($ip)))
      ->fetchField();
    $form['users'][] = array(
      '#type' => 'help',
      '#markup' => $total_users ? format_plural($total_users, 'This address has been used by 1 account.', 'This address has been used by @num accounts.', array('@num' => $total_users)) : t('This address has not been used by any accounts.'),
    );
    $form['users'][] = array(
      '#markup' => views_embed_view('ip_address_users_by_ip', 'default', $ip),
    );
  }
  return $form;
}
