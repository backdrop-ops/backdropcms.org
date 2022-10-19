<?php
/**
 * @file
 * Widget form of Repeating dates module.
 */

/**
 * Helper function to return the complex form for the widget.
 *
 * @see repeating_dates_field_widget_form()
 */
function _repeating_dates_widget_form($form_state, $instance, $values, $cardinality) {
  if (isset($values['rrule'])) {
    // Values have to get parsed, as the rrule comes as string.
    $parsed = _repeating_dates_parse_rrule($values);
  }
  else {
    // Back from node preview, parsed values come from the tempstore.
    $parsed = $values;
  }

  if ($cardinality == FIELD_CARDINALITY_UNLIMITED) {
    // "-1" makes no sense as #max attribute.
    $cardinality = REPEATING_DATES_MAX_VALUES;
  }
  elseif ($cardinality < 5) {
    backdrop_set_message(t('The "Number of values" setting in the field %field is very low. You can only save !numvalue.', array(
      '%field' => $instance['label'],
      '!numvalue' => format_plural($cardinality, '1 value', '@count values'),
    )), 'warning');
  }
  $weekday_options = _repeating_dates_weekday_options();

  $selector_part = $instance['field_name'] . '[und][rrule]';
  $widget_settings = $instance['widget']['settings'];

  $element = array();
  $element['rrule'] = array(
    '#type' => 'fieldset',
    '#title' => $instance['label'],
  );

  // Ical: DTSTART.
  $element['rrule']['dtstart'] = array(
    '#type' => 'html_datetime',
    '#title' => t('Start'),
    '#default_value' => isset($parsed['dtstart']) ? $parsed['dtstart'] : NULL,
    '#prefix' => '<div class="rd-group rd-group-dtstart">',
    '#attributes' => array(
      'time' => array('step' => '60'),
    ),
    '#required' => $instance['required'],
  );
  // Ical: DTEND.
  if (!$widget_settings['hide_to_date']) {
    $element['rrule']['dtend'] = array(
      '#type' => 'html_datetime',
      '#title' => t('To', array(), array('context' => 'Until date or time')),
      '#default_value' => isset($parsed['dtend']) ? $parsed['dtend'] : NULL,
      '#suffix' => '</div>',
      '#attributes' => array(
        'time' => array('step' => '60'),
      ),
    );
  }
  else {
    $element['rrule']['dtend'] = array(
      '#type' => 'value',
      '#value' => isset($parsed['dtstart']) ? $parsed['dtstart'] : NULL,
    );
    $element['rrule']['dtstart']['#suffix'] = '</div>';
  }

  // Ical: FREQ.
  $element['rrule']['freq'] = array(
    '#type' => 'select',
    '#title' => t('Repeats'),
    '#options' => _repeating_dates_frequency_options(),
    '#default_value' => isset($parsed['freq']) ? $parsed['freq'] : $widget_settings['default_repeat_freq'],
    '#wrapper_attributes' => array('class' => array('frequency-select')),
  );

  // Ical: INTERVAL.
  $element['rrule']['interval_day'] = array(
    '#type' => 'number',
    '#title' => t('Every'),
    '#field_suffix' => t('day(s)'),
    '#min' => 1,
    '#max' => 366,
    '#size' => 3,
    '#default_value' => isset($parsed['interval_day']) ? $parsed['interval_day'] : 1,
    '#states' => array(
      'visible' => array(
        ':input[name="' . $selector_part . '[freq]"]' => array('value' => 'daily'),
      ),
      'enabled' => array(
        ':input[name="' . $selector_part . '[freq]"]' => array('value' => 'daily'),
      ),
    ),
  );

  // Group "weekly".
  // Ical: INTERVAL.
  $element['rrule']['interval_week'] = array(
    '#type' => 'number',
    '#title' => t('Every'),
    '#field_suffix' => t('week(s)'),
    '#min' => 1,
    '#max' => 53,
    '#size' => 3,
    '#default_value' => isset($parsed['interval_week']) ? $parsed['interval_week'] : 1,
    '#states' => array(
      'visible' => array(
        ':input[name="' . $selector_part . '[freq]"]' => array('value' => 'weekly'),
      ),
      'enabled' => array(
        ':input[name="' . $selector_part . '[freq]"]' => array('value' => 'weekly'),
      ),
    ),
  );
  // Ical: BYDAY.
  $element['rrule']['byday_week'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Weekdays'),
    '#options' => _repeating_dates_weedays_short(),
    '#default_value' => isset($parsed['byday_week']) ? $parsed['byday_week'] : array(),
    '#states' => array(
      'visible' => array(
        ':input[name="' . $selector_part . '[freq]"]' => array('value' => 'weekly'),
      ),
      'enabled' => array(
        ':input[name="' . $selector_part . '[freq]"]' => array('value' => 'weekly'),
      ),
    ),
  );

  // Group "monthly".
  // Ical: INTERVAL.
  $element['rrule']['interval_month'] = array(
    '#type' => 'number',
    '#title' => t('Every'),
    '#field_suffix' => t('month(s)'),
    '#min' => 1,
    '#max' => 12,
    '#size' => 3,
    '#default_value' => isset($parsed['interval_month']) ? $parsed['interval_month'] : 1,
    '#states' => array(
      'visible' => array(
        ':input[name="' . $selector_part . '[freq]"]' => array('value' => 'monthly'),
      ),
      'enabled' => array(
        ':input[name="' . $selector_part . '[freq]"]' => array('value' => 'monthly'),
      ),
    ),
  );
  // Switch between BYMONTHDAY and BYSETPOS + BYDAY.
  $element['rrule']['monthday_setpos'] = array(
    '#type' => 'radios',
    '#title' => t('Selection type'),
    '#options' => array(
      0 => t('By day of month (number)'),
      1 => t('By position and day'),
    ),
    '#default_value' => isset($parsed['monthday_setpos']) ? $parsed['monthday_setpos'] : 0,
    '#states' => array(
      'visible' => array(
        ':input[name="' . $selector_part . '[freq]"]' => array('value' => 'monthly'),
      ),
      'enabled' => array(
        ':input[name="' . $selector_part . '[freq]"]' => array('value' => 'monthly'),
      ),
    ),
  );
  // Ical: BYMONTHDAY.
  $element['rrule']['bymonthday_month'] = array(
    '#type' => 'number',
    '#title' => t('Day of month'),
    '#min' => 1,
    '#max' => 31,
    '#size' => 3,
    '#default_value' => isset($parsed['bymonthday_month']) ? $parsed['bymonthday_month'] : 1,
    '#states' => array(
      'visible' => array(
        ':input[name="' . $selector_part . '[freq]"]' => array('value' => 'monthly'),
        ':input[name="' . $selector_part . '[monthday_setpos]"]' => array('value' => '0'),
      ),
      'enabled' => array(
        ':input[name="' . $selector_part . '[freq]"]' => array('value' => 'monthly'),
        ':input[name="' . $selector_part . '[monthday_setpos]"]' => array('value' => '0'),
      ),
    ),
  );
  // Ical: BYSETPOS.
  $element['rrule']['bysetpos_month'] = array(
    '#type' => 'select',
    '#title' => t('Position'),
    '#options' => _repeating_dates_position_options(),
    '#default_value' => isset($parsed['bysetpos_month']) ? $parsed['bysetpos_month'] : 1,
    '#states' => array(
      'visible' => array(
        ':input[name="' . $selector_part . '[freq]"]' => array('value' => 'monthly'),
        ':input[name="' . $selector_part . '[monthday_setpos]"]' => array('value' => '1'),
      ),
      'enabled' => array(
        ':input[name="' . $selector_part . '[freq]"]' => array('value' => 'monthly'),
        ':input[name="' . $selector_part . '[monthday_setpos]"]' => array('value' => '1'),
      ),
    ),
    '#prefix' => '<div class="rd-group rd-group-posday">',
  );
  // Ical: BYDAY.
  $element['rrule']['byday_month'] = array(
    '#type' => 'select',
    '#title' => 'Day',
    '#options' => $weekday_options,
    '#default_value' => isset($parsed['byday_month']) ? $parsed['byday_month'] : '',
    '#states' => array(
      'visible' => array(
        ':input[name="' . $selector_part . '[freq]"]' => array('value' => 'monthly'),
        ':input[name="' . $selector_part . '[monthday_setpos]"]' => array('value' => '1'),
      ),
      'enabled' => array(
        ':input[name="' . $selector_part . '[freq]"]' => array('value' => 'monthly'),
        ':input[name="' . $selector_part . '[monthday_setpos]"]' => array('value' => '1'),
      ),
    ),
    '#suffix' => '</div>',
  );

  // Group "yearly".
  // Switch between BYMONTHDAY + BYMONTH and BYSETPOS + BYDAY + BYMONTH.
  $element['rrule']['yearly_setpos'] = array(
    '#type' => 'radios',
    '#title' => t('Selection type'),
    '#options' => array(
      0 => t('By day (number) and month'),
      1 => t('By position, day and month'),
    ),
    '#default_value' => isset($parsed['yearly_setpos']) ? $parsed['yearly_setpos'] : 0,
    '#states' => array(
      'visible' => array(
        ':input[name="' . $selector_part . '[freq]"]' => array('value' => 'yearly'),
      ),
      'enabled' => array(
        ':input[name="' . $selector_part . '[freq]"]' => array('value' => 'yearly'),
      ),
    ),
  );
  // Ical: BYMONTHDAY.
  $element['rrule']['bymonthday_year'] = array(
    '#type' => 'number',
    '#title' => t('Day'),
    '#min' => 1,
    '#max' => 31,
    '#size' => 3,
    '#default_value' => isset($parsed['bymonthday_year']) ? $parsed['bymonthday_year'] : 1,
    '#states' => array(
      'visible' => array(
        ':input[name="' . $selector_part . '[freq]"]' => array('value' => 'yearly'),
        ':input[name="' . $selector_part . '[yearly_setpos]"]' => array('value' => '0'),
      ),
      'enabled' => array(
        ':input[name="' . $selector_part . '[freq]"]' => array('value' => 'yearly'),
        ':input[name="' . $selector_part . '[yearly_setpos]"]' => array('value' => '0'),
      ),
    ),
    '#prefix' => '<div class="rd-group rd-group-year">',
  );
  // Ical: BYSETPOS.
  $element['rrule']['bysetpos_year'] = array(
    '#type' => 'select',
    '#title' => t('Position'),
    '#options' => _repeating_dates_position_options(),
    '#default_value' => isset($parsed['bysetpos_year']) ? $parsed['bysetpos_year'] : 1,
    '#states' => array(
      'visible' => array(
        ':input[name="' . $selector_part . '[freq]"]' => array('value' => 'yearly'),
        ':input[name="' . $selector_part . '[yearly_setpos]"]' => array('value' => '1'),
      ),
      'enabled' => array(
        ':input[name="' . $selector_part . '[freq]"]' => array('value' => 'yearly'),
        ':input[name="' . $selector_part . '[yearly_setpos]"]' => array('value' => '1'),
      ),
    ),
  );
  // Ical: BYDAY.
  unset($weekday_options['any']);
  $element['rrule']['byday_year'] = array(
    '#type' => 'select',
    '#title' => t('Day'),
    '#options' => $weekday_options,
    '#default_value' => isset($parsed['byday_year']) ? $parsed['byday_year'] : '',
    '#states' => array(
      'visible' => array(
        ':input[name="' . $selector_part . '[freq]"]' => array('value' => 'yearly'),
        ':input[name="' . $selector_part . '[yearly_setpos]"]' => array('value' => '1'),
      ),
      'enabled' => array(
        ':input[name="' . $selector_part . '[freq]"]' => array('value' => 'yearly'),
        ':input[name="' . $selector_part . '[yearly_setpos]"]' => array('value' => '1'),
      ),
    ),
  );
  // Ical: BYMONTH.
  $element['rrule']['bymonth_year'] = array(
    '#type' => 'select',
    '#title' => t('Month'),
    '#options' => date_month_names(TRUE),
    '#default_value' => isset($parsed['bymonth_year']) ? $parsed['bymonth_year'] : 1,
    '#states' => array(
      'visible' => array(
        ':input[name="' . $selector_part . '[freq]"]' => array('value' => 'yearly'),
      ),
      'enabled' => array(
        ':input[name="' . $selector_part . '[freq]"]' => array('value' => 'yearly'),
      ),
    ),
    '#suffix' => '</div>',
  );

  // Setting an explicit end is essential, as we write all recurrences to the
  // database.
  // Switch between COUNT and UNTIL.
  $element['rrule']['stopby'] = array(
    '#type' => 'radios',
    '#title' => t('Stop based on'),
    '#options' => array(
      'count' => t('Count'),
      'date' => t('Date'),
    ),
    '#default_value' => isset($parsed['stopby']) ? $parsed['stopby'] : $widget_settings['default_stop_mode'],
    '#states' => array(
      'visible' => array(
        ':input[name="' . $selector_part . '[freq]"]' => array('!value' => ''),
      ),
      'enabled' => array(
        ':input[name="' . $selector_part . '[freq]"]' => array('!value' => ''),
      ),
    ),
  );
  // Ical: COUNT.
  $count = $cardinality;
  if (isset($parsed['count']) && $parsed['count'] <= $cardinality) {
    // The default value could have changed in the meantime, don't let it be
    // higher than the field cardinality.
    $count = $parsed['count'];
  }
  $element['rrule']['count'] = array(
    '#type' => 'number',
    '#title' => t('Stop after'),
    '#min' => 1,
    '#max' => $cardinality,
    '#size' => 3,
    '#field_suffix' => t('recurrences'),
    '#default_value' => $count,
    '#states' => array(
      'visible' => array(
        ':input[name="' . $selector_part . '[freq]"]' => array('!value' => ''),
        ':input[name="' . $selector_part . '[stopby]"]' => array('value' => 'count'),
      ),
      'required' => array(
        ':input[name="' . $selector_part . '[freq]"]' => array('!value' => ''),
        ':input[name="' . $selector_part . '[stopby]"]' => array('value' => 'count'),
      ),
    ),
  );
  // Ical: UNTIL.
  $date = new DateTime('+ 3 months', new DateTimeZone('UTC'));
  $element['rrule']['until'] = array(
    '#type' => 'html_date',
    '#title' => t('Stop on'),
    '#default_value' => isset($parsed['until']) ? $parsed['until'] : $date->format('Y-m-d'),
    '#attributes' => array(
      'min' => format_date(REQUEST_TIME, 'custom', 'Y-m-d'),
    ),
    '#states' => array(
      'visible' => array(
        ':input[name="' . $selector_part . '[freq]"]' => array('!value' => ''),
        ':input[name="' . $selector_part . '[stopby]"]' => array('value' => 'date'),
      ),
      'required' => array(
        ':input[name="' . $selector_part . '[freq]"]' => array('!value' => ''),
        ':input[name="' . $selector_part . '[stopby]"]' => array('value' => 'date'),
      ),
    ),
  );

  // Ical: EXDATE.
  $exdates = isset($parsed['exdates']) ? $parsed['exdates'] : array();
  $element['rrule']['exdates'] = array(
    '#type' => 'details',
    '#summary' => t('Exclude dates'),
    '#open' => (count($exdates) > 0) ? TRUE : FALSE,
    '#states' => array(
      'visible' => array(
        ':input[name="' . $selector_part . '[freq]"]' => array('!value' => ''),
      ),
    ),
  );
  if (!empty($widget_settings['exdate_helptext'])) {
    $element['rrule']['exdates']['#details'] = check_plain($widget_settings['exdate_helptext']);
  }
  $wrapper_id = backdrop_clean_css_identifier($instance['field_name'] . '-datelist-wrapper');
  $element['rrule']['exdates']['dates'] = array(
    '#type' => 'repeating_dates_datelist',
    '#default_value' => $exdates,
    '#prefix' => '<div id="' . $wrapper_id . '">',
    '#suffix' => '</div>',
  );
  $parents = array(
    $instance['field_name'],
    LANGUAGE_NONE,
    'rrule',
    'exdates',
  );
  $element['rrule']['exdates']['add_exception'] = array(
    '#type' => 'submit',
    '#value' => t('Add exception'),
    '#submit' => array(),
    '#limit_validation_errors' => array($parents),
    '#ajax' => array(
      'callback' => 'repeating_dates_add_exdate_callback',
      'wrapper' => $wrapper_id,
    ),
  );

  $element['#element_validate'][] = '_repeating_dates_widget_validate';

  return $element;
}

/**
 * Helper function to output options for select lists.
 */
function _repeating_dates_position_options() {
  // Months have four weeks and some days... not sure.
  // Without translation context the text outcome can be odd.
  $context = array('context' => 'Repeating date pos');
  return array(
    1 => t('First', array(), $context),
    2 => t('Second', array(), $context),
    3 => t('Third', array(), $context),
    4 => t('Fourth', array(), $context),
    -1 => t('Last', array(), $context),
  );
}

/**
 * Helper function to return an ordered list of weekdays.
 */
function _repeating_dates_weekday_options() {
  // Avoid date_week_days_ordered() to keep the array keys.
  $days = date_week_days(TRUE);
  $ical_keys = _repeating_dates_get_ical_daykeys();
  $first_day = config_get('system.date', 'first_day');
  if ($first_day > 0) {
    $before = $after = array();
    foreach ($days as $number => $name) {
      if ($number < $first_day) {
        $before[$ical_keys[$number]] = $name;
      }
      else {
        $after[$ical_keys[$number]] = $name;
      }
    }
    $days = $after + $before;
  }
  else {
    $days = array_combine($ical_keys, $days);
  }
  $days['any'] = t('Day (any)');
  return $days;
}

/**
 * Helper function to return an list of abbreviated weekdays.
 */
function _repeating_dates_weedays_short() {
  $days_abbr = date_week_days_abbr(TRUE);
  $ical_keys = _repeating_dates_get_ical_daykeys();

  $first_day = config_get('system.date', 'first_day');
  if ($first_day > 0) {
    $before = $after = array();
    foreach ($days_abbr as $number => $name) {
      if ($number < $first_day) {
        $before[$ical_keys[$number]] = $name;
      }
      else {
        $after[$ical_keys[$number]] = $name;
      }
    }
    $days_abbr = $after + $before;
  }
  else {
    $days_abbr = array_combine($ical_keys, $days_abbr);
  }
  return $days_abbr;
}

/**
 * Helper function to prepare saved data for the widget form.
 *
 * @param array $values
 *   Array of the field items retrieved from the database.
 *
 * @return array
 *   Array to preselect existing values in the widget's form items.
 */
function _repeating_dates_parse_rrule($values) {
  if (empty($values)) {
    return array();
  }

  $data = array();
  if (is_numeric($values['dtstart'])) {
    $data['dtstart'] = array(
      'date' => format_date($values['dtstart'], 'custom', 'Y-m-d'),
      'time' => format_date($values['dtstart'], 'custom', 'H:i'),
    );
  }
  if (is_numeric($values['dtend'])) {
    $data['dtend'] = array(
      'date' => format_date($values['dtend'], 'custom', 'Y-m-d'),
      'time' => format_date($values['dtend'], 'custom', 'H:i'),
    );
  }
  if (empty($values['rrule'])) {
    $data['freq'] = '';
    return $data;
  }

  $ruleset = explode("\r\n", $values['rrule']);
  $rrule = str_replace('RRULE:', '', $ruleset[0]);
  $rrule_array = array();
  foreach (explode(';', $rrule) as $item) {
    $parts = explode('=', $item);
    $rrule_array[$parts[0]] = $parts[1];
  }
  // We maybe have exdates.
  $exdates = array();
  if (isset($ruleset[1])) {
    $components = explode(':', $ruleset[1]);
    $date_strings = explode(',', $components[1]);
    foreach ($date_strings as $string) {
      $date = new DateTime($string);
      $exdates[] = $date->format('Y-m-d');
    }
  }
  $data['exdates'] = $exdates;

  $data['freq'] = strtolower($rrule_array['FREQ']);

  if (isset($rrule_array['UNTIL'])) {
    $until = new DateTime($rrule_array['UNTIL']);
    $data['stopby'] = 'date';
    $data['until'] = $until->format('Y-m-d');
  }
  else {
    $data['stopby'] = 'count';
    $data['count'] = max($rrule_array['COUNT'], 1);
  }

  switch ($data['freq']) {
    case 'daily':
      $data['interval_day'] = $rrule_array['INTERVAL'];
      break;

    case 'weekly':
      $data['interval_week'] = $rrule_array['INTERVAL'];
      $data['byday_week'] = explode(',', $rrule_array['BYDAY']);
      break;

    case 'monthly':
      if (isset($rrule_array['BYSETPOS'])) {
        $data['monthday_setpos'] = '1';
        $data['bysetpos_month'] = $rrule_array['BYSETPOS'];
        $data['byday_month'] = $rrule_array['BYDAY'];
      }
      elseif (isset($rrule_array['BYMONTHDAY']) && $rrule_array['BYMONTHDAY'] == '-1') {
        // Tricky last day.
        $data['monthday_setpos'] = '1';
        $data['byday_month'] = 'any';
        $data['bysetpos_month'] = $rrule_array['BYMONTHDAY'];
      }
      else {
        $data['monthday_setpos'] = '0';
        $data['bymonthday_month'] = $rrule_array['BYMONTHDAY'];
      }
      $data['interval_month'] = $rrule_array['INTERVAL'];
      break;

    case 'yearly':
      $data['yearly_setpos'] = isset($rrule_array['BYSETPOS']) ? '1' : '0';
      if ($data['yearly_setpos'] === '0') {
        $data['bymonthday_year'] = isset($rrule_array['BYMONTHDAY']) ? $rrule_array['BYMONTHDAY'] : NULL;
      }
      else {
        $data['bysetpos_year'] = isset($rrule_array['BYSETPOS']) ? $rrule_array['BYSETPOS'] : NULL;
        $data['byday_year'] = isset($rrule_array['BYDAY']) ? $rrule_array['BYDAY'] : NULL;
      }
      $data['bymonth_year'] = isset($rrule_array['BYMONTH']) ? $rrule_array['BYMONTH'] : NULL;
      break;
  }

  return $data;
}
