<?php
/**
 * @file
 * Code for the "releases" subtab to the edit tab on project nodes.
 */

/**
 * Menu callback; Redirect the user to node/add/project-release/[nid].
 */
function project_release_add_node_redirect($project_node, $release_type) {
  backdrop_goto('node/add/' . str_replace('_', '-', $release_type) . '/' . $project_node->nid);
}

/**
 * Access callback; Check if the link for creating a release should be shown.
 */
function project_release_add_node_access($project_node, $release_type) {
  $project_node_type = node_type_get_type($project_node);
  $release_type_matches = $release_type === $project_node_type->settings['project_release_type'];
  return $release_type_matches && node_access('create', $release_type);
}

/**
 * Releases subtab for the project edit tab.
 */
function project_release_project_edit_releases($node) {
  project_project_set_breadcrumb($node);
  backdrop_set_title(check_plain($node->title));
  return backdrop_get_form('project_release_project_edit_form', $node);
}

function project_release_project_edit_form($form, $form_state, $node) {
  $api_versions = project_release_get_api_versions($node->type);
  // Since this form is used relatively infrequently, don't allow the js to be aggregated.
  backdrop_add_js(backdrop_get_path('module', 'project_release') . '/project_release.js', array('preprocess' => FALSE));

  // Get all the data about major versions for this project.
  $query = db_select('project_release_supported_versions', 'psrv');
  $query->join('project', 'p', 'psrv.nid = p.nid');
  $query->condition('psrv.nid', $node->nid);
  if ($api_versions) {
    $query->condition('psrv.version_api', $api_versions);
  }
  $query->fields('psrv', array('version_api', 'version_major', 'supported', 'recommended'));
  $query->fields('p', array('name'));
  $query->orderBy('psrv.version_api');
  $query->orderBy('psrv.version_major');
  $result = $query->execute();
  $data = array();
  foreach ($result as $object) {
    if (empty($data[$object->version_api])) {
      $data[$object->version_api] = array(
        'name' => $object->name,
        'majors' => array(),
      );
    }
    $data[$object->version_api]['majors'][$object->version_major] = array(
      '#supported' => $object->supported ? TRUE : FALSE,
      '#recommended' => $object->recommended ? TRUE : FALSE,
    );
  }
  if (empty($data)) {
    $form['help'] = array(
      '#markup' => '<p>' . t('No release versions for this project exist.') . '</p>',
    );
  }
  else {
    if (!empty($api_versions)) {
      $form['help'] = array(
        '#markup' => '<p>' . t('This page allows you to define which major versions are supported within this project and select which major version (if any) should be recommended for new users to download.') . '</p>',
      );
    }
    // Build the form elements for supported and recommended major versions.
    $form['version_api'] = array(
      '#tree' => TRUE,
      '#theme' => 'project_release_edit_table',
    );
    foreach ($data as $api_version => $api_data) {
      $form['version_api'][$api_version] = array(
        'version_major' => array(),
      );
      $recommended_version = -1;
      $recommended_options = array(-1 => t('None'));
      foreach ($api_data['majors'] as $major_version => $major_data) {
        if ($major_data['#recommended'] == TRUE) {
          $recommended_version = $major_version;
        }

        $recommended_options[$major_version] = $major_version;
        $current_recommended = project_release_get_current_recommended($node->nid, $api_version, $major_version);
        $form['version_api'][$api_version]['version_major'][$major_version] = array(
          'current' => array(
            '#markup' => (!empty($current_recommended)) ? check_plain($current_recommended->project_release['version']) : '',
          ),
          'supported' => array(
            '#type' => 'checkbox',
            '#title' => t('Supported'),
            '#default_value' => $major_data['#supported'],
            '#attributes' => array('class' => array('supported')),
          ),
        );
      }
      $form['version_api'][$api_version]['recommended'] = array(
        '#title' => t('Recommended major version'),
        '#type' => 'select',
        '#options' => $recommended_options,
        '#default_value' => $recommended_version,
        '#prefix' => '<div class="container-inline">',
        '#suffix' => '</div>',
        '#attributes' => array('class' => array('recommended')),
      );
    }

    $form['actions']['#type'] = 'actions';
    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
    );
  }

  return $form;
}

function theme_project_release_edit_table($variables) {
  $output = '';
  $element = $variables['element'];

  $header = array(
    t('Major version'),
    t('Current Release'),
    t('Supported'),
  );
  foreach (element_children($element) as $version_api) {
    $output .= '<h3>'. check_plain($version_api) .'</h3>';
    $rows = array();
    krsort($element[$version_api]['version_major']);
    foreach (element_children($element[$version_api]['version_major']) as $version_major) {
      $row = array();
      $row[] = $version_major;
      $row[] = backdrop_render($element[$version_api]['version_major'][$version_major]['current']);
      // We want to unset the titles for each element, since we already have
      // table headers to label each column.
      unset($element[$version_api]['version_major'][$version_major]['supported']['#title']);
      $row[] = backdrop_render($element[$version_api]['version_major'][$version_major]['supported']);
      $rows[] = $row;
    }
    // Finally, add a row for the currently recommended version.
    $row = array();
    $row[] = array(
      'data' => backdrop_render($element[$version_api]['recommended']),
      'colspan' => 5,
    );
    $rows[] = $row;
    $output .= theme('table', array('header' => $header, 'rows' => $rows));
  }

  return $output;
}

/**
 * Validates the project form regarding release-specific fields.
 *
 * @ingroup project_release_fapi
 * @see project_release_project_edit_releases()
 */
function project_release_project_edit_form_validate($form, &$form_state) {
  if (isset($form_state['values']['version_api'])) {
    foreach ($form_state['values']['version_api'] as $version_api => $api_info) {
      // First, we just iterate through to see if *any* majors are supported.
      foreach ($api_info['version_major'] as $version_major => $flags) {
        // At least 1 major is supported, so validate the settings.
        if ($flags['supported'] == FALSE && $api_info['recommended'] == $version_major) {
          form_error($form['version_api'][$version_api]['recommended'], t('You can not recommend a major version that is not supported for %version_api.', array('%version_api' => $version_api)));
        }
      }
    }
  }
}

/**
 * Submit handler when project admins use the releases subtab.
 *
 * @ingroup project_release_fapi
 * @see project_release_project_edit_releases()
 */
function project_release_project_edit_form_submit($form, &$form_state) {
  $nid = $form_state['build_info']['args'][0]->nid;

  if (!empty($form_state['values']['version_api'])) {
    $updates = array();
    foreach ($form_state['values']['version_api'] as $version_api => $values) {
      if (!empty($values['version_major'])) {
        foreach ($values['version_major'] as $version_major => $major_values) {
          $major_values['recommended'] = ($values['recommended'] == $version_major) ? 1 : 0;
          $existing = db_select('project_release_supported_versions', 'psrv')
            ->fields('psrv', array('supported', 'recommended'))
            ->condition('nid', $nid)
            ->condition('version_api', $version_api)
            ->condition('version_major', $version_major)
            ->execute()
            ->fetchAssoc();
          if ($existing) {
            if (count(array_diff_assoc($major_values, $existing)) > 0) {
              db_update('project_release_supported_versions')
                ->fields($major_values)
                ->condition('nid', $nid)
                ->condition('version_api', $version_api)
                ->condition('version_major', $version_major)
                ->execute();
              $updates[] = array('version_api' => $version_api, 'version_major' => $version_major);
            }
          }
          else {
            db_insert('project_release_supported_versions')
              ->fields(array(
                'nid' => $nid,
                'version_api' => $version_api,
                'version_major' => $version_major,
                'supported' => $major_values['supported'],
                'recommended' => $major_values['recommended'],
              ))
              ->execute();
            $updates[] = array('version_api' => $version_api, 'version_major' => $version_major);
          }
        }
      }
    }
  }
  db_update('node')
    ->fields(array('changed' => time()))
    ->condition('nid', $nid)
    ->execute();

  backdrop_set_message(t('Release settings have been saved.'));

  // Queue release XML generation for update status module.
  $queue = BackdropQueue::get('project_release_xml');
  $queue->createItem(array('project_nid' => $nid));

  // Since we've possibly adjusted the status/recommended releases, we need to
  // recompute the update status for each set of releases and re-generate the
  // download table.
  if (!empty($updates)) {
    backdrop_register_shutdown_function('project_release_compute_update_status_multiple', $nid, $updates);
  }
}
