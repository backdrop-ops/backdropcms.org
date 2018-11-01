<?php

/**
 * @file
 * Akismet client class for Backdrop.
 */

/**
 * Backdrop Akismet client implementation.
 */
class AkismetBackdrop extends Akismet {

  /**
   * Overrides the connection timeout based on module configuration.
   *
   * @see Akismet::__construct().
   */
  public function __construct() {
    parent::__construct();
    $this->requestTimeout = config_get('akismet.settings', 'akismet_connection_timeout');
  }

  /**
   * Mapping of configuration names to Backdrop variables.
   *
   * @see Akismet::loadConfiguration()
   */
  public $configuration_map = array(
    'key' => 'akismet_api_key',
  );

  /**
   * Implements Akismet::loadConfiguration().
   */
  public function loadConfiguration($name) {
    $name = $this->configuration_map[$name];
    return config_get('akismet.settings', $name);
  }

  /**
   * Implements Akismet::saveConfiguration().
   */
  public function saveConfiguration($name, $value) {
    // Set local variable.
    if (property_exists('AkismetBackdrop', $name)) {
      $this->{$name} = $value;
    }
    // Persist in Backdrop.
    $name = $this->configuration_map[$name];
    config_set('akismet.settings', $name, $value);
  }

  /**
   * Implements Akismet::deleteConfiguration().
   */
  public function deleteConfiguration($name) {
    $name = $this->configuration_map[$name];
    $config = config('akismet.settings');
    $config->clear($name);
  }

  /**
   * Implements Akismet::getUserAgentInformation().
   */
  public function getUserAgentInformation() {
    // Retrieve Backdrop distribution and installation profile information.
    $profile = backdrop_get_profile();
    $profile_info = system_get_info('module', $profile) + array(
      'distribution_name' => 'Backdrop',
      'version' => VERSION,
    );

    // Retrieve Akismet module information.
    $akismet_info = system_get_info('module', 'akismet');
    if (empty($akismet_info['version'])) {
      // Manually build a module version string for repository checkouts.
      $akismet_info['version'] = BACKDROP_CORE_COMPATIBILITY . '-2.x-dev';
    }

    $data = array(
      'platformName' => $profile_info['distribution_name'],
      'platformVersion' => $profile_info['version'],
      'clientName' => $akismet_info['name'],
      'clientVersion' => $akismet_info['version'],
    );
    return $data;
  }

  /**
   * Overrides Akismet::getSiteURL().
   */
  public function getSiteURL() {
    return $GLOBALS['base_url'];
  }

  /**
   * Overrides Akismet::writeLog().
   */
  function writeLog() {
    foreach ($this->log as $entry) {
      $entry['Request: ' . $entry['request']] = !empty($entry['data']) ? $entry['data'] : NULL;
      unset($entry['request'], $entry['data']);

      $entry['Request headers:'] = $entry['headers'];
      unset($entry['headers']);

      $entry['Response: ' . $entry['response_code'] . ' ' . $entry['response_message'] . ' (' . number_format($entry['response_time'], 3) . 's)'] = $entry['response'];
      unset($entry['response'], $entry['response_code'], $entry['response_message'], $entry['response_time']);

      // The client class contains the logic for recovering from certain errors,
      // and log messages are only written after that happened. Therefore, we
      // can normalize the severity of all log entries to the overall success or
      // failure of the attempted request.
      // @see Akismet::query()
      akismet_log($entry, $this->lastResponse->isError ? WATCHDOG_ERROR : NULL);
    }

    // After writing log messages, empty the log.
    $this->purgeLog();
  }

  /**
   * Implements Akismet::request().
   */
  protected function request($method, $server, $path, $query = NULL, array $headers = array()) {
    $request = array(
      'method' => $method,
      'headers' => $headers,
      'timeout' => $this->requestTimeout,
    );
    if (isset($query)) {
      if ($method == 'GET') {
        $path .= '?' . $query;
      }
      elseif ($method == 'POST') {
        $request['data'] = $query;
      }
    }

    $dhr = backdrop_http_request($server . '/' . $path, $request);
    // @todo Core: Ensure that $dhr->code is an integer.
    $dhr->code = (int) $dhr->code;
    // @todo Core: Any other code than 200 is interpreted as error.
    if ($dhr->code >= 200 && $dhr->code < 300) {
      unset($dhr->error);
    }
    // @todo Core: data property is not assigned if there is no response body.
    if (!isset($dhr->data)) {
      $dhr->data = NULL;
    }
    // @todo Core: Timeout produces a bogus non-negative status code.
    // @see http://drupal.org/node/1246376
    if ($dhr->code === 1) {
      $dhr->code = -1;
    }

    $response = (object) array(
      'code' => $dhr->code,
      'message' => isset($dhr->error) ? $dhr->error : NULL,
      'headers' => isset($dhr->headers) ? $dhr->headers : array(),
      'body' => $dhr->data,
    );
    return $response;
  }
}

/**
 * Backdrop Akismet client implementation using test parameter.
 */
class AkismetBackdropTest extends AkismetBackdrop {

  /**
   * {@inheritdoc}
   */
  public function query($method, $path, $data, $authenticate = TRUE) {
    $data['is_test'] = 1;
    return parent::query($method, $path, $data, $authenticate);
  }
}

class AkismetBackdropTestLocal extends AkismetBackdropTest {

  /**
   * Overrides AkismetBackdropTest::__construct().
   */
  function __construct() {
    // Replace server/endpoint with our local fake server.
    list(, $server) = explode('://', $GLOBALS['base_url'], 2);
    $this->server = 'http://' . $server . '/akismet-test';

    parent::__construct();
  }


  function getAkismetURL($authenticate) {
    if ($authenticate) {
      return $this->server . '/' . $this->key;
    }
    else {
      return $this->server . '/unauthenticated';
    }
  }

  /**
   * Overrides AkismetBackdrop::request().
   *
   * Passes-through SimpleTest assertion HTTP headers from child-child-site and
   * triggers errors to make them appear in parent site (where tests are ran).
   *
   * @see http://drupal.org/node/875342
   */
  protected function request($method, $server, $path, $query = NULL, array $headers = array()) {
    $response = parent::request($method, $server, $path, $query, $headers);
    $keys = preg_grep('@^x-backdrop-assertion-@', array_keys($response->headers));
    foreach ($keys as $key) {
      $header = $response->headers[$key];
      $header = unserialize(urldecode($header));
      $message = strtr('%type: !message in %function (line %line of %file).', array(
        '%type' => $header[1],
        '!message' => $header[0],
        '%function' => $header[2]['function'],
        '%line' => $header[2]['line'],
        '%file' => $header[2]['file'],
      ));
      trigger_error($message, E_USER_ERROR);
    }
    return $response;
  }
}

/**
 * Backdrop Akismet client implementation using an invalid server.
 */
class AkismetBackdropTestInvalid extends AkismetBackdropTestLocal {

  private $currentAttempt = 0;

  private $originalServer;

  /**
   * Overrides AkismetBackdropTest::__construct().
   */
  function __construct() {
    $this->configuration_map['server'] = 'akismet_test_invalid_api_endpoint';
    parent::__construct();
    $this->originalServer = $this->server;

    $this->saveConfiguration('server', 'fake-host');
  }

  /**
   * Overrides Akismet::query().
   */
  public function query($method, $path, $data, $authenticate = TRUE) {
    $this->currentAttempt = 0;
    return parent::query($method, $path, $data, $authenticate);
  }

  /**
   * Overrides Akismet::handleRequest().
   *
   * Akismet::$server is replaced with an invalid server, so all requests will
   * result in a network error. However, if the 'akismet_testing_server_failover'
   * variable is set to TRUE, then the last request attempt will succeed.
   */
  protected function handleRequest($method, $server, $path, $data) {
    $this->currentAttempt++;

    if (config_get('akismet.settings', 'akismet_testing_server_failover') == TRUE && $this->currentAttempt == $this->requestMaxAttempts) {
      // Prior to PHP 5.3, there is no late static binding, so there is no way
      // to access the original value of AkismetBackdropTest::$server.
      $server = strtr($server, array($this->server => $this->originalServer));
    }
    return parent::handleRequest($method, $server, $path, $data);
  }
}
