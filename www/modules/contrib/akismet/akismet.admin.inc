<?php

/**
 * @file
 * Administrative page callbacks for Akismet module.
 */

/**
 * Checks the configuration status on Akismet administration pages.
 *
 * On all Akismet administration pages, check the module configuration and
 * display the corresponding requirements error, if invalid.
 */
function akismet_admin_site_status($force = FALSE, $update = FALSE) {
  $status = _akismet_status($force, $update);
  if (empty($_POST) && !$status['isVerified']) {
    // Fetch and display requirements error message, without re-checking.
    module_load_install('akismet');
    $requirements = akismet_requirements('runtime', FALSE);
    if (isset($requirements['akismet']['description'])) {
      backdrop_set_message($requirements['akismet']['description'], 'error');
    }
  }
  return $status;
}

/**
 * Menu callback; Displays a list of forms configured for Akismet.
 */
function akismet_admin_form_list() {
  akismet_admin_site_status();
  _akismet_testing_mode_warning();

  // Reset the cached list of protected forms.
  akismet_form_cache(TRUE);

  $modes = array(
    AKISMET_MODE_ANALYSIS => t('Text analysis'),
  );

  $header = array(
    t('Form'),
    t('Protection mode'),
    array('data' => t('Operations'), 'colspan' => 2),
  );
  $result = db_query('SELECT form_id FROM {akismet_form}')->fetchCol();
  $forms = array();
  $module_info = system_get_info('module');
  foreach ($result as $form_id) {
    $forms[$form_id] = akismet_form_load($form_id);
    // system_get_info() only supports enabled modules. Default to the module's
    // machine name in case it is disabled.
    $module = $forms[$form_id]['module'];
    if (!isset($module_info[$module])) {
      $module_info[$module]['name'] = $module;
    }
    $forms[$form_id]['title'] = t('!module: !form-title', array(
      '!form-title' => $forms[$form_id]['title'],
      '!module' => t($module_info[$module]['name']),
    ));
  }

  // Sort forms by title (including module name prefix).
  backdrop_sort($forms, array('title' => SORT_STRING));

  $rows = array();
  foreach ($forms as $form_id => $akismet_form) {
    $row_attributes = array();
    $row = array();
    $row[] = $akismet_form['title'];
    if (isset($modes[$akismet_form['mode']])) {
      if ($akismet_form['mode'] == AKISMET_MODE_ANALYSIS) {
        // @todo Output unsure mode in summary listing.
        $row[] = t('!protection-mode (@discard)', array(
          '!protection-mode' => $modes[$akismet_form['mode']],
          '@discard' => $akismet_form['discard'] ? t('discard') : t('retain'),
        ));
      }
      else {
        $row[] = $modes[$akismet_form['mode']];
      }
    }
    else {
      $row[] = t('- orphan -');
    }
    if (empty($akismet_form['orphan'])) {
      $row[] = array('data' => array(
        '#type' => 'link',
        '#title' => t('Configure'),
        '#href' => 'admin/config/content/akismet/manage/' . $form_id,
      ));
    }
    else {
      $row[] = '';
      $row_attributes['class'] = array('error');
      backdrop_set_message(t("%module module's %form_id form no longer exists.", array(
        '%form_id' => $form_id,
        '%module' => isset($module_info[$akismet_form['module']]['name']) ? t($module_info[$akismet_form['module']]['name']) : $akismet_form['module'],
      )), 'warning');
    }
    $row[] = array('data' => array(
      '#type' => 'link',
      '#title' => t('Unprotect'),
      '#href' => 'admin/config/content/akismet/unprotect/' . $form_id,
    ));

    $rows[] = $row_attributes + array('data' => $row);
  }

  $build['forms'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => l(t('Add form'), 'admin/config/content/akismet/add'),
  );

  return $build;
}

/**
 * Return registered forms as an array suitable for a 'checkboxes' form element #options property.
 */
function akismet_admin_form_options() {
  // Retrieve all registered forms.
  $form_list = akismet_form_list();

  // Remove already configured form ids.
  $result = db_query('SELECT form_id FROM {akismet_form}')->fetchCol();
  foreach ($result as $form_id) {
    unset($form_list[$form_id]);
  }
  // If all registered forms are configured already, output a message, and
  // redirect the user back to overview.
  if (empty($form_list)) {
    backdrop_set_message(t('All available forms are protected already.'));
    backdrop_goto('admin/config/content/akismet');
  }

  // Load module information.
  $module_info = system_get_info('module');

  // Transform form information into an associative array suitable for #options.
  $options = array();
  foreach ($form_list as $form_id => $info) {
    // system_get_info() only supports enabled modules. Default to the module's
    // machine name in case it is disabled.
    $module = $info['module'];
    if (!isset($module_info[$module])) {
      $module_info[$module]['name'] = $module;
    }
    $options[$form_id] = t('!module: !form-title', array(
      '!form-title' => $info['title'],
      '!module' => t($module_info[$module]['name']),
    ));
  }
  // Sort form options by title.
  asort($options);

  return $options;
}

/**
 * Form builder; Configure Akismet protection for a form.
 */
function akismet_admin_configure_form($form, &$form_state, $akismet_form = NULL) {
  // If no $akismet_form was passed, then we are adding a new form configuration.
  if (!isset($akismet_form)) {
    if (!isset($form_state['storage']['akismet_form'])) {
      $form_state['storage']['step'] = 'select';
    }
    else {
      $form_state['storage']['step'] = 'configure';
      $akismet_form = $form_state['storage']['akismet_form'];
    }
  }
  // When adding a new form configuration, passing form_id via path argument.
  elseif (is_string($akismet_form)) {
    $akismet_form = akismet_form_new($akismet_form);
    $form_state['storage']['step'] = 'configure';
    $form_state['storage']['akismet_form'] = $akismet_form;
  }
  // Otherwise, we are editing an existing form configuration.
  else {
    $form_state['storage']['step'] = 'configure';
    $form_state['storage']['akismet_form'] = $akismet_form;
  }

  $form['#tree'] = TRUE;
  $form['actions'] = array(
    '#type' => 'actions',
  );
  $form['#attached'] = array(
    'js' => array(
      backdrop_get_path('module', 'akismet') . '/akismet.admin.js',
    ),
  );

  switch ($form_state['storage']['step']) {
    case 'select':
      $form['akismet']['form_id'] = array(
        '#type' => 'select',
        '#title' => t('Form'),
        '#options' => akismet_admin_form_options(),
        '#required' => TRUE,
      );
      $form['actions']['next'] = array(
        '#type' => 'submit',
        '#value' => t('Next'),
        '#submit' => array('akismet_admin_configure_form_next_submit'),
      );
      break;

    case 'configure':
      backdrop_set_title(t('Configure %form-title protection', array('%form-title' => $akismet_form['title'])), PASS_THROUGH);
      $recommended = t('recommended');

      $form['akismet']['form_id'] = array(
        '#type' => 'value',
        '#value' => $akismet_form['form_id'],
      );

      $form['akismet']['mode']['#value'] = AKISMET_MODE_ANALYSIS;

      // Form elements defined by hook_akismet_form_info() use the
      // 'parent][child' syntax, which Form API also uses internally for
      // form_set_error(), and which allows us to recurse into nested fields
      // during processing of submitted form values. However, since we are using
      // those keys also as internal values to configure the fields to use for
      // textual analysis, we need to encode them. Otherwise, a nested field key
      // would result in the following checkbox attribute:
      //   '#name' => 'akismet[enabled_fields][parent][child]'
      // This would lead to a form validation error, because it is a valid key.
      // By encoding them, we prevent this from happening:
      //   '#name' => 'akismet[enabled_fields][parent%5D%5Bchild]'
      $elements = array();
      foreach ($akismet_form['elements'] as $key => $value) {
        $elements[rawurlencode($key)] = $value;
      }
      $enabled_fields = array();
      foreach ($akismet_form['enabled_fields'] as $value) {
        $enabled_fields[] = rawurlencode($value);
      }
      $form['akismet']['enabled_fields'] = array(
        '#type' => 'checkboxes',
        '#title' => t('Text fields to analyze'),
        '#options' => $elements,
        '#default_value' => $enabled_fields,
        '#description' => t('Only enable fields that accept text (not numbers). Omit fields that contain sensitive data (e.g., credit card numbers) or computed/auto-generated values, as well as author information fields (e.g., name, e-mail).'),
        '#access' => !empty($akismet_form['elements']),
        '#states' => array(
          'visible' => array(
            ':input[name="akismet[mode]"]' => array('value' => (string) AKISMET_MODE_ANALYSIS),
          ),
        ),
      );

      $form['akismet']['unsure'] = array(
        '#type' => 'radios',
        // TODO: Rework this UI to better fit Akismet's wording: https://blog.akismet.com/2014/04/23/theres-a-ninja-in-your-akismet/
        '#title' => t('When Akismet thinks a post is spam, but isn\'t sure'),
        '#default_value' => $akismet_form['unsure'],
        '#options' => array(
          'moderate' => t('Retain the post for manual moderation'),
          'accept' => t('Accept the post'),
          'discard' => t('Disallow the post'),
        ),
        '#required' => TRUE,
      );
      // Only possible for forms supporting moderation of unpublished posts.
      $form['akismet']['unsure']['moderate']['#access'] = !empty($akismet_form['moderation callback']);

      $form['akismet']['discard'] = array(
        '#type' => 'radios',
        '#title' => t('When Akismet is sure a post is spam'),
        '#default_value' => $akismet_form['discard'],
        '#options' => array(
          1 => t('!option <em class="akismet-recommended">(!recommended)</em>', array(
            '!option' => t('Discard the post'),
            '!recommended' => $recommended,
          )),
          0 => t('!option <em class="akismet-recommended">(!recommended)</em>', array(
            '!option' => t('Retain the post for manual moderation'),
            '!recommended' => $recommended,
          )),
        ),
        '#required' => TRUE,
        // Only possible for forms supporting moderation of unpublished posts.
        '#access' => !empty($akismet_form['moderation callback']),
        // Only possible for forms protected via text analysis.
      );

      $all_permissions = array();
      foreach (module_implements('permission') as $module) {
        if ($module_permissions = module_invoke($module, 'permission')) {
          foreach ($module_permissions as &$info) {
            $info += array('module' => $module);
          }
          $all_permissions += $module_permissions;
        }
      }
      // Prepend Akismet's global permission to the list.
      array_unshift($akismet_form['bypass access'], 'bypass akismet protection');
      $permissions = array();
      foreach ($akismet_form['bypass access'] as $permission) {
        // @todo D7: Array keys are used as CSS class for the link list item,
        //   but are not sanitized: http://drupal.org/node/98696
        $permissions[backdrop_html_class($permission)] = array(
          'title' => $all_permissions[$permission]['title'],
          'href' => 'admin/config/people/permissions',
          'fragment' => 'module-' . $all_permissions[$permission]['module'],
          'html' => TRUE,
        );
      }
      $form['akismet']['bypass']['#markup'] = t('Note that the spam check is skipped for users having any of these permissions: !permission-list', array(
        '!permission-list' => theme('links', array(
          'links' => $permissions,
          // @todo D7: Something went entirely wrong: system.menus.css makes ANY
          //   ul.links appear as if it would have the .inline CSS class.
          'attributes' => array(),
        )),
      ));


      $form['actions']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save'),
      );
      break;
  }

  $form['actions']['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'admin/config/content/akismet',
  );

  return $form;
}

/**
 * Form submit handler for 'Next' button on Akismet form configuration form.
 */
function akismet_admin_configure_form_next_submit($form, &$form_state) {
  $form_id = $form_state['values']['akismet']['form_id'];
  $form_state['redirect'] = $_GET['q'] . '/' . $form_id;
}

/**
 * Form submit handler for akismet_admin_configure_form().
 */
function akismet_admin_configure_form_submit($form, &$form_state) {
  $akismet_form = $form_state['values']['akismet'];
  // Merge in form information from $form_state.
  $akismet_form += $form_state['storage']['akismet_form'];

  // Only store a list of enabled textual analysis checks.
  $akismet_form['checks'] = array_keys(array_filter($akismet_form['checks']));
  // Prepare selected fields for storage.
  $enabled_fields = array();
  foreach (array_keys(array_filter($akismet_form['enabled_fields'])) as $field) {
    $enabled_fields[] = rawurldecode($field);
  }
  $akismet_form['enabled_fields'] = $enabled_fields;

  $status = akismet_form_save($akismet_form);
  if ($status === SAVED_NEW) {
    backdrop_set_message(t('The form protection has been added.'));
  }
  else {
    backdrop_set_message(t('The form protection has been updated.'));
  }

  $form_state['redirect'] = 'admin/config/content/akismet';
}

/**
 * Form builder; Remove Akismet protection from a form.
 */
function akismet_admin_unprotect_form($form, &$form_state, $akismet_form) {
  $form['#tree'] = TRUE;
  $form['form'] = array(
    '#type' => 'item',
    '#title' => t('Form'),
    '#markup' => $akismet_form['title'],
  );
  $form['akismet']['form_id'] = array(
    '#type' => 'value',
    '#value' => $akismet_form['form_id'],
  );

  return confirm_form($form,
    t('Are you sure you want to unprotect this form?'),
    'admin/config/content/akismet',
    t('Akismet will no longer protect this form from spam.')
  );
}

/**
 * Form submit handler for akismet_admin_unprotect_form().
 */
function akismet_admin_unprotect_form_submit($form, &$form_state) {
  akismet_form_delete($form_state['values']['akismet']['form_id']);

  backdrop_set_message(t('The form protection has been removed.'));

  $form_state['redirect'] = 'admin/config/content/akismet';
}

/**
 * Form builder; Global Akismet settings form.
 *
 * This form does not validate the API key, since the fallback method still
 * needs to be able to be reconfigured in case Akismet services are down.
 * Akismet::verifyKey would invalidate the keys and throw an error; hence,
 * _akismet_fallback() would invoke form_set_error(), effectively preventing this
 * form from submitting.
 *
 * @todo Implement proper form validation now that akismet() no longer triggers
 *   the fallback mode.
 */
function akismet_admin_settings($form, &$form_state) {
  $config = config('akismet.settings');
  $check = empty($_POST);
  $status = akismet_admin_site_status($check);
  if ($check && $status['isVerified'] && $config->get('akismet_testing_mode') == 0) {
    backdrop_set_message(t('Akismet verified your key. The service is operating correctly.'));
  }

  $form['#config'] = 'akismet.settings';
  // The key is not #required, so that users can install this module and
  // configure it later.
  $form['akismet_api_key'] = array(
    '#type' => 'textfield',
    '#title' => t('API key'),
    '#default_value' => $config->get('akismet_api_key'),
    '#element_validate' => array('akismet_admin_settings_validate_key'),
    '#description' => t('To obtain an API key, <a href="@signup-url">sign up</a> or log in to your <a href="@account-url">account</a>, add a subscription for this site, and copy the key into the field below.', array(
      '@signup-url' => 'https://akismet.com/signup',
      '@account-url' => 'https://akismet.com/account',
    )),
  );
  $form['access-keys']['ping_holder'] = array(
    '#type' => 'fieldset',
    '#title' => t('Test connection'),
    '#title_display' => 'invisible',
    '#states' => array(
      'visible' => array(
        ':input[name="akismet_api_key"]' => array('!value' => ''),
      ),
    ),
    'ping_action' => array(
      '#type' => 'button',
      '#value' => t('Test Akismet connection'),
      '#submit' => array('akismet_admin_ping_submit'),
      '#ajax' => array(
        'callback' => 'akismet_admin_verify_key_ajax_callback',
        'wrapper' => 'akismet-ping-response',
        'method' => 'replace',
        'effect' => 'fade',
      ),
    ),
    'help' => array(
      '#type' => 'container',
      '#attributes' => array(
        'class' => array('description'),
      ),
      'helptext' => array(
        '#markup' => t('Be sure that you have checked "Log all Akismet messages" in the "Advanced" configuration in order to review full API responses.'),
      ),
    ),
    'akismet_ping_replace' => array(
      '#type' => 'container',
      '#attributes' => array(
        'id' => 'akismet-ping-response',
      ),
      'response' => array(
        '#markup' => isset($form_state['storage']['akismet_ping']) ? $form_state['storage']['akismet_ping'] : '',
      ),
    )
  );
  $form['akismet_fallback'] = array(
    '#type' => 'radios',
    '#title' => t('When the Akismet service is unavailable'),
    '#default_value' => $config->get('akismet_fallback'),
    '#options' => array(
      AKISMET_FALLBACK_ACCEPT => t('Accept all form submissions'),
      AKISMET_FALLBACK_BLOCK => t('Block all form submissions'),
    ),
  );

  $form['akismet_privacy_link'] = array(
    '#type' => 'checkbox',
    '#title' => t("Show a link to Akismet's privacy policy"),
    '#return_value' => 1,
    '#default_value' => $config->get('akismet_privacy_link'),
    '#description' => t('If you disable this option, you should inform visitors about the privacy of their data through other means.'),
  );

  $form['akismet_testing_mode'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable testing mode'),
    '#default_value' => $config->get('akismet_testing_mode'),
    '#description' => t('Do not enable this option if this site is publicly accessible.'),
  );

  $form['akismet_advanced'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced configuration'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  // Lower severity numbers indicate a high severity level.
  $min_severity = $config->get('akismet_log_minimum_severity');
  $form['akismet_advanced']['akismet_log_minimum_severity'] = array(
    '#type' => 'radios',
    '#title' => t('Akismet logging level warning'),
    '#options' => array(
      WATCHDOG_WARNING => t('Only log warnings and errors'),
      WATCHDOG_DEBUG => t('Log all Akismet messages'),
    ),
    '#default_value' => $min_severity <= WATCHDOG_WARNING ? WATCHDOG_WARNING : WATCHDOG_DEBUG,
  );
  $form['akismet_advanced']['akismet_connection_timeout'] = array(
    '#type' => 'textfield',
    '#title' => t('Time-out when attempting to contact Akismet servers'),
    '#description' => t('This is the length of time that a call to Akismet will wait before timing out.'),
    '#default_value' => $config->get('akismet_connection_timeout'),
    '#size' => 5,
    '#field_suffix' => t('seconds'),
    '#required' => TRUE,
  );
  $form['#submit'][] = 'akismet_admin_settings_prepare_submit';
  $form = system_settings_form($form);
  $form['#submit'][] = 'akismet_admin_settings_submit';
  return $form;
}

/**
 * Element validation handler for API key text fields.
 */
function akismet_admin_settings_validate_key($element, &$form_state) {
  if ($element['#value'] !== '') {
    // Remove any leading/trailing white-space and override submitted value.
    $element['#value'] = trim($element['#value']);
    form_set_value($element, $element['#value'], $form_state);
  }
}

/**
 * Form submission handler for global settings form.
 */
function akismet_admin_settings_prepare_submit($form, &$form_state) {
  // If the minimum log severity checkbox was disabled (no input), convert
  // 0 into WATCHDOG_DEBUG.
  if (!isset($form_state['input']['akismet_log_minimum_severity'])) {
    $form_state['values']['akismet_log_minimum_severity'] = WATCHDOG_DEBUG;
  }
}

/**
 * Form submission handler for global settings form.
 */
function akismet_admin_settings_submit($form, &$form_state) {
  // Clear the local Akismet status cache.
  _akismet_status(TRUE, TRUE);
}

/**
 * Form submit handler for the ping button.
 */
function akismet_admin_verify_key_ajax_callback($form, &$form_state) {
  $akismet = akismet();
  $success = $akismet->verifyKey($form_state['values']['akismet_api_key']);
  if ($success) {
    $log_message = t('Akismet API key is valid.');
  }
  else {
    $log_message = t('Key verification failed.  Please check your <a href="!log_url">Recent log messages</a> for more information.', array(
      '!log_url' => url('admin/reports/dblog'),
    ));
  }
  $form_state['storage']['akismet_ping'] = $form['access-keys']['ping_holder']['akismet_ping_replace']['response']['#markup'] = $log_message;
  return $form['access-keys']['ping_holder']['akismet_ping_replace'];
}
