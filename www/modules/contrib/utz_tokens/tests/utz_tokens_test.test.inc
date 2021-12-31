<?php
/**
 * @file Utility functions for testing User Time Zone Tokens.
 */

/**
 * Testing block for Javascript. Return a render array of test strings that
 * compare PHP and JS rendering with the same PHP formatting codes.
 */
function _utz_tokens_test_view() {
  $datetime = '2021-04-04 10:05 PDT';

  $build[] = array('#markup' => _utz_tokens_test($datetime, 'd D j l N S w z')); // days
  $build[] = array('#markup' => _utz_tokens_test($datetime, 'W')); // weeks
  $build[] = array('#markup' => _utz_tokens_test($datetime, 'F m M n t')); // months
  $build[] = array('#markup' => _utz_tokens_test($datetime, 'L o Y y')); // years
  $build[] = array('#markup' => _utz_tokens_test($datetime, 'a A')); // ampm
  $build[] = array('#markup' => _utz_tokens_test($datetime, 'B')); // swatch
  $build[] = array('#markup' => _utz_tokens_test($datetime, 'g H h')); // hours
  $build[] = array('#markup' => _utz_tokens_test($datetime, 'i s u v')); // minsec
  $build[] = array('#markup' => _utz_tokens_test($datetime, 'e I O P p T Z')); // timezone
  $build[] = array('#markup' => _utz_tokens_test($datetime, 'c')); // iso8601
  $build[] = array('#markup' => _utz_tokens_test($datetime, 'r')); // rfc2822
  $build[] = array('#markup' => _utz_tokens_test($datetime, 'U')); // timestamp

  return $build;
}

/**
 * Testing function for Javascript. Return a string to compare PHP and JS for a
 * given time and format string.
 */
function _utz_tokens_test($datetime, $format) {
  $datetime_obj = new DateTime($datetime);
  $datetime_obj->setTimeZone(new DateTimeZone(backdrop_get_user_timezone()));
  $user_is_anonymous = user_is_anonymous() ? '1' : '0';
  $phpstr = $datetime_obj->format($format);
  $jsstr = '<span class="utz-datetime" data-timestamp="' . $datetime_obj->getTimestamp() . '" data-format="' . $format . '" data-anonymous="' . $user_is_anonymous . '">' . 'PLACEHOLDER' . '</span>';
  return '<p>time: \'' . $datetime . '\' format: \'' . $format . '\'<br>' . $phpstr . '<br>' . $jsstr . '</p>';
}
