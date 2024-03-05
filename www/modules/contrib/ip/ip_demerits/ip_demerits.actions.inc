<?php
/**
 * @file
 * Actions for IP Demerits module.
 */

/**
 * Callback for "Remove demerit" bulk action.
 */
function ip_demerits_remove_demerit_action($demerit, &$context) {
  db_query('
    DELETE FROM {ip_demerits}
    WHERE demerit_id = :demerit_id
    ', array(':demerit_id' => $demerit->demerit_id));
}
