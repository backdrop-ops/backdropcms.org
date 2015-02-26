<?php
/**
 * Implements hook_preprocess_page().
 *
 * Adds an action on 404 pages to create a redirect.
 */
function redirect_preprocess_page(&$variables) {
  dpm('kong');
  if (redirect_is_current_page_404() && user_access('administer redirects')) {
    dpm(user_access('administer redirects'));
//if (!isset($page['content']['system_main']['actions'])) {
    $page['content']['system_main']['actions'] = array(
      '#theme' => 'links',
      '#links' => array(),
      '#attributes' => array('class' => array('action-links')),
      '#weight' => -100,
    );
//}
// We cannot simply use current_path() because if a 404 path is set, then
// that value overrides whatever is in $_GET['q']. The
// backdrop_deliver_html_page() function thankfully puts the original current
// path into $_GET['destination'].
    $destination = backdrop_get_destination();
    $page['content']['system_main']['actions']['#links']['add_redirect'] = array(
      'title' => t('Add URL redirect from this page to another location'),
      'href' => 'admin/config/search/redirect/add',
      'query' => array('source' => $destination['destination']) + backdrop_get_destination(),
    );
    dpm($page);
  }
}