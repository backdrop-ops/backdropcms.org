<?php
/**
 * @file
 * User pages for IP Demerits module.
 */

/**
 * Replacement page for user's IP tab.
 */
function ip_demerits_user_ip_view($form, &$form_state, $uid) {
  $account = user_load($uid);
  if (!$account) {
    return array(array('#markup' => t('There is no account with uid @uid', array(
      '@uid' => $uid))));
  }

  // This is the original View showing all IP addresses for the user
  $build[] = array(
    '#markup' => t('IP addresses'),
    '#prefix' => '<h2>',
    '#suffix' => '</h2>',
  );
  $build[] = array(
    '#markup' => views_embed_view('ip_tracker_user', 'page', $uid),
  );

  // Listing of all demerits received by this user
  $build[] = array(
    '#markup' => t('Demerits'),
    '#prefix' => '<h2>',
    '#suffix' => '</h2>',
  );

  // TBD, add total demerits for this user
  $total_points = (int) db_query('
    SELECT points
    FROM {ip_demerits_uid_totals}
    WHERE uid = :uid
    ', array(':uid' => $uid))
    ->fetchField();
  $build[] = array(
    '#type' => 'help',
    '#markup' => $total_points ? format_plural($total_points, '%name has received 1 demerit.', '%name has received @points demerits.', array('%name' => $account->name, '@points' => $total_points)) : t('%name has received no demerits.', array('%name' => $account->name)),
  );

  $build[] = array(
    '#markup' => views_embed_view('ip_demerits', 'by_user', $uid),
  );

  return $build;
}
