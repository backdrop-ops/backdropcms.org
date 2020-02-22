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
  require_once backdrop_get_path('module', 'tugboat') . '/unirest-php/src/Unirest.php';
  // Prevent issues with duplicate response headers by disabling FOLLOWLOCATION.
  Unirest\Request::curlOpt(CURLOPT_FOLLOWLOCATION, FALSE);

  $config = config('tugboat.settings');
  $preview_id = $config->get('preview_id');

  // Clone the provided preview.
  $clone_response = Unirest\Request::post(TUGBOAT_API_URL . 'previews/' . $preview_id . '/clone', tugboat_api_headers());
  $cloned = tugboat_api_get_response($clone_response, 'form', 'create_preview_site');

  if ($cloned) {
    // Keep checking the job until it's complete and the preview's ready.
    $job_response = Unirest\Request::get(TUGBOAT_API_URL . 'jobs/' . $cloned->job, tugboat_api_headers());
    if ($job_response->code == 200) {
      do {
        sleep($job_response->headers['Retry-After']);
        $job_response = Unirest\Request::get(TUGBOAT_API_URL . 'jobs/' . $cloned->job, tugboat_api_headers());
      }
      while ($job_response->code == 200);
    }

    $job = tugboat_api_get_response($job_response, 'form', 'create_preview_site');
    if ($job) {
      // Get new preview.
      $preview_response = Unirest\Request::get(TUGBOAT_API_URL . 'previews/' . $job->preview, tugboat_api_headers());
      $preview = tugboat_api_get_response($preview_response, 'form', 'create_preview_site');

      // Add the new preview to config.
      $config->set('previews.' . $preview->id, $preview->url);
      $config->save();

      // Redirect to the 'Preview ready' page.
      backdrop_set_message(t('New site created successfully!'));
      $form_state['redirect'] = $config->get('page_url') . '/' . $preview->id;
    }
  }
}

/**
 * AJAX handler for tugboat_preview_create_form().
 */
function tugboat_preview_create_form_ajax($form, $form_state) {
  $commands = array();

  if (isset($form_state['redirect'])) {
    $commands[] = ajax_command_redirect($form_state['redirect']);
  }
  else {
    // Allow messages to be displayed immediately.
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
  $previews = $config->get('previews');

  // Only show valid preview IDs.
  if (isset($previews[$preview_id])) {
    return theme('tugboat_preview_ready_page', array(
      'url' => $previews[$preview_id],
      'age' => format_interval($config->get('preview_site_age')),
    ));
  }

  backdrop_set_title(t('Page not found'));
  return system_404();
}
