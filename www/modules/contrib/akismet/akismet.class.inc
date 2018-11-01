<?php

abstract class Akismet {
  /**
   * The Akismet API version, used in HTTP requests.
   */
  const API_VERSION = '1.1';

  /**
   * Network communication failure code: Server could not be reached.
   *
   * @see AkismetNetworkException
   */
  const NETWORK_ERROR = 900;

  /**
   * Client communication failure code: Bad request.
   *
   * @see AkismetBadRequestException
   */
  const REQUEST_ERROR = 400;

  /**
   * Client communication failure code: Authentication error.
   *
   * @see AkismetAuthenticationException
   */
  const AUTH_ERROR = 401;

  /**
   * The Akismet API key to use for request authentication.
   *
   * @var string
   */
  public $key = '';

  /**
   * The Akismet server to communicate with, without the API key subdomain.
   *
   * @var string
   */
  public $server = 'rest.akismet.com';

  /**
   * Maximum number of attempts for a request to the Akismet server.
   *
   * @see Akismet::query()
   * @see Akismet::$requestTimeout
   *
   * @var integer
   */
  public $requestMaxAttempts = 2;

  /**
   * Seconds in which a request to the Akismet server times out.
   *
   * The timeout applies per request. Akismet::query() will retry a request until
   * it reaches Akismet::$requestMaxAttempts. With the default values, a Akismet
   * API call has a total timeout of 6 seconds in case of a server failure.
   *
   * @see Akismet::request()
   * @see Akismet::$requestMaxAttempts
   *
   * @var float
   */
  public $requestTimeout = 3.0;

  /**
   * The last server response.
   *
   * @var AkismetResponse
   */
  public $lastResponse = NULL;

  /**
   * Flag indicating whether to invoke Akismet::writeLog() in Akismet::query().
   *
   * @var bool
   */
  public $writeLog = TRUE;

  /**
   * A list of logged requests.
   *
   * @var array
   */
  public $log = array();

  function __construct() {
    $this->key = $this->loadConfiguration('key');
  }

  /**
   * Loads a configuration value from client-side storage.
   *
   * @param string $name
   *   The configuration setting name to load. Currently the only possibility is
   *   - key: The API key for Akismet authentication.
   *
   * @return mixed
   *   The stored configuration value or NULL if there is none.
   *
   * @see Akismet::saveConfiguration()
   * @see Akismet::deleteConfiguration()
   */
  abstract public function loadConfiguration($name);

  /**
   * Saves a configuration value to client-side storage.
   *
   * @param string $name
   *   The configuration setting name to save.
   * @param mixed $value
   *   The value to save.
   *
   * @see Akismet::loadConfiguration()
   * @see Akismet::deleteConfiguration()
   */
  abstract protected function saveConfiguration($name, $value);

  /**
   * Deletes a configuration value from client-side storage.
   *
   * @param string $name
   *   The configuration setting name to delete.
   *
   * @see Akismet::loadConfiguration()
   * @see Akismet::saveConfiguration()
   */
  abstract protected function deleteConfiguration($name);

  /**
   * Checks user-submitted content with Akismet.
   *
   * @param array $data
   *   An associative array containing any of the keys:
   *   - blog: The URL of this site.
   *   - user_ip: The IP address of the text submitter.
   *   - user_agent: The user-agent string of the web browser submitting the
   *     text.
   *   - referrer: The HTTP_REFERER value.
   *   - permalink: The permanent URL where the submitted text can be found.
   *   - comment_type: A description of the type of content being checked:
   *     https://blog.akismet.com/2012/06/19/pro-tip-tell-us-your-comment_type/
   *   - comment_author: The (real) name of the content author.
   *   - comment_author_email: The email address of the content author.
   *   - comment_author_url: The URL (if any) that the content author provided.
   *   - comment_content: The body of the content. If the content consists of
   *     multiple fields, concatenate them into one postBody string, separated
   *     by " \n" (space and line-feed).
   *   - comment_date_gmt: The date the content was submitted.
   *   - comment_post_modified_gmt: (For comments only) The creation date of the
   *     post being commented on.
   *   - blog_lang: The languages in use on this site, in ISO 639-1 format. Ex:
   *     "en, fr_ca".
   *   - blog_charset: The character encoding in use for the values being
   *     submitted.
   *   - user_role: The role of the user who submitted the comment. Optional.
   *     Should be 'administrator' for site administrators; submitting a value
   *     of 'administrator' will guarantee that Akismet sees the content as ham.
   *   - server: The contents of $_SERVER, to be added to the request.
   *
   * @return int|array
   *   On failure, the status code. On success, an associative array keyed as
   *   follows:
   *   - guid: The GUID returned by Akismet.
   *   - classification: the spam classification ('ham', 'spam', or 'unsure').
   */
  public function checkContent(array $data = array()) {
    if (empty($data['server'])) {
      $server = $_SERVER;
    }
    else {
      $server = _akismet_prepare_server_vars($data['server']);
      unset($data['server']);
    }
    $parameters = $data + $server;
    $result = $this->query('POST', 'comment-check', $parameters);
    if ($result->isError) {
      return $result->code;
    }

    $guid = $result->guid();
    if ($result->body === 'false') {
      return array(
        'classification' => 'ham',
        'guid' => $guid,
      );
    }

    if (!empty($result->headers['x-akismet-pro-tip'])) {
      return array(
        'classification' => 'spam',
        'guid' => $guid,
      );
    }

    if ($result->body === 'true') {
      return array(
        'classification' => 'unsure',
        'guid' => $guid,
      );
    }

    // If we get to this point, there was an error of some kind that we didn't
    // catch.
    return 500;
  }

  public function sendFeedback($data, $feedback) {
    if (isset($data['server'])) {
      $server = _akismet_prepare_server_vars($data['server']);
      unset($data['server']);
    }
    $parameters = (array) $data + (array) $server;

    if ($feedback === 'spam') {
      $result = $this->query('POST', 'submit-spam', $parameters);
    }
    else {
      $result = $this->query('POST', 'submit-ham', $parameters);
    }
    if ($result->isError) {
      return $result->code;
    }
    return $result->body;
  }

  /**
   * Returns platform and version information about the Akismet client.
   *
   * Retrieves platform and Akismet client version information to send along to
   * Akismet in the User-Agent header.
   *
   * @return array
   *   An associative array containing:
   *   - platformName: The name of the platform/distribution; e.g., "Backdrop".
   *   - platformVersion: The version of platform/distribution; e.g., "1.0".
   *   - clientName: The official Akismet client name; e.g., "Akismet".
   *   - clientVersion: The version of the Akismet client; e.g., "1.x-2.0".
   */
  abstract public function getUserAgentInformation();

  /**
   * Returns a string suitable for the User-Agent header of an Akismet request.
   *
   * @return string
   *   A string such as 'Backdrop/1.0 / Akismet/1.1'.
   */
  public function getUserAgent() {
    $info = $this->getUserAgentInformation();
    return "{$info['platformName']}/{$info['platformVersion']} | {$info['clientName']}/{$info['clientVersion']}";
  }

  abstract public function getSiteURL();

  /**
   * Writes log messages to a permanent location/storage.
   *
   * Not abstract, since clients are not required to write log messages.
   * The log may be written and appended to a file (via file_put_contents()),
   * syslog (on *nix-based systems), or a database.
   *
   * @see Akismet::log
   */
  public function writeLog() {
    // After writing log messages, empty the log.
    $this->purgeLog();
  }

  /**
   * Purges captured log messages.
   *
   * @see Akismet::writeLog()
   */
  final public function purgeLog() {
    $this->log = array();
  }

  /**
   * @param $method
   * @param $path
   * @param $data
   * @param bool $authenticate
   *
   * @return \AkismetResponse|int
   */
  public function query($method, $path, $data, $authenticate = TRUE) {
    $data += array(
      'blog' => $this->getSiteURL(),
    );

    $server = $this->getAkismetURL($authenticate);
    $max_attempts = $this->requestMaxAttempts;
    while ($max_attempts-- > 0) {
      try {
        $result = $this->handleRequest($method, $server, $path, $data);
      }
      catch (AkismetBadRequestException $e) {
        // Irrecoverable error, so don't try further.
        break;
      }
      catch (AkismetAuthenticationException $e) {
        // Irrecoverable error, so don't try further.
        break;
      }
      catch (AkismetException $e) {
        // If the requested resource does not exist, or the request was
        // malformed, there is no point in trying further.
        if ($e->getCode() >= 400 && $e->getCode() < 500) {
          break;
        }
      }
      // Unless we have a positive result, try again.
      if (!$this->lastResponse->isError) {
        break;
      }
    }

    // Write all captured log messages.
    if ($this->writeLog) {
      $this->writeLog();
    }

    return $this->lastResponse;
  }

  /**
   * Returns the correct REST server to use for a query.
   *
   * @param bool $authenticate
   *   If TRUE, returns a URL with an API key subdomain. If FALSE, returns a
   *   URL without an API key subdomain (to be used for non-authenticated
   *   calls.)
   *
   * @return string
   */
  protected function getAkismetURL($authenticate) {
    if ($authenticate) {
      $url = 'https://' . $this->key . '.' . $this->server;
    }
    else {
      $url = 'https://' . $this->server;
    }
    return $url;
  }

  /**
   * Prepares an HTTP request to the Akismet server and processes the response.
   *
   * @param $method
   * @param $server
   * @param $path
   * @param $data
   *
   * @return
   * @throws \AkismetAuthenticationException
   * @throws \AkismetException
   * @throws \AkismetNetworkException
   * @throws \AkismetResponseException
   */
  protected function handleRequest($method, $server, $path, $data) {
    $time_start = microtime(TRUE);
    if ($method == 'POST') {
      $headers['Content-Type'] = 'application/x-www-form-urlencoded';
    }
    $headers['User-Agent'] = $this->getUserAgent();

    // Append API version to REST endpoint.
    $server .= '/' . self::API_VERSION;

    // Encode data.
    $query = http_build_query($data);

    $response_data = $this->request($method, $server, $path, $query, $headers);
    $this->lastResponse = $response = new AkismetResponse($response_data);

    $time_stop = microtime(TRUE);

    $request_info = array(
      'request' => $method . ' ' . $server . '/' . $path,
      'headers' => $headers,
      'data' => $data,
      'response_code' => $response->code,
      'response_message' => $response->message,
      'response' => $response->body,
      'response_time' => $time_stop - $time_start,
    );
    if ($response->isError) {
      if ($response->code <= 0) {
        throw new AkismetNetworkException('Network error.', self::NETWORK_ERROR, NULL, $this, $request_info);
      }
      if ($response->code === self::REQUEST_ERROR) {
        throw new AkismetBadRequestException($response->message, self::REQUEST_ERROR, NULL, $this, $request_info);
      }
      if ($response->code === self::AUTH_ERROR) {
        throw new AkismetAuthenticationException($response->message, self::REQUEST_ERROR, NULL, $this, $request_info);
      }
      if ($response->code >= 500) {
        throw new AkismetResponseException($response->message, $response->code, NULL, $this, $request_info);
      }
      throw new AkismetException($response->message, $response->code, NULL, $this, $request_info);
    }
    else {
      // No message is logged in case of success.
      $this->log[] = array(
          'severity' => 'debug',
        ) + $request_info;

      return $this->lastResponse;
    }
  }

  /**
   * Performs an HTTP request to the Akismet server.
   *
   * @param string $method
   *   The HTTP method to use; i.e., 'GET', 'POST', or 'PUT'.
   * @param string $server
   *   The base URL of the server to perform the request against; e.g.,
   *   'http://foo.akismet.com'.
   * @param string $path
   *   The REST path/resource to request; e.g., 'site/1a2b3c'.
   * @param string $query
   *   (optional) A prepared string of HTTP query parameters to append to $path
   *   for $method GET, or to use as request body for $method POST.
   * @param array $headers
   *   (optional) An associative array of HTTP request headers to send along
   *   with the request.
   *
   * @return object
   *   An object containing response properties:
   *   - code: The HTTP status code as integer returned by the Akismet server.
   *   - message: The HTTP status message string returned by the Akismet server,
   *     or NULL if there is no message.
   *   - headers: An associative array containing the HTTP response headers
   *     returned by the Akismet server. Header name keys are expected to be
   *     lower-case; i.e., "content-type" instead of "Content-Type".
   *   - body: The HTTP response body string returned by the Akismet server, or
   *     NULL if there is none.
   *
   * @see Akismet::handleRequest()
   */
  abstract protected function request($method, $server, $path, $query = NULL, array $headers = array());

  /**
   * Verifies an API key with Akismet.
   *
   * @param string $key
   *   The API key to be checked, if different than the one in the constructor.
   *
   * @return boolean|int
   *   TRUE or FALSE if we got a response from the server; otherwise, the error
   *   code.
   */
  public function verifyKey($key) {
    if (empty($key)) {
      $key = $this->key;
    }
    $parameters = [
      'key' => $key,
    ];
    $response = $this->query('POST', 'verify-key', $parameters, FALSE);
    if ($response->body === 'valid') {
      return TRUE;
    }
    if ($response->body === 'invalid') {
      return FALSE;
    }
    return ($response->code);
  }
}

/**
 * Represents a response from the Akismet API.
 */
class AkismetResponse {

  /**
   * @var int $code
   */
  protected $code;

  /**
   * @var string $message
   */
  protected $message;

  /**
   * Associative array of response headers, keyed by header name.
   *
   * @var array
   */
  protected $headers;

  /**
   * The body of the response. Usually one word: 'true', 'false', 'valid'.
   *
   * @var string
   */
  protected $body;

  /**
   * A flag indicating whether the response indicated an error.
   *
   * @var bool
   */
  protected $isError;

  public function __construct($data) {
    $this->headers = $data->headers;
    $this->body = $data->body;
    $this->code = $data->code;

    // Determine basic error condition based on HTTP status code.
    $this->isError = ($this->code < 200 || $this->code >= 300);

    // The Akismet API returns 200 OK even when there's an error, so it's
    // hard to be sure what kind of response this is.

    // One way we can be sure the request was malformed is if we receive the
    // header 'X-Akismet-Debug-Help'.
    if (!empty($this->headers['x-akismet-debug-help'])) {
      $this->isError = TRUE;
      $this->code = Akismet::REQUEST_ERROR;
      $this->message = $data->headers['x-akismet-debug-help'];
    }
    // Another clue is if we receive the body text "Invalid API key."
    if ($this->body === 'Invalid API key.') {
      $this->isError = TRUE;
      $this->code = Akismet::AUTH_ERROR;
      $this->message = $this->body;
    }
  }

  public function __get($name) {
    return $this->{$name};
  }

  public function guid() {
    if (!empty($this->headers['x-akismet-guid'])) {
      return $this->headers['x-akismet-guid'];
    }
    return FALSE;
  }
}


/**
 * A catchable Akismet exception.
 *
 * The Akismet class internally uses exceptions to handle HTTP request errors
 * within the Akismet::handleRequest() method. All exceptions thrown in the
 * Akismet class and derived classes should be instances of the AkismetException
 * class if they pertain to errors that can be caught/handled within the class.
 * Other errors should not use the AkismetException class and handled
 * differently.
 *
 * No AkismetException is supposed to pile up as a user-facing fatal error. All
 * functions that invoke Akismet::handleRequest() have to catch Akismet
 * exceptions.
 *
 * @see Akismet::query()
 * @see Akismet::handleRequest()
 *
 * @param $message
 *   The Exception message to throw.
 * @param $code
 *   The Exception code.
 * @param $previous
 *   (optional) The previous Exception, if any.
 * @param $instance
 *   The Akismet class instance the Exception is thrown in.
 * @param $arguments
 *   (optional) A associative array containing information about a performed
 *   HTTP request that failed:
 *   - request: (string) The HTTP method and URI of the performed request; e.g.,
 *     "GET https://rest.akismet.com/v1/foo/bar". In case of GET requests, do
 *     not add query parameters to the URI; pass them in 'data' instead.
 *   - data: (array) An associative array containing HTTP GET/POST/PUT request
 *     query parameters that were sent to the server.
 *   - response: (mixed) The server response, either as string, or the already
 *     parsed response; i.e., an array.
 */
class AkismetException extends Exception {
  /**
   * @var Akismet
   */
  protected $akismet;

  /**
   * The severity of this exception.
   *
   * By default, all exceptions should be logged and appear as errors (unless
   * overridden by a later log entry).
   *
   * @var string
   */
  protected $severity = 'error';

  /**
   * Overrides Exception::__construct().
   */
  function __construct($message = '', $code = 0, Exception $previous = NULL, Akismet $akismet, array $request_info = array()) {
    // Fatal error on PHP <5.3 when passing more arguments to Exception.
    if (version_compare(phpversion(), '5.3') >= 0) {
      parent::__construct($message, $code, $previous);
    }
    else {
      parent::__construct($message, $code);
    }
    $this->akismet = $akismet;

    // Log the exception.
    $message = array(
      'severity' => $this->severity,
      'message' => 'Error @code: %message (@server-ip)',
      'arguments' => array(
        '@code' => $code,
        '%message' => $message,
      ),
    );
    // Add HTTP request information, if available.
    if (!empty($request_info)) {
      $message += $request_info;
    }
    $akismet->log[] = $message;
  }
}

/**
 * Akismet network error exception.
 *
 * Thrown in case a HTTP request results in code <= 0, denoting a low-level
 * communication error.
 */
class AkismetNetworkException extends AkismetException {
  /**
   * Overrides AkismetException::$severity.
   *
   * The client may be able to recover from this error, so use a warning level.
   */
  protected $severity = 'warning';
}

/**
 * Akismet server response exception.
 *
 * Thrown when a request to the Akismet server succeeds, but the response does
 * not contain an expected element; e.g., a backend configuration or execution
 * error.
 *
 * @see Akismet::handleRequest()
 */
class AkismetResponseException extends AkismetException {
  /**
   * Overrides AkismetException::$severity.
   *
   * Might be a client-side error, but more likely a server-side error. The
   * client may be able to recover from this error.
   */
  protected $severity = 'debug';
}
/**
 * Akismet authentication error exception.
 *
 * Thrown in case API keys or other authentication parameters are invalid.
 */
class AkismetAuthenticationException extends AkismetException {
}

/**
 * Akismet error due to bad client request exception.
 */
class AkismetBadRequestException extends AkismetException {
}
