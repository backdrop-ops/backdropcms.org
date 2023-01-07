<?php
/**
 * @file
 * Default theme implementation to display Views JSON Simile/Exhibit output.
 *
 * Available variables:
 * - $view: The View object.
 * - $rows: Hierachial array of key=>value pairs to convert to JSON
 * - $options: Array of options for this style
 *
 * @see template_preprocess_views_views_json_style_exhibit()
 *
 * @ingroup views_templates
 */

$jsonp_prefix = $options['jsonp_prefix'];

if ($view->override_path) {
  // We're inside a live preview where the JSON is pretty-printed.
  $json = _views_json_encode_formatted($rows, $options);
  if ($jsonp_prefix) {
    $json = "$jsonp_prefix($json)";
  }
  print "<code>$json</code>";
}
else {
  $json = json_encode($rows);
  if ($jsonp_prefix) {
    $json = "$jsonp_prefix($json)";
  }
  if ($options['using_views_api_mode']) {
    // We're in Views API mode.
    print $json;
  }
  else {
    // We want to send the JSON as a server response so switch the content
    // type and stop further processing of the page.
    $content_type = ($options['content_type'] == 'default') ? 'application/json' : $options['content_type'];
    backdrop_add_http_header("Content-Type", $content_type . '; charset=utf-8');
    print $json;
    backdrop_page_footer();
    exit;
  }
}
