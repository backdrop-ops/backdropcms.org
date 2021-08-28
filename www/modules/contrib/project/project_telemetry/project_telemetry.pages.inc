<?php
/**
 * @file
 * Menu callbacks that provide front-end pages for the Project Telemetry module.
 */

/**
 * Form callback; Settings form for enabling Telemetry on a project node.
 */
function project_telemetry_project_settings(array $form, array &$form_state, Node $node) {
  $form['help'] = array(
    '#type' => 'help',
    '#help' => t('This form is used to enable Telemetry data collection on this project. Projects must first be vetted to ensure no personal information is being sent.')
  );

  $form['project_nid'] = array(
    '#type' => 'value',
    '#value' => $node->nid,
  );

  $form['enabled'] = array(
    '#title' => t('Telemetry data collection'),
    '#type' => 'radios',
    '#options' => array(
      '0' => t('Disabled'),
      '1' => t('Enabled'),
    ),
    '#default_value' => (int) $node->project['telemetry']['enabled'],
    '#description' => t('Allows data sent by installations of this project to be recorded on this server. If disabled, any data sent will be ignored and not saved to the database.'),
  );

  $form['allowed_values'] = array(
    '#tree' => TRUE,
    '#type' => 'container',
    '#theme' => 'project_telemetry_allowed_values',
    '#id' => 'telemetry-allowed-values',
    '#description' => t('Allowed values must be set to determine which values sent by a module should be retained. Keys are those specified in hook_telemetry_data() by the sending module.'),
  );

  $current_values = isset($form_state['values']['allowed_values']) ? $form_state['values']['allowed_values'] : $node->project['telemetry']['allowed_values'];

  // Add a blank row for adding new values.
  if (empty($current_values) || !empty($form_state['values']['_add_new'])) {
    $current_values['_new'] = array(
      'key' => '',
      'label' => '',
      'precision' => '',
    );
  }

  $position = 0;
  foreach ($current_values as $key => $value_settings) {
    $form['allowed_values'][$position]['key'] = array(
      '#title' => t('Key'),
      '#title_display' => 'invisible',
      '#type' => 'textfield',
      '#size' => 20,
      '#default_value' => isset($value_settings['key']) ? $value_settings['key'] : $key,
    );
    $form['allowed_values'][$position]['label'] = array(
      '#title' => t('Label'),
      '#title_display' => 'invisible',
      '#type' => 'textfield',
      '#size' => 20,
      '#default_value' => $value_settings['label'],
    );
    $form['allowed_values'][$position]['precision'] = array(
      '#title' => t('Aggregation precision'),
      '#title_display' => 'invisible',
      '#type' => 'select',
      '#options' => array(
        '' => t('No adjustment (non-numeric value)'),
        '0' => t('Integers (0, 1, 2, etc.)'),
        '1' => t('1 Decimal (1.10, 1.11, 1.12, etc.)'),
        '2' => t('2 Decimals (1.10.1, 1.10.2, 1.10.3, etc.)'),
      ),
      '#default_value' => $value_settings['precision'],
    );
    $position++;
  }
  $form['allowed_values']['_add'] = array(
    '#type' => 'submit',
    '#value' => t('New allowed value'),
    '#parents' => array('_add'),
    '#weight' => 100,
    '#submit' => array('project_telemetry_project_settings_add'),
    '#ajax' => array(
      'wrapper' => 'telemetry-allowed-values',
      'callback' => 'project_telemetry_project_settings_ajax',
    ),
  );

  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save settings'),
  );

  return $form;
}

/**
 * Submit handler for project_telemetry_project_settings() to add a new value.
 */
function project_telemetry_project_settings_add(&$form, &$form_state) {
  $form_state['values']['_add_new'] = TRUE;
  $form_state['rebuild'] = TRUE;
}

/**
 * AJAX submit handler for project_telemetry_project_settings().
 */
function project_telemetry_project_settings_ajax(&$form, &$form_state) {
  return $form['allowed_values'];
}

/**
 * Submit handler for project_telemetry_project_settings().
 */
function project_telemetry_project_settings_submit(&$form, &$form_state) {
  $node = node_load($form_state['values']['project_nid']);
  $node->project['telemetry']['enabled'] = (bool) $form_state['values']['enabled'];

  // Convert the "key" value to an actual array key.
  $allowed_values = array();
  foreach ($form_state['values']['allowed_values'] as $value_settings) {
    $key = $value_settings['key'];
    unset($value_settings['key']);
    if (strlen($key)) {
      $allowed_values[$key] = $value_settings;
    }
  }
  $node->project['telemetry']['allowed_values'] = $allowed_values;

  $node->save();

  backdrop_set_message(t('Project telemetry settings saved.'));
}

/**
 * Menu callback; Display Telemetry data for a node.
 *
 * @param Node $node
 *   Either a Project or Project Release node.
 */
function project_telemetry_page(Node $node) {
  if ($node->type === 'project') {
    $output = project_telemetry_project_page($node);
  }
  else {
    $output = project_telemetry_release_page($node);
  }
  return $output;
}

/**
 * Menu callback; Display Telemetry data for a project node.
 *
 * @param Node $node
 *   The project node object for which we're displaying Telemetry data.
 */
function project_telemetry_project_page(Node $node) {
  $telemetry_settings = $node->project['telemetry'];
  $telemetry_aggregates = _project_telemetry_load_aggregates($node->nid, $telemetry_settings['allowed_values']);

  $page = array();

  // Even if data is collected, disable access to this page if Telemetry is
  // turned off on this project.
  if (!$telemetry_settings['enabled']) {
    $page['disabled'] = array(
      '#markup' => t('Telemetry is not currently enabled for this project. Enable data storage for this project under <a href="!settings">Telemetry settings</a>.', array(
        '!settings' => url('node/' . $node->nid . '/edit/telemetry'),
      )),
    );
    return $page;
  }

  $help = '';
  $help .= '<p>' . t('Below is Telemetry data collected for this project.') . '</p>';

  $page['help'] = array(
    '#type' => 'help',
    '#markup' => $help
  );

  $header = array(
    t('Value'),
    t('Count'),
    t('Percentage'),
  );

  foreach ($telemetry_aggregates as $item_key => $aggregate_values) {
    $rows = array();
    foreach ($aggregate_values as $rounded_number => $aggregate_value) {
      $row = array();
      $row[] = check_plain($rounded_number);
      $row[] = $aggregate_value['count'];
      $row[] = $aggregate_value['percentage'] * 100 . '%';;
      $rows[] = $row;
    }
    $page['items'][$item_key] = array(
      '#theme' => 'table',
      '#header' => $header,
      '#caption' => $telemetry_settings['allowed_values'][$item_key]['label'],
      '#rows' => $rows,
      '#sticky' => FALSE,
    );
  }

  return $page;

}

/**
 * Menu callback; Display Telemetry data for a project release node.
 *
 * @param Node $node
 *   The project release node object for which we're displaying Telemetry data.
 */
function project_telemetry_release_page(Node $node) {
  // TODO: Per-release Telemetry data?
  $page = 'Per release Telemetry not yet implemented';
  return $page;
}

/**
 * Load aggregated data for a project node.
 *
 * @param int $project_nid
 *   The Node ID of the project for which to load data.
 *
 * @param array $allowed_values
 *   A list of values that should be allowed to be retrieved. Keyed by the
 *   item_key. With values:
 *     - label: The human-readable label of the allowed value.
 *     - precision: An integer indicating the number of decimals by which to
 *       group items.
 */
function _project_telemetry_load_aggregates($project_nid, $allowed_values) {
  $result = db_query('SELECT item_key, item_value, COUNT(item_value) AS item_count FROM {project_telemetry_raw} WHERE project_nid = :project_nid GROUP BY item_key, item_value', array(':project_nid' => $project_nid));

  // Track the total results in all categories.
  $totals = array();

  // Initialize the returned results in a defined order.
  $rounded_values = array();
  foreach ($allowed_values as $key => $allowed_value) {
    $rounded_values[$key] = array();
  }

  foreach ($result as $row) {
    $key = $row->item_key;
    $count = $row->item_count;
    $rounded_value = _project_telemetry_round_version_string($row->item_value, $allowed_values[$key]['precision']);

    // Increment the count for each rounded group of version numbers.
    if (!isset($rounded_values[$key][$rounded_value]['count'])) {
      $rounded_values[$key][$rounded_value]['count'] = 0;
    }
    $rounded_values[$key][$rounded_value]['count'] += $count;

    // Increment the total number of values for a particular data key.
    $totals[$key] = isset($totals[$key]) ? $totals[$key] : 0;
    $totals[$key] += $count;
  }

  // Populate percentages.
  foreach ($rounded_values as $key => $item_aggregates) {
    foreach ($item_aggregates as $rounded_value => $item_aggregate) {
      $rounded_values[$key][$rounded_value]['percentage'] = round($item_aggregate['count'] / $totals[$key], 2);
    }
  }

  // Sort by percentages.
  foreach ($rounded_values as $key => $item_aggregates) {
    backdrop_sort($rounded_values[$key], array('percentage'));
  }

  return $rounded_values;
}

/**
 * Truncate a version to the indicated number of decimals.
 *
 * Note this does not perform real "rounding", it only truncates values to the
 * indicated decimal. It all takes non-float numbers, such as "2.10.12". If
 * rounded to one decimal, this example would become "2.10".
 *
 * @param string $version_string
 *   A version string
 * @param int $precision
 *   The number of decimals to retain. If NULL, no modification will be done.
 */
function _project_telemetry_round_version_string($version_string, $precision) {
  if (strlen($precision)) {
    $decimal_location = 0;
    for ($n = 0; $n <= $precision; $n++) {
      $decimal_location = strpos($version_string, '.', $decimal_location + 1);
    }
    if ($decimal_location !== FALSE) {
      $version_string = substr($version_string, 0, $decimal_location);
    };
  }

  return $version_string;
}
