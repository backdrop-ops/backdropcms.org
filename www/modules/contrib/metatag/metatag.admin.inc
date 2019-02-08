<?php
/**
 * @file
 * Administration page callbacks for the metatag module.
 */

function _metatag_config_overview_indent($text, $instance) {
  $parents = metatag_config_get_parent_instances($instance);
  array_shift($parents);

  // Add indentation to the leading cell.
  if (!empty($parents)) {
    $prefix = array_fill(0, count($parents), '<div class="indent">');
    $suffix = array_fill(0, count($parents), '</div>');
    $text = implode('', $prefix) . $text . implode('', $suffix);
  }

  return $text;
}

function metatag_config_overview() {
  $metatags = metatag_get_info('tags');

  $names = config_get_names_with_prefix('metatag.instance');
  $config_files = config_load_multiple($names);
  $configs = array();
  foreach ($config_files as $config) {
    $configs[$config['instance']] = $config;
  }

  uksort($configs, '_metatag_config_instance_sort');

  $rows = array();
  foreach ($configs as $config) {
    $row = array();

    // Style disabled configurations differently.
    if (isset($config['disabled']) && $config['disabled']) {
      $row['class'][] = 'disabled';
    }

    if (isset($config['label'])) {
      $label = check_plain($config['label']);
    }
    else {
      // @todo remove this? Replace with "Unknown?"
      $label = check_plain(metatag_config_instance_label($config['instance']));
    }

    $details  = '<div class="metatag-config-label collapsed">';
    $details .= '  <a href="#" class="toggle-details">' . $label . '</a>';
    $details .= '</div>';
    $details .= '<div class="metatag-config-details js-hide">';

    $inherits = array();
    $parents = metatag_config_get_parent_instances($config['instance']);
    array_shift($parents);
    foreach (array_reverse($parents) as $parent) {
      if (!isset($configs[$parent])) {
        $rows[$parent] = array(
          _metatag_config_overview_indent('<div class="metatag-config-label">' . check_plain(metatag_config_instance_label($parent)) . '</div>', $parent),
          '',
        );
      }
      else {
        $inherits[$parent] = metatag_config_instance_label($parent);
        if (!empty($configs[$parent]['disabled'])) {
          $inherits[$parent] .= ' ' . t('(disabled)');
        }
      }
    }

    // Show how this config inherits from its parents.
    if (!empty($inherits)) {
      $details .= '<div class="inheritance"><p>' . t('Inherits meta tags from: @parents', array('@parents' => implode(', ', $inherits))) . '</p></div>';
    }

    // Add a summary of the configuration's defaults.
    $summary = array();
    foreach ($config['config'] as $metatag => $data) {
      // Skip meta tags that were disabled.
      if (empty($metatags[$metatag])) {
        continue;
      }
      // Skip meta tags that were not specifically defined.
      if (empty($data['value'])) {
        continue;
      }
      $summary[] = array(
        check_plain($metatags[$metatag]['label']) . ':',
        check_plain(metatag_get_value($metatag, $data, array('raw' => TRUE))),
      );
    }
    if (!empty($summary)) {
      $details .= theme('table', array(
        'rows' => $summary,
        'attributes' => array('class' => array('metatag-value-summary')),
      ));
    }
    else {
      $details .= '<p class="warning">No overridden default meta tags</p>';
      $row['class'][] = 'warning';
    }

    // Close the details div
    $details .= '</div>';

    // Add indentation to the leading cell based on how many parents the config has.
    $details = _metatag_config_overview_indent($details, $config['instance']);

    $row['data']['details'] = $details;

    $operations = array();
    if (metatag_config_access('configure', $config)) {
      $operations['edit'] = array(
        'title' => t('Configure'),
        'href' => 'admin/config/metadata/metatags/config/' . $config['instance'],
      );
      if ($config['storage'] == METATAG_STORAGE_DEFAULT) {
        $operations['edit']['title'] = t('Override');
      }
    }
    if (metatag_config_access('enable', $config)) {
      $operations['enable'] = array(
        'title' => t('Enable'),
        'href' => 'admin/config/metadata/metatags/config/' . $config['instance'] . '/enable',
        'query' => array(
          'token' => backdrop_get_token('enable-' . $config['instance']),
        ) + backdrop_get_destination(),
      );
    }
    if (metatag_config_access('disable', $config)) {
      $operations['disable'] = array(
        'title' => t('Disable'),
        'href' => 'admin/config/metadata/metatags/config/' . $config['instance'] . '/disable',
        'query' => array(
          'token' => backdrop_get_token('disable-' . $config['instance']),
        ) + backdrop_get_destination(),
      );
    }
    if (metatag_config_access('revert', $config)) {
      $operations['revert'] = array(
        'title' => t('Revert'),
        'href' => 'admin/config/metadata/metatags/config/' . $config['instance'] . '/revert',
      );
    }
    if (metatag_config_access('delete', $config)) {
      $operations['delete'] = array(
        'title' => t('Delete'),
        'href' => 'admin/config/metadata/metatags/config/' . $config['instance'] . '/delete',
      );
    }
    if (module_exists('i18n_string') && !$settings->get('i18n_disabled', FALSE)) {
      $operations['translate'] = array(
        'title' => t('Translate'),
        'href' => 'admin/config/metadata/metatags/config/' . $config['instance'] . '/translate',
      );
    }
    $row['data']['operations'] = array(
      'data' => array(
        '#theme' => 'links',
        '#links' => $operations,
        '#attributes' => array('class' => array('links', 'inline')),
      ),
    );
    $rows[$config['instance']] = $row;
  }

  $suffix = '<small>' . t('Items marked as "Unknown" will not be used. They are likely for types of pages that have been deleted or disabled on the <a href="@url">Settings page</a>.', array('@url' => url('admin/config/metadata/metatags/settings'))) . '</small>';

  $build['config_table'] = array(
    '#theme' => 'table',
    '#header' => array(
      'type' => t('Type'),
      'operations' => t('Operations'),
    ),
    '#rows' => $rows,
    '#empty' => t('No meta tag defaults available yet.'),
    '#attributes' => array(
      'class' => array('metatag-config-overview'),
    ),
    '#attached' => array(
      'js' => array(
        backdrop_get_path('module', 'metatag') . '/js/metatag.admin.js',
      ),
      'css' => array(
        backdrop_get_path('module', 'metatag') . '/css/metatag.admin.css',
      ),
    ),
    '#suffix' => $suffix,
  );

  return $build;
}

/**
 * Build an FAPI #options array for the instance select field.
 */
function _metatag_config_instance_get_available_options() {
  $options = array();
  $instances = metatag_config_instance_info();

  foreach ($instances as $instance => $instance_info) {
    if (metatag_config_load($instance)) {
      continue;
    }

    $parents = metatag_config_get_parent_instances($instance, FALSE);
    array_shift($parents);
    if (!empty($parents)) {
      $parent = reset($parents);
      $parent_label = isset($instances[$parent]['label']) ? $instances[$parent]['label'] : t('Unknown');
      if (!isset($options[$parent_label])) {
        $options[$parent_label] = array();
        if (!metatag_config_load($parent)) {
          $options[$parent_label][$parent] = t('All');
        }
      }
      $options[$parent_label][$instance] = $instance_info['label'];
      unset($options[$parent]);
    }
    else {
      $options[$instance] = $instance_info['label'];
    }
  }

  return $options;
}

function metatag_config_add_form($form, &$form_state) {
  $form['instance'] = array(
    '#type' => 'select',
    '#title' => t('Type'),
    '#description' => t('Select the type of default meta tags you would like to add.'),
    '#options' => _metatag_config_instance_get_available_options(),
    '#required' => TRUE,
  );
  $form['config'] = array(
    '#type' => 'value',
    '#value' => array(),
  );

  $form['actions']['#type'] = 'actions';
  $form['actions']['save'] = array(
    '#type' => 'submit',
    '#value' => t('Add and configure'),
  );
  $form['actions']['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => isset($_GET['destination']) ? $_GET['destination'] : 'admin/config/metadata/metatags',
  );

  return $form;
}

function metatag_config_add_form_submit($form, &$form_state) {
  form_state_values_clean($form_state);
  $config = $form_state['values'];
  metatag_config_save($config);
  $form_state['redirect'] = 'admin/config/metadata/metatags/config/' . $config['instance'];
}

function metatag_config_edit_form($form, &$form_state, $config) {
  $form['instance'] = array(
    '#type' => 'value',
    '#value' => $config['instance'],
  );

  $contexts = explode(':', $config['instance']);
  $options['context'] = $contexts[0];
  if ($contexts[0] != 'global') {
    $info = entity_get_info($contexts[0]);
    $options['token types'] = array($info['token type']);
    // Allow the defined tokens to be modified.
    backdrop_alter('metatag_token_types', $options);
  }

  // Ensure that this configuration is properly compared to its parent 'default'
  // configuration values.
  if (count($contexts) > 1) {
    // If the config is something like 'node:article' or 'taxonomy_term:tags'
    // then the parent default config is 'node' or 'taxonomy_term'.
    $default_instance = $contexts;
    array_pop($default_instance);
    $default_instance = implode(':', $default_instance);
    $options['defaults'] = metatag_config_load($default_instance);
  }
  elseif ($contexts[0] != 'global') {
    // If the config is something like 'node' or 'taxonomy_term' then the
    // parent default config is 'global'.
    $options['defaults'] = metatag_config_load('global');
  }
  else {
    // If the config is 'global' than there are no parent defaults.
    $options['defaults'] = array();
  }

  metatag_metatags_form($form, $config['instance'], $config['config'], $options);
  $form['metatags']['#type'] = 'container';

  $form['actions']['#type'] = 'actions';
  $form['actions']['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );
  $form['actions']['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => isset($_GET['destination']) ? $_GET['destination'] : 'admin/config/metadata/metatags',
  );

  $form['#submit'][] = 'metatag_config_edit_form_submit';
  return $form;
}

function metatag_config_edit_form_submit($form, &$form_state) {
  // Build the configuration object and save it.
  form_state_values_clean($form_state);
  $config = $form_state['values'];
  $config['config'] = $config['metatags'][LANGUAGE_NONE];
  unset($config['metatags']);
  metatag_config_save($config);

  $label = metatag_config_instance_label($config['instance']);
  backdrop_set_message(t('The meta tag defaults for @label have been saved.', array('@label' => $label)));

  $form_state['redirect'] = 'admin/config/metadata/metatags';
}

function metatag_config_enable($data) {
  if (!isset($_GET['token']) || !backdrop_valid_token($_GET['token'], 'enable-' . $data['instance'])) {
    return MENU_ACCESS_DENIED;
  }

  $name = str_replace(':', '.', $data['instance']);
  $config = config('metatag.instance.' . $name);
  $config->set('disabled', FALSE);
  $config->save();

  $label = metatag_config_instance_label($data['instance']);
  backdrop_set_message(t('The meta tag defaults for @label have been enabled.', array('@label' => $label)));
  backdrop_goto();
}

function metatag_config_disable($data) {
  if (!isset($_GET['token']) || !backdrop_valid_token($_GET['token'], 'disable-' . $data['instance'])) {
    return MENU_ACCESS_DENIED;
  }

  $name = str_replace(':', '.', $data['instance']);
  $config = config('metatag.instance.' . $name);
  $config->set('disabled', TRUE);
  $config->save();

  $label = metatag_config_instance_label($data['instance']);
  backdrop_set_message(t('The meta tag defaults for @label have been disabled.', array('@label' => $label)));
  backdrop_goto();
}

function metatag_config_delete_form($form, &$form_state, $config) {
  $form['instance'] = array('#type' => 'value', '#value' => $config['instance']);
  $form['metatag_action'] = array('#type' => 'value', '#value' => '');

  $label = metatag_config_instance_label($config['instance']);
  if (metatag_config_access('delete', $config)) {
    $title = t('Are you sure you want to delete the meta tag defaults for @label?', array('@label' => $label));
    $form['metatag_action']['#value'] = 'delete';
  }
  else {
    $title = t('Are you sure you want to revert the meta tag defaults for @label?', array('@label' => $label));
    $form['metatag_action']['#value'] = 'revert';
  }

  return confirm_form(
    $form,
    $title,
    'admin/config/metadata/metatags',
    t('This action cannot be undone.')
  );
}

function metatag_config_delete_form_submit($form, &$form_state) {
  $config = metatag_config_load($form_state['values']['instance']);
  metatag_delete_config($config);

  $label = metatag_config_instance_label($config['instance']);
  if ($form['metatag_action']['#value'] == 'delete') {
    $title = t('The meta tag defaults for @label have been deleted.', array('@label' => $label));
  }
  else {
    // Put back the default config.
    config_install_default_config('metatag', 'metatag.instance.' . $form_state['values']['instance']);
    $title = t('The meta tag defaults for @label have been reverted.', array('@label' => $label));
  }

  $form_state['redirect'] = 'admin/config/metadata/metatags';
}

/**
 * Form constructor: Reverts to default metatags.
 *
 * @see metatag_bulk_revert_form_submit()
 *
 * @ingroup forms
 */
function metatag_bulk_revert_form() {
  // Get the list of entity:bundle options
  $options = array();
  foreach (entity_get_info() as $entity_type => $entity_info) {
    foreach (array_keys($entity_info['bundles']) as $bundle) {
      if (metatag_entity_supports_metatags($entity_type, $bundle)) {
        $options[$entity_type . ':' . $bundle] = $entity_info['label'] . ': ' . $entity_info['bundles'][$bundle]['label'];
      }
    }
  }

  $form['update'] = array(
    '#type' => 'checkboxes',
    '#required' => TRUE,
    '#title' => t('Remove meta tags for all content of these types:'),
    '#options' => $options,
    '#default_value' => array(),
  );

  $metatags = metatag_get_info();
  $options = array();
  foreach ($metatags['tags'] as $tag_name => $tag) {
    $options[$tag_name] = t('@group_label: @tag_label', array(
      '@group_label' => $metatags['groups'][$tag['group']]['label'],
      '@tag_label' => $tag['label'],
    ));
  }

  if (count($options) > 0) {
    $form['tags'] = array(
      '#type' => 'checkboxes',
      '#required' => FALSE,
      '#title' => t('Only revert these tags:'),
      '#description' => t('If no options are selected, all tags will be reverted.'),
      '#options' => $options,
    );
  }

  if (module_exists('translation')) {
    $languages = language_list();
    $options = array(
      LANGUAGE_NONE => t('Language neutral'),
    );
    foreach ($languages as $language) {
      $options[$language->langcode] = $language->name;
    }

    $form['languages'] = array(
      '#type' => 'checkboxes',
      '#required' => FALSE,
      '#title' => t('Only revert tags for these languages:'),
      '#description' => t('If no options are selected, all languages will be reverted.'),
      '#options' => $options,
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Revert'),
    '#attributes' => array(
      'class' => array('button-primary'),
    ),
  );

  return $form;
}

/**
 * Form submit handler for metatag reset bulk revert form.
 *
 * @see metatag_batch_revert_form()
 * @see metatag_bulk_revert_batch_finished()
 */
function metatag_bulk_revert_form_submit($form, &$form_state) {
  $batch = array(
    'title' => t('Bulk updating metatags'),
    'operations' => array(),
    'finished' => 'metatag_bulk_revert_batch_finished',
    'file' => backdrop_get_path('module', 'metatag') . '/metatag.admin.inc',
  );

  $tags = isset($form_state['values']['tags']) ? array_filter($form_state['values']['tags']) : array();
  $languages = isset($form_state['values']['languages']) ? array_filter($form_state['values']['languages']) : array();

  // Set a batch operation per entity:bundle.
  foreach (array_filter($form_state['values']['update']) as $option) {
    list($entity_type, $bundle) = explode(':', $option);
    $batch['operations'][] = array('metatag_bulk_revert_batch_operation', array($entity_type, $bundle, $tags, $languages));
  }

  batch_set($batch);
}

/**
 * Batch callback: delete custom metatags for selected bundles.
 */
function metatag_bulk_revert_batch_operation($entity_type, $bundle, $tags, $languages, &$context) {
  if (!isset($context['sandbox']['current'])) {
    $context['sandbox']['count'] = 0;
    $context['sandbox']['current'] = 0;
  }

  // Query the selected entity table.
  $entity_info = entity_get_info($entity_type);
  $query = new EntityFieldQuery();
  $query->entityCondition('entity_type', $entity_type)
    ->propertyCondition($entity_info['entity keys']['id'], $context['sandbox']['current'], '>')
    ->propertyOrderBy($entity_info['entity keys']['id']);
  if ($entity_type != 'user') {
    /**
     * Entities which do not define a bundle such as User fail returning no results.
     * @see https://www.drupal.org/node/1054168#comment-5266208
     */
    $query->entityCondition('bundle', $bundle);
  }
  // Get the total amount of entities to process.
  if (!isset($context['sandbox']['total'])) {
    $context['sandbox']['total'] = $query->count()->execute();
    $query->count = FALSE;

    // If there are no bundles to revert, stop immediately.
    if (!$context['sandbox']['total']) {
      $context['finished'] = 1;
      return;
    }
  }

  // Process 25 entities per iteration.
  $query->range(0, 25);
  $result = $query->execute();
  $entity_ids = !empty($result[$entity_type]) ? array_keys($result[$entity_type]) : array();
  foreach ($entity_ids as $entity_id) {
    $metatags = metatag_metatags_load($entity_type, $entity_id);
    if (!empty($metatags)) {
      $reset = FALSE;
      if (empty($tags)) {
        // All tags should be reset, so we just delete any records from the db.
        $query = db_delete('metatag')
          ->condition('entity_type', $entity_type)
          ->condition('entity_id', $entity_id);
        if (!empty($languages)) {
          $query->condition('language', $languages, 'IN');
        }
        $query->execute();
        metatag_metatags_cache_clear($entity_type, $entity_id);
        $reset = TRUE;
      }
      else {
        // Iterate over tags and unset those, that we want to reset.
        $needs_reset = FALSE;
        foreach ($metatags as $metatags_language => $metatags_tags) {
          if (empty($languages) || in_array($metatags_language, $languages)) {
            foreach ($metatags_tags as $metatags_tag => $metatags_value) {
              if (in_array($metatags_tag, $tags)) {
                unset($metatags[$metatags_language][$metatags_tag]);
                $needs_reset = TRUE;
              }
            }
          }
        }
        // Save modified metatags.
        if ($needs_reset) {
          // We don't have a revision id, so we'll get the active one.
          // Unfortunately, the only way of getting the active revision ID is to
          // first load the entity, and then extract the ID. This is a bit
          // performance intensive, but it seems to be the only way of doing it.
          $entities = entity_load($entity_type, array($entity_id));
          if (!empty($entities[$entity_id])) {
            // We only care about the revision_id.
            list(, $revision_id, ) = entity_extract_ids($entity_type, $entities[$entity_id]);
          }
          metatag_metatags_save($entity_type, $entity_id, $revision_id, $metatags, $bundle);
          $reset = TRUE;
        }
      }
      if ($reset) {
        $context['results'][] = t('Reverted metatags for @bundle with id @id.', array(
          '@bundle' => $entity_type . ': ' . $bundle,
          '@id' => $entity_id,
        ));
      }
    }
  }

  $context['sandbox']['count'] += count($entity_ids);
  $context['sandbox']['current'] = max($entity_ids);

  if ($context['sandbox']['count'] != $context['sandbox']['total']) {
    $context['finished'] = $context['sandbox']['count'] / $context['sandbox']['total'];
  }
}

/**
 * Batch finished callback.
 */
function metatag_bulk_revert_batch_finished($success, $results, $operations) {
  if ($success) {
    if (!count($results)) {
      backdrop_set_message(t('No metatags were reverted.'));
    }
    else {
      $message = theme('item_list', array('items' => $results));
      backdrop_set_message($message);
    }
  }
  else {
    $error_operation = reset($operations);
    backdrop_set_message(t('An error occurred while processing @operation with arguments : @args',
      array('@operation' => $error_operation[0], '@args' => print_r($error_operation[0], TRUE))));
  }
}

/**
 * Misc settings page.
 */
function metatag_admin_settings_form() {
  $settings = config('metatag.settings');

  $form = array('#config' => 'metatag.settings');

  $form['#attached'] = array(
    'js' => array(
      backdrop_get_path('module', 'metatag') . '/js/metatag.admin.js',
    ),
    'css' => array(
      backdrop_get_path('module', 'metatag') . '/css/metatag.admin.css',
    ),
  );

  $form['entities'] = array(
    '#type' => 'fieldset',
    '#title' => t('Allow Meta tags'),
    '#description' => t('Specity where meta tag information can be defined.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  foreach (entity_get_info() as $entity_type => $entity_info) {
    // Only show entities that are capable of using meta tags.
    if (metatag_entity_type_is_suitable($entity_type, $entity_info)) {
      $entity_enabled = metatag_entity_supports_metatags($entity_type);
      $form['entities']['metatag_enable_' . $entity_type] = array(
        '#type' => 'checkbox',
        '#title' => t($entity_info['label']),
        '#default_value' => $entity_enabled,
      );

      // Some entities, e.g. User, (core) File, have a single bundle with the
      // same name as the entity, so only show the bundles list if there is
      // more than one of them and the bundle's name isn't the same as the
      // entity type's.
      if (!empty($entity_info['bundles'])) {
        foreach ($entity_info['bundles'] as $bundle_name => $bundle_info) {
          if (count($entity_info['bundles']) > 1 || $entity_type != $bundle_name) {
            // If the entity type was disabled, automatically enable the bundle.
            // This will have the effect that if the entity type is enabled in
            // the form then all of the bundles will then also be enabled. This
            // is safe to do because in the rest of the module the bundle will
            // be ignored if the entity is disabled.
            $form['entities']['metatag_enable_' . $entity_type . '__' . $bundle_name] = array(
              '#type' => 'checkbox',
              '#title' => t($bundle_info['label']),
              '#default_value' => metatag_entity_supports_metatags($entity_type, $bundle_name),
              '#attributes' => array(
                'class' => array('metatag-bundle-checkbox'),
              ),
              '#states' => array(
                'visible' => array(
                  ':input[name="metatag_enable_' . $entity_type . '"]' => array('checked' => TRUE),
                ),
              ),
            );
          }
        }
      }
    }
  }

  if (module_exists('i18n')) {
    $form['i18n'] = array(
      '#type' => 'fieldset',
      '#title' => t('Internationalization options'),
      '#description' => t('Additional options are available when using the <a href="@url">Internationalization module</a>.', array('@url' => 'https://www.drupal.org/project/i18n')),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
    $form['i18n']['i18n_disabled'] = array(
      '#title' => t('Disable i18n integration'),
      '#type' => 'checkbox',
      '#description' => t('If the i18n_string module is enabled but the site does not need to translate Metatag strings, the integration may be disabled to avoid a large volume of records being added to the translation system.'),
      '#default_value' => $settings->get('i18n_disabled'),
      '#disabled' => !module_exists('i18n_string'),
    );
    $form['i18n']['i18n_translate_output'] = array(
      '#title' => t('Translate output'),
      '#type' => 'checkbox',
      '#description' => t('Optionally translate the final page output.'),
      '#default_value' => $settings->get('i18n_translate_output'),
      '#disabled' => !module_exists('i18n_string'),
    );
    $form['i18n']['i18n_enable_watchdog'] = array(
      '#title' => t('Enable logging of translations to Watchdog'),
      '#type' => 'checkbox',
      '#description' => t('Optionally log to Watchdog any changes that happen.'),
      '#default_value' => $settings->get('i18n_enable_watchdog'),
      '#disabled' => !module_exists('i18n_string'),
    );
  }

  $form['advanced'] = array(
    '#type' => 'fieldset',
    '#title' => t('Advanced settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['advanced']['cache_output'] = array(
    '#type' => 'checkbox',
    '#title' => t('Cache meta tag output'),
    '#description' => t('When disabled, all meta tag output will be generated on request (disabling this setting may slow down your site).'),
    '#default_value' => $settings->get('cache_output'),
  );
  $form['advanced']['load_all_pages'] = array(
    '#type' => 'checkbox',
    '#title' => t('Always output meta tags, even if global default'),
    '#description' => t('When disabled, meta tags will be removed when values are not explicitly provided.'),
    '#default_value' => $settings->get('load_all_pages'),
  );
  $form['advanced']['leave_core_tags'] = array(
    '#type' => 'checkbox',
    '#title' => t('Always output canonical, shortlink, and generator meta tags'),
    '#description' => t('When disabled, the meta tags for canonical, shortlink, and generator will be removed when not provided.'),
    '#default_value' => $settings->get('leave_core_tags'),
  );
  $form['advanced']['entity_no_lang_default'] = array(
    '#type' => 'checkbox',
    '#title' => t('Only output meta tags that match the requested language'),
    '#description' => t('When disabled, meta tags for the <em>default language</em> will be output when no match is found for the requested language.'),
    '#default_value' => $settings->get('entity_no_lang_default'),
  );
  $form['advanced']['tag_admin_pages'] = array(
    '#type' => 'checkbox',
    '#title' => t('Output meta tags on administrative pages also'),
    '#description' => t('When disabled, meta tags will only be output on public-facing pages.'),
    '#default_value' => $settings->get('tag_admin_pages'),
  );
  $form['advanced']['extended_permissions'] = array(
    '#type' => 'checkbox',
    '#title' => t('Provide a permission for each individual meta tag'),
    '#default_value' => $settings->get('extended_permissions'),
  );
  $form['advanced']['token_sanitize'] = array(
    '#type' => 'checkbox',
    '#title' => t('Sanitize meta tag text during token replacement'),
    '#default_value' => $settings->get('token_sanitize'),
  );
  $form['advanced']['pager_string'] = array(
    '#title' => t('Replace the token [current-page:pager] with the following custom pager string:'),
    '#type' => 'textfield',
    '#description' => t('The value "PAGER" (must be in uppercase) will be replaced by the actual page count. Note: the pager will only output if the page number is 2 or above and the string "page=NUMBER" is in the URL.'),
    '#default_value' => $settings->get('pager_string'),
  );

  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  // Submit handler.
  $form['#submit'][] = 'metatag_admin_settings_form_submit';

  return system_settings_form($form);
}

/**
 * Validate handler for metatag_admin_settings_form().
 */
function metatag_admin_settings_form_validate(&$form, &$form_state) {
  foreach($form_state['values'] as $key => $value) {
    // If the setting is disabled...
    if (strstr($key, 'metatag_enable') && $value == 0) {
      // If the setting is for an entity...
      if (!strstr($key, '__')) {
        // Disable all its bundles.
        foreach ($form_state['values'] as $key2 => $value2) {
          if (strstr($key2, $key)) {
            $form_state['values'][$key2] = 0;
          }
        }
      }
    }
  }
}

/**
 * Submit handler for metatag_admin_settings_form().
 */
function metatag_admin_settings_form_submit(&$form, &$form_state) {
  cache_clear_all('entity_info:', 'cache', TRUE);
  cache_clear_all('*', 'cache_metatag', TRUE);
  backdrop_set_message(t('The Metatag cache has been cleared.'));
}
