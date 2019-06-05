<?php
/**
 * Menu callbacks for BackdropCMS.org Tugboat pages.
 */

/**
 * Menu callback; Create a demo given correct $_GET arguments.
 */
function borg_tugboat_demo_create_page() {
  // Disable caching on this page, needed for AJAX form submitting working with
  // the page cache.
  backdrop_page_is_cacheable(FALSE);
  return theme('borg_tugboat_demo_create_page', array(
    'form' => backdrop_get_form('borg_tugboat_create_demo_form'),
  ));
}

/**
 * Menu callback; Provide the user a link to their new demo page.
 */
function borg_tugboat_demo_done_page() {
  return theme('borg_tugboat_demo_done_page', array(
    'url' => isset($_GET['url']) ? $_GET['url'] : 'Unknown',
    'duration' => format_interval(config_get('borg_tugboat.settings', 'sandbox_lifetime')),
  ));
}

/**
 * Provide a form that creates a new sandbox.
 */
function borg_tugboat_create_demo_form($form, &$form_state) {
  form_load_include($form_state, 'inc', 'borg_tugboat', 'borg_tugboat.pages');

  // Tugboat repository ID, a 24 character hash.
  $form['repo'] = array(
    '#type' => 'value',
    '#value' => config_get('borg_tugboat.settings', 'repository_id'),
  );
  // Base preview branch/tag name.
  $form['ref'] = array(
    '#type' => 'value',
    '#value' => config_get('borg_tugboat.settings', 'repository_base'),
  );
  // Date added to the created preview for reference.
  $form['date'] = array(
    '#type' => 'value',
    '#value' => format_date(REQUEST_TIME, 'custom', 'Y-m-d'),
  );
  $form['create_sandbox'] = array(
    '#type' => 'submit',
    '#value' => t('Create demo sandbox'),
    '#validate' => array('borg_tugboat_create_demo_form_validate'),
    '#submit' => array('borg_tugboat_create_demo_form_submit'),
    '#attributes' => array('class' => array('button', 'button-large', 'demo')),
    '#ajax' => array(
      'progress' => array(
        'type' => 'throbber',
        'message' => t('Creating demo sandbox...'),
      ),
      'callback' => 'borg_tugboat_create_demo_form_ajax',
    ),
  );

  return $form;
}


/**
 * Check if the base preview exists.
 */
function borg_tugboat_create_demo_form_validate($form, &$form_state) {
  $repo = $form_state['values']['repo'];
  $ref = $form_state['values']['ref'];

  // Get a list of all current sandboxes to see if a designated base preview
  // exists for the given branch.
  $base_found = FALSE;
  $return_data = array();
  $error_string = '';
  _borg_tugboat_execute("ls previews repo=$repo", $return_data, $error_string);
  if ($error_string) {
    form_set_error('ref', t('A demo sandbox cannot be created at this time.'));
    watchdog('borg_tugboat', 'A sandbox failed to be created, error from Tugboat.qa: <pre>@error</pre>', array('@error' => $error_string));
    return;
  }

  foreach ($return_data as $preview) {
    // Only compare against base previews.
    if (!$preview['anchor']) {
      continue;
    }

    if ($preview['name'] === $ref) {
      // Store the Tugboat base preview ID for use in cloning.
      $form_state['values']['ref_id'] = $preview['id'];
      $base_found = TRUE;
    }
  }

  if (!$base_found) {
    form_set_error('branch', t('The tag or branch "@ref" could not be found for creating a demo sandbox.', array('@ref' => $ref)));
    watchdog('borg_tugboat', 'A sandbox with the ref "@ref" failed to be created because no base preview with that name could be found', array('@ref' => $ref));
  }
}

/**
 * Submit handler for borg_tugboat_create_demo_form().
 */
function borg_tugboat_create_demo_form_submit($form, &$form_state) {
  $ref = $form_state['values']['ref'];
  $repo = $form_state['values']['repo'];
  $date = $form_state['values']['date'];

  $return_data = array();
  $error_string = '';

  // For unknown reasons initial attempts to create a sandbox may fail. So try
  // creating a sandbox twice if the first attempt does not work.
  for ($attempt_number = 1; $attempt_number <= 2; $attempt_number++) {
    // Use the ref_id set in the validate handler above to create a clone.
    // "clone" approach, nearly instantaneous but an exact copy.
    if (isset($form_state['values']['ref_id'])) {
      $ref_id = $form_state['values']['ref_id'];
      $return_status = _borg_tugboat_execute("clone $ref_id label=one-click-demo--$date--$ref", $return_data, $error_string);
    }
    // "create preview" approach, slightly slower but more flexible. Note that
    // because we set ref_id above in the validate function, this should never
    // execute. It's left here for reference.
    else {
      $return_status = _borg_tugboat_execute("create preview label=one-click-demo--$date--$ref repo=$repo preview=$ref", $return_data, $error_string);
    }
    if ($return_status) {
      break;
    }
  }

  if (!$return_status) {
    backdrop_set_message(t('A demo sandbox could not be created at this time! Please try again later.'));
    if ($error_string) {
      watchdog('borg_tugboat', 'Failed to create sandbox. Error from Tugboat: <pre>@error</pre>', array('@error' => $error_string));
    }
    else {
      watchdog('borg_tugboat', 'Failed to create sandbox. No error data returned from Tugboat!');
    }
  }
  else {
    backdrop_set_message(t('New demo sandbox creation successful.'));
    $form_state['redirect'] = array('demo/created', array('query' => array(
      'url' => $return_data['url'],
    )));
  }
}

function borg_tugboat_create_demo_form_ajax($form, $form_state) {
  $commands = array();

  if (isset($form_state['redirect'])) {
    $url = url($form_state['redirect'][0], $form_state['redirect'][1]);
    $commands[] = ajax_command_redirect($url);
  }
  else {
    $commands[] = ajax_command_remove('.borg-demo-create-page .messages');
    $commands[] = ajax_command_prepend('.borg-demo-create-page', theme('status_messages'));
  }

  return array('#type' => 'ajax', '#commands' => $commands);
}
