<?php

/**
 * (Page callback)
 *
 * @return string
 */
function civicrm_setup_page() {
  $coreUrl = dirname(file_create_url(backdrop_get_path('module', 'civicrm')));
  $corePath = dirname(__DIR__);
  $classLoader = implode(DIRECTORY_SEPARATOR, [$corePath, 'CRM', 'Core', 'ClassLoader.php']);

  if (file_exists($classLoader)) {
    require_once $classLoader;
    CRM_Core_ClassLoader::singleton()->register();
    \Civi\Setup::assertProtocolCompatibility(1.0);
    \Civi\Setup::init([
      // This is just enough information to get going. Backdrop.civi-setup.php does more scanning.
      'cms' => 'Backdrop',
      'srcPath' => $corePath,
    ], NULL, _civicrm_setup_logger());
    $ctrl = \Civi\Setup::instance()->createController()->getCtrl();
    $ctrl->setUrls(array(
      'ctrl' => url('civicrm'),
      'res' => $coreUrl . '/setup/res/',
      'jquery.js' => $coreUrl . '/bower_components/jquery/dist/jquery.min.js',
      'font-awesome.css' => $coreUrl . '/bower_components/font-awesome/css/font-awesome.min.css',
      // Not used? 'finished' => url('civicrm/dashboard', ['query' => ['reset' => 1],]),
    ));
    // return _civicrm_setup_runCtrl($ctrl);
    \Civi\Setup\BasicRunner::run($ctrl);
    exit();
  }
  else {
    backdrop_set_message(t('Cannot perform setup for CiviCRM. The file "@file" is missing.', [
      '@file' => $classLoader,
    ]), 'error');
    return '';
  }
}

/**
 * @return NULL|\Psr\Log\LoggerInterface
 */
function _civicrm_setup_logger() {
  return NULL;
  //$logger = new class extends \Psr\Log\AbstractLogger {
  //  public function log($level, $message, array $context = array()) {
  //    echo "<tt>$message</tt><br>\n";
  //  }
  //};
  //return $logger;
}

///**
// * @param \Civi\Setup\UI\SetupControllerInterface $ctrl
// * @return mixed
// */
//function _civicrm_setup_runCtrl($ctrl) {
//  \Civi\Setup::assertProtocolCompatibility(1.1);
//  $method = $_SERVER['REQUEST_METHOD'];
//
//  /** @var \Civi\Setup\UI\SetupResponse $response */
//  $response = $ctrl->run($method, ($method === 'GET' ? $_GET : $_POST));
//
//  if ($response->isComplete) {
//    \Civi\Setup\BasicRunner::send($ctrl, $response);
//    exit();
//  }
//
//  foreach ($response->headers as $k => $v) {
//    backdrop_add_http_header($k, $v, TRUE);
//  }
//
//  foreach ($response->assets as $asset) {
//    switch ($asset['type']) {
//      case 'script-url':
//        backdrop_add_js($asset['url'], 'external');
//        break;
//
//      case 'script-code':
//        backdrop_add_js($asset['code'], 'inline');
//        break;
//
//      case 'style-url':
//        backdrop_add_css($asset['url'], 'external');
//        break;
//
//      default:
//        throw new \Exception("Unrecognized page asset: " . $asset['type']);
//    }
//  }
//
//  return $response->body;
//}
