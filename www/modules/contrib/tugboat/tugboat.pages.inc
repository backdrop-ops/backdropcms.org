<?php
/**
 * Menu callbacks for Tugboat module pages.
 */

/**
 * Page for creating a preview site.
 */
function tugboat_preview_create_page() {
  backdrop_add_css(backdrop_get_path('module', 'tugboat') . '/css/tugboat.css');

  // Disable caching on this page; needed for AJAX form submitting working with
  // the page cache.
  backdrop_page_is_cacheable(FALSE);

  return theme('tugboat_preview_create_page', array(
    'form' => backdrop_get_form('tugboat_preview_create_form'),
  ));
}

/**
 * Provide a form that creates a new preview site.
 */
function tugboat_preview_create_form($form, &$form_state) {
  return array(
    'create_preview_site' => array(
      '#type' => 'submit',
      '#value' => check_plain(config_get('tugboat.settings', 'create_page_button')),
      '#submit' => array('tugboat_preview_create_form_submit'),
      '#ajax' => array(
        'progress' => array(
          'type' => 'throbber',
          'message' => t('Please wait...'),
        ),
        'callback' => 'tugboat_preview_create_form_ajax',
      ),
    ),
  );
}

/**
 * Submit handler for tugboat_preview_create_form().
 */
function tugboat_preview_create_form_submit($form, &$form_state) {
  $config = config('tugboat.settings');
  $build_method = $config->get('build_method');
  $repository_id = $config->get('repository_id');
  $git_ref = $config->get('git_ref');
  $preview_id = $config->get('preview_id');
  $error_message = FALSE;

  // Determine the expiration time.
  $preview_site_age = $config->get('preview_site_age');
  $expire_date = date_create();
  $expire_date->modify("+$preview_site_age seconds");
  $expire_date->setTimezone(timezone_open('UTC'));
  $expire_string = $expire_date->format('c');

  // Build a new preview.
  if ($build_method == 'create') {
    $built = tugboat_api_curl('/previews', array('method' => 'post', 'body' => array('ref' => $git_ref, 'repo' => $repository_id, 'expires' => $expire_string)));
  }
  elseif ($build_method == 'clone') {
    $built = tugboat_api_curl('/previews/' . $preview_id . '/clone', array('method' => 'post', 'body' => array('expires' => $expire_string)));
  }

  if ($built['code'] >= 400) {
    $error_message = $built['body']['message'];
  }
  else {
    // Keep checking the job until it's complete and the preview's ready.
    $job = tugboat_api_curl('/jobs/' . $built['body']['job']);
    while ($job['code'] == 200) {
      sleep($job['header']['Retry-After']);
      $job = tugboat_api_curl('/jobs/' . $built['body']['job']);
    }

    if ($job['code'] >= 400) {
      $error_message = $job['body']['message'];
    }
    else {
      // Get new preview.
      $preview = tugboat_api_curl('/previews/' . $job['body']['preview']);

      if ($preview['code'] >= 400) {
        $error_message = $preview['body']['message'];
      }
      else {
        // Add the new preview to config.
        $previews = state_get('tugboat_previews');
        $previews[$preview['body']['id']] = $preview['body']['url'];
        state_set('tugboat_previews', $previews);

        // Redirect to the 'Preview ready' page.
        backdrop_set_message(t('New site created successfully!'));
        $form_state['redirect'] = $config->get('page_url') . '/' . $preview['body']['id'];
      }
    }
  }

  // Display error message.
  if ($error_message) {
    backdrop_set_message(t('An error occurred: @message', array('@message' => $error_message)), 'error');

    // Reload the page so the message can be displayed.
    $form_state['redirect'] = $config->get('page_url');
  }
}

/**
 * AJAX handler for tugboat_preview_create_form().
 */
function tugboat_preview_create_form_ajax($form, $form_state) {
  $commands = array();

  if (isset($form_state['redirect'])) {
    $commands[] = ajax_command_redirect(url($form_state['redirect']));
  }
  else {
    // Allow messages to be displayed immediately.
    // TODO: This doesn't seem to work...
    $commands[] = ajax_command_remove('.tugboat-create-page .messages');
    $commands[] = ajax_command_prepend('.tugboat-create-page', theme('status_messages'));
  }

  return array(
    '#type' => 'ajax',
    '#commands' => $commands,
  );
}

/**
 * Page for linking to the preview site.
 */
function tugboat_preview_ready_page($preview_id) {
  backdrop_add_css(backdrop_get_path('module', 'tugboat') . '/css/tugboat.css');

  $config = config('tugboat.settings');
  $previews = state_get('tugboat_previews', array());

  // Only show valid preview IDs.
  if (isset($previews[$preview_id])) {
    return theme('tugboat_preview_ready_page', array(
      'preview_id' => $preview_id,
      'url' => $previews[$preview_id],
      'age' => format_interval($config->get('preview_site_age')),
    ));
  }

  backdrop_set_title(t('Page not found'));
  return system_404();
}
