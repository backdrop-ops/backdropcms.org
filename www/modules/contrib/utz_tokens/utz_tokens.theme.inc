<?php
/**
 * @file
 * Theme and preprocess functions for output by User Time Zone Tokens module.
 */

/**
 * @addtogroup themeable
 * @{
 */

/**
 * Returns HTML for a utz_datetime element.
 */
function theme_utz_datetime($variables) {
  $datetime = $variables['datetime'];
  $format = $variables['format'];

  // Allow use of the machine name of a system date/time format.
  $system_formats = system_get_date_formats();
  if (isset($system_formats[$format])) {
    $format = $system_formats[$format]['pattern'];
  }

  try {
    $datetime_obj = new DateTime($datetime); // could fail & throw exception,
    $datetime_obj->setTimeZone(new DateTimeZone(backdrop_get_user_timezone()));
    $format = filter_xss($format);
    $format_attr = str_replace('"', '&quot;', $format); // sanitize for attribute
    $user_is_anonymous = user_is_anonymous() ? '1' : '0';
    return '<span class="utz-datetime" data-timestamp="' . $datetime_obj->getTimestamp() . '" data-format="' . $format_attr . '" data-anonymous="' . $user_is_anonymous . '">' . $datetime_obj->format($format) . '</span>';
  }
  catch (Exception $e) {
    return t('utz_datetime error: unable to parse time %datetime.', array('%datetime' => $datetime));
  }
}

/** @} End of addtogroup themeable */
