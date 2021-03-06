<?php
/**
 * @file
 * Gitlc API wrapper class definition.
 */


use \Curl\Curl;

require_once 'libraries/php-curl-class/src/Curl/CaseInsensitiveArray.php';
require_once 'libraries/php-curl-class/src/Curl/Curl.php';


/**
 * Gitlc API wrapper class.
 */
class GitlcAPI {

  /**
   * The Curl object used for performing HTTP requests.
   *
   * @var \Curl\Curl
   */
  protected $curl;

  /**
   * Token required to interact with Gitlc API.
   *
   * @var string
   */
  protected $token = '';

  /**
   * Organisation or User name.
   *
   * @var string
   */
  protected $owner_name;

  /**
   * Repository name.
   *
   * @var string
   */
  protected $repo_name;

  /**
   * Debug mode. If enabled, data will be printed via backdrop_set_message().
   *
   * @var bool
   */
  protected $debug = FALSE;

  /**
   * Latest error message from GitlcAPI.
   *
   * @var string
   */
  protected $error_message = '';

  /**
   * Constructor.
   */
  public function __construct() {
    $this->reInitCurl();
    $this->setDebug();
  }

  /**
   * Initializate $this->curl with Curl class and preset headers and user agent.
   */
  public function reInitCurl() {
    $this->curl = new Curl();
    $this->curl->setHeader('Content-Type', 'application/json');
    $this->curl->setUserAgent('Backdrop CMS Gitlc API module');
    $this->curl->setHeader('Accept', '*/*');
  }

  /**
   * Set owner name for requests.
   */
  public function setOwnerName($owner_name) {
    $this->owner_name = $owner_name;
  }

  /**
   * Set repository name for requests.
   */
  public function setRepoName($repo_name) {
    $this->repo_name = $repo_name;
  }

  /**
   * Set debug value. False by default.
   *
   * @param $debug boolean
   *   TRUE or FALSE
   */
  public function setDebug($debug = FALSE) {
    $this->debug = $debug;
  }

  /**
   * Set token for Gitlc API.
   *
   * @param $token
   *   Token to access Gitlc API.
   */
  public function setToken($token = '') {
    $this->token = $token;
  }

  /**
   * Get store token.
   *
   * @return
   *   A string value. Stored token for Gitlc API access.
   */
  public function getToken() {
    return $this->token;
  }

  /**
   * Determine if curl request has been falen with error.
   *
   * @return boolean
   *   TRUE or FALSE based on answer from Gitlc API.
   */
  public function isError() {
    return $this->curl->curlError or $this->curl->error;
  }

  /**
   * Get Curl details after request.
   *
   * @return array
   *   An array of request information:
   *     - code: the last error number. @see curl_errno.
   *     - message: A clear text error message for the last cURL operation.
   *   @see curl_error.
   *     - request_headers: an array of request headers.
   *     - response_headers: an array of response headers.
   */
  public function testingGetHeaders() {
    return array(
      'code' => $this->curl->curlErrorCode,
      'message' => $this->curl->curlErrorMessage,
      'request_headers' => $this->curl->requestHeaders,
      'response_headers' => $this->curl->responseHeaders,
      'curl' => $this->curl
    );
  }

  /**
   * Get Curl details if error happen.
   *
   * @return
   *   An array of request information. @see testingGetHeaders.
   *   FALSE if there is no error.
   */
  public function getErrors() {
    if ($this->isError()) {
      return $this->testingGetHeaders();
    }
    return FALSE;
  }

  /**
   * Get GitlcAPi Error message if error happen.
   *
   * @return
   *   String with error message.
   */
  public function getGitlcAPIErrorMessage() {
    return $this->error_message;
  }

  /**
   * Determine if request has been cached.
   *
   * @return
   *   FALSE if error. Object with answer if request success.
   */
  protected function getResponse() {
    if ($this->debug) {
      backdrop_set_message('<pre>' . print_r($this->testingGetHeaders(), TRUE) . '</pre>');
    }
    if ($this->isError()) {
      if (isset($this->curl->response->error) && $this->curl->response->error) {
        if (isset($this->curl->response->message)) {
          $this->error_message = $this->curl->response->message;
        }
      }

      if ($message = $this->getGitlcAPIErrorMessage()) {
        backdrop_set_message(t('Gitlc error message !message', array('!message' => $message)), 'error');
      }
      else {
        $code = $this->curl->errorCode;
        $message = $this->curl->errorMessage;
        backdrop_set_message(t('Communication error: !code !message', array(
          '!code' => $code,
          '!message' => $message
        )), 'error');
      }

      return FALSE;
    }
    if (is_object($this->curl->response)) {
      return (array) $this->curl->response;
    }
    if ('null' == $this->curl->response or NULL == $this->curl->response) {
      return FALSE;
    }
    return $this->curl->response;
  }

  /**
   * Perform GET request to Gitlc API and return answer.
   *
   * @param $command
   *   String value. Gitlc API url with tokens Like :owner, :repo and ect.
   * @param $params array
   *   Values for request and tokens for request url. Like :owner, :repo, :id
   *   and etc.
   *
   * @return array|bool|null
   *   FALSE if request failed. An array of data on success.
   */
  protected function getRequest($command, $params = array()) {

    $this->prepareCommand($command, $params);

    if ($this->getToken()) {
      $this->curl->setHeader('Token', $this->getToken());
    }

    $this->curl->get(GITLC_API_URI . '/' . $command, $params);
    $response = $this->getResponse();
    return $response;
  }

  /**
   * Perform PUT request to Gitlc API and return answer.
   *
   * @param $command
   *   String value. Gitlc API url with tokens Like :owner, :repo and ect.
   * @param $params array
   *   Values for request and tokens for request url. Like :owner, :repo, :id
   *   and etc.
   *
   * @return
   *   FALSE if request failed. Object if success.
   */
  protected function putRequest($command, $params = array()) {

    $this->prepareCommand($command, $params);

    if ($this->getToken()) {
      $this->curl->setHeader('Token', $this->getToken());
    }

    $this->curl->put(GITLC_API_URI . '/' . $command, $params);
    $response = $this->getResponse();
    return $response;
  }

  /**
   * Perform POST request to Gitlc API and return answer.
   *
   * @param $command
   *   String value. Gitlc API url with tokens Like :owner, :repo and ect.
   * @param $params array
   *   Values for request and tokens for request url. Like :owner, :repo, :id
   *   and etc.
   *
   * @return
   *   FALSE if request failed. Object if success.
   */
  protected function postRequest($command, $params = array()) {

    $this->prepareCommand($command, $params);

    if ($this->getToken()) {
      $this->curl->setHeader('Token', $this->getToken());
    }
    $this->curl->post(GITLC_API_URI . '/' . $command, $params);
    $response = $this->getResponse();
    return $response;
  }

  /**
   * Perform DELETE request to Gitlc API and return answer.
   *
   * @param $command
   *   String value. Gitlc API url with tokens Like :owner, :repo and ect.
   * @param $params array
   *   Values for request and tokens for request url. Like :owner, :repo, :id
   *   and etc.
   *
   * @return
   *   FALSE if request failed. Object if success.
   */
  protected function deleteRequest($command, $params = array()) {

    $this->prepareCommand($command, $params);

    if ($this->getToken()) {
      $this->curl->setHeader('Token', $this->getToken());
    }

    $this->curl->delete(GITLC_API_URI . '/' . $command, $params);
    $response = $this->getResponse();
    return $response;
  }

  /**
   * Replace tokens with values in request url.
   *
   * @param $command
   *   String value. Gitlc API url with tokens Like :owner, :repo and ect.
   * @param $params array
   *   Values for request and tokens for request url. Like :owner, :repo, :id
   *   and etc.
   */
  private function prepareCommand(&$command, &$params) {
    foreach ($params as $key => $val) {
      if ($key[0] == ':') {
        $command = str_replace($key, $val, $command);
        unset($params[$key]);
      }
    }
  }

  /**
   * Get repository information.
   * https://developer.github.com/v3/repos/#get
   *
   * @return
   *   Return repository information or FALSE if there is no repository or no
   *   access.
   *
   * @see SetRepoName
   * @see SetOwnerName
   */
  public function getHook() {
    return $this->getRequest(
      'repos/:owner/:repo/hook', array(
        ':owner' => $this->owner_name,
        ':repo' => $this->repo_name,
      )
    );
  }

  /**
   * Create hook for repository.
   *
   * @param $settings array
   *   An array of settings for hook
   *     - url: The URL to which the payloads will be delivered.
   *     - secret: If provided, payloads will be delivered with an Signature
   *   header.
   *
   * @return
   *   Return hook information or FALSE if there is no access.
   *
   * @see SetRepoName
   * @see SetOwnerName
   */
  public function createHook($settings) {
    $settings[':owner'] = $this->owner_name;
    $settings[':repo'] = $this->repo_name;
    return $this->postRequest('repos/:owner/:repo/hook', $settings);
  }

  /**
   * Update hook for repository.
   *
   * @param $settings array
   *   An array of settings for hook
   *     - url: The URL to which the payloads will be delivered.
   *     - secret: If provided, payloads will be delivered with an Signature
   *   header.
   *
   * @return
   *   Return hook information or FALSE if there is no access.
   *
   * @see SetRepoName
   * @see SetOwnerName
   */
  public function updateHook($settings) {
    $settings[':owner'] = $this->owner_name;
    $settings[':repo'] = $this->repo_name;
    return $this->putRequest('repos/:owner/:repo/hook', $settings);
  }

  /**
   * Delete hook for repository.
   * @return
   *   Return empty value if success or FALSE if error.
   *
   * @see SetRepoName
   * @see SetOwnerName
   */
  public function deleteHook() {
    return $this->deleteRequest(
      'repos/:owner/:repo/hook', array(
        ':owner' => $this->owner_name,
        ':repo' => $this->repo_name,
      )
    );
  }
}
