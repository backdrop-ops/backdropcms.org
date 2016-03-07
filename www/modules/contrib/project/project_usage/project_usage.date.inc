<?php

/**
 * @file
 * API functions for manipulating usage dates and timestamps.
 */

/**
 * getdate() with timezone adjustment.
 *
 * PHP's getdate() is affected by the server's timezone. We need to cancel it
 * out so everything is GMT.
 *
 * @param $timestamp
 *   An optional, integer UNIX timestamp.
 * @return
 *   An array with results identical to PHP's getdate().
 */
function project_usage_gmgetdate($timestamp = NULL) {
  $timestamp = isset($timestamp) ? $timestamp : REQUEST_TIME;
  $gmt_offset = (int) date('Z', $timestamp);
  return getdate($timestamp - $gmt_offset);
}

/**
 * Compute the starting timestamp for a day based on a timestamp.
 *
 * @param $timestamp
 *   Mixed, either a GMT timestamp or an array returned by
 *   project_usage_gmgetdate().
 * @param $day_adjustment
 *   An integer specifying a number of days to adjust. A value of 0 will give
 *   the start of the current day. A value of -1 will give the start of the
 *   previous day. A value of 1 will give the value of the start of the next
 *   day.
 *
 * @return
 *   GMT UNIX timestamp.
 */
function project_usage_daily_timestamp($time = NULL, $day_adjustment = 0) {
  $time_parts = is_array($time) ? $time : project_usage_gmgetdate($time);
  $day = $time_parts['mday'] + $day_adjustment;
  return gmmktime(0, 0, 0, $time_parts['mon'], $day, $time_parts['year']);
}

/**
 * Compute a timestamp for the beginning of a week N weeks ago.
 *
 * @param $time
 *   Mixed. Integer timestamp or an array returned by project_usage_gmgetdate().
 * @param $weeks_ago
 *   An integer specifying a number of weeks previous. A value of 0 indicates
 *   the current week.
 *
 * @return
 *   GMT UNIX timestamp.
 */
function project_usage_weekly_timestamp($time = NULL, $weeks_ago = 0) {
  $time_parts = is_array($time) ? $time : project_usage_gmgetdate($time);
  $day = $time_parts['mday'] - $time_parts['wday'] + (7 * $weeks_ago);
  return gmmktime(0, 0, 0, $time_parts['mon'], $day, $time_parts['year']);
}

/**
 * Build an array of timestamps for the beginning (midnight Sunday) for each
 * week since a given timestamp.
 *
 * @param $timestamp
 *   UNIX timestamp. The first returned timestamp will be the beginning of the
 *   week with this time in it.
 * @return
 *   An array of GMT timestamps sorted in ascending order. The first value is
 *   is the week containing $timestamp. Each subsequent value is the timestamp
 *   for the next week. The final value is the beginning of the current week.
 */
function project_usage_get_weeks_since($timestamp) {
  $times = array();

  // First, compute the start of the current week so we know when to stop...
  $this_week = project_usage_weekly_timestamp();

  // ...then compute all the weeks up to that.
  $parts = project_usage_gmgetdate($timestamp);
  $i = 0;
  do {
    $times[$i] = project_usage_weekly_timestamp($parts, $i);
  } while ($times[$i++] < $this_week);

  return $times;
}

/**
 * Return an array of the most recent weeks for which we have data.
 *
 * @return
 *   An array of UNIX timestamps sorted newest to oldest. Will not include
 *   the current week.
 */
function project_usage_get_active_weeks($reset = FALSE) {
  $weeks = state_get('project_usage_active_weeks', array());
  if ($reset || empty($weeks)) {
    $count = config_get('project_usage.settings', 'number_shown_weeks');
    $timestamps = db_query_range("SELECT DISTINCT(timestamp) FROM {project_usage_week_project} ORDER BY timestamp DESC", 0, $count);
    $weeks = array();
    foreach ($timestamps as $week) {
      $weeks[] = $week->timestamp;
    }
    state_set('project_usage_active_weeks', $weeks);
  }
  return $weeks;
}

/**
 * Get the latest week with the latest project usage data.
 *
 * @return
 *   The UNIX timestamp of the latest week with project usage data.
 */
function project_usage_get_current_active_week() {
  static $current_week = 0;
  if (empty($current_week)) {
    $weeks = project_usage_get_active_weeks();
    $current_week = array_shift($weeks);
  }
  return $current_week;
}

/**
 * Sets the expiry timestamp for cached project usage pages.
 *
 * Default is 24 hours.
 *
 * @return The UNIX timestamp to expire the page at.
 */
function project_usage_cache_time() {
  return REQUEST_TIME + config_get('project_usage.settings', 'cache_length');
}

