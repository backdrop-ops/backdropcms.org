<?php
/**
 * @file
 * GitHub API wrapper class definition.
 */

use \Curl\Curl;

require_once 'libraries/php-curl-class/src/Curl/CaseInsensitiveArray.php';
require_once 'libraries/php-curl-class/src/Curl/Curl.php';


/**
 * GitHub API wrapper class.
 */
class GitHubAPI {

  // Curl Class object.
  protected $curl = FALSE;
  // Token required to interact with GitHub API.
  protected $token = FALSE;
  // Folder where Get Requests to API will be cached.
  protected $cache_folder = FALSE;
  // Debug mode. If enabled, data will be printed via backdrop_set_message.
  protected $debug = FALSE;
  // We store cache based on token. It's a path to personal folder for current token.
  protected $current_cache_folder = FALSE;
  // TRUE If request has been served from cache.
  protected $cache_expiration = FALSE;
  // TRUE to disable cache and request directly to GitHub API.
  protected $disable_cache = FALSE;
  // Organisation or User name.
  protected $owner_name = FALSE;
  // Repository name.
  protected $repo_name = FALSE;
  // For how long we cache result.
  protected $age = FALSE;
  // How much results per page.
  public $per_page = 100;

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
    $this->curl->setUserAgent('Backdrop CMS GitHub API module');
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
   * Set token for GitHub API and prepare caching folder.
   *
   * @param $token
   *   Token to access GitHub API.
   */
  public function setToken($token = '') {
    $this->token = $token;
    $this->prepareCacheFolder();
  }

  /**
   * Get store token.
   *
   * @return
   *   A string value. Stored token for GitHub API access.
   */
  public function getToken() {
    return $this->token;
  }

  /**
   * Determine if curl request has been falen with error.
   *
   * @return boolean
   *   TRUE or FALSE based on answer from GitHub API.
   */
  public function isError() {
    return $this->curl->curlError;
  }

  /**
   * Get Curl details after request.
   *
   * @return array
   *   An array of request information:
   *     - code: the last error number. @see curl_errno.
   *     - message: A clear text error message for the last cURL operation. @see curl_error.
   *     - request_headers: an array of request headers.
   *     - response_headers: an array of response headers.
   */
  public function testingGetHeaders() {
    return array (
        'code' => $this->curl->curlErrorCode,
        'message' => $this->curl->curlErrorMessage,
        'request_headers' => $this->curl->requestHeaders,
        'response_headers' => $this->curl->responseHeaders
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
   * Disable Get request caching to GitHub API.
   */
  public function disableCache() {
    $this->disable_cache = TRUE;
  }

  /**
   * Prepare directory to cache requests.
   *
   * @access protected
   */
  protected function prepareCacheFolder() {
    $root_folder = 'private://github_cache/';

    if (!$client_folder = $this->getToken()) {
      $client_folder = 'uid-0';
    }

    $this->current_cache_folder = $root_folder . $client_folder;
    file_prepare_directory($this->current_cache_folder, FILE_CREATE_DIRECTORY);

  }

  /**
   * Set debug value. False by default.
   *
   * @param $age integer
   *   Time in seconds. We will cache result based on request time + $age.
   */
  public function setAge($age) {
    $this->age = $age;
  }

  /**
   * Get current caching age.
   *
   * @access private
   * @return  integer
   *   Time in seconds @see setAge.
   */
  private function getResponseAge() {
    global $user;
    if ($this->age) {
      return $this->age;
    }
    if ($age_header = $this->curl->responseHeaders['Cache-Control']) {
      list($type, $maxage, $smaxage) = explode(',', $age_header);
      list($name, $age) = explode('=', $maxage);
      if ($user->uid == 0) {
        // Default max age is 60. Let's cache for anonymous users for 5 min.
        $age = $age * 5;
      }
      return $age;
    }
    return 0;
  }

  /**
   * Save cache for future requests.
   *
   * @access private
   * @param $command
   *   String value. GitHub API url with already placed owner and repo if required.
   * @param $params array
   *   Values for request. We create a hash file based on params and command to make sure that cache is unique for request.
   */
  private function cacheRequest($command, $params) {
    if ($this->disable_cache) {
      return FALSE;
    }
    $serialize_object = serialize(array('command' => $command, 'params' => $params));
    $file_name = hash('sha256', $serialize_object);

    $contents['response'] = $this->curl->response;
    $contents['age'] = $this->getResponseAge();
    $contents = json_encode($contents);

    $uri = $this->current_cache_folder . '/' . $file_name;
    file_unmanaged_save_data($contents, $uri, FILE_EXISTS_REPLACE);

  }

  /**
   * Return cache if exists.
   *
   * @access private
   * @param $command
   *   String value. GitHub API url with already placed owner and repo if required.
   * @param $params array
   *   Values for request. We create a hash file based on params and command to make sure that cache is unique for request.
   * @return
   *   FALSE if there is no cache. Object if cache exists and didnot expire yet.
   */
  private function getCacheRequest($command, $params) {
    if ($this->disable_cache) {
      return FALSE;
    }
    $serialize_object = serialize(array('command' => $command, 'params' => $params));
    $file_name = hash('sha256', $serialize_object);

    $uri = $this->current_cache_folder . '/' . $file_name;

    $filename = drupal_realpath($uri);

    if (file_exists($filename)) {
      $timestamp = filemtime($filename);
      if ($contents = @json_decode(file_get_contents($filename))) {
        if (($timestamp + $contents->age) > REQUEST_TIME) {

          $this->cache_expiration = $timestamp + $contents->age;

          if ($this->debug) {
            backdrop_set_message('Cache returned!');
          }
          return $contents->response;
        } else {
          if ($this->debug) {
            backdrop_set_message('No cache returned!'. ($timestamp + $contents->age) . '>'.REQUEST_TIME);
          }
        }
      }
    }
    return FALSE;
  }

  /**
   * Determine if request has been cached.
   *
   * @return 
   *   FALSE if not and time when cache get expired in TIMESTAMP.
   */
  public function isCached() {
    return $this->cache_expiration;
  }

  /**
   * Determine if request has been cached.
   *
   * @access protected
   * @return 
   *   FALSE if error. Object with answer if request success.
   */
  protected function getResponse() {
    if ($this->debug) {
      backdrop_set_message('<pre>'.print_r($this->testingGetHeaders(), true).'</pre>');
    }
    if ($this->isError()) {
      return FALSE;
    }

    return $this->curl->response;
  }

   /**
   * Perform GET request to GitHub API and return answer.
   *
   * @access protected
   * @param $command
   *   String value. GitHub API url with tokens like :owner, :repo and ect.
   * @param $params array
   *   Values for request and tokens for request url. LIke :owner, :repo, :id and etc.
   * @return
   *   FALSE if request failed. Object if success.
   */
  protected function getRequest($command, $params = array()) {

    $this->prepareCommand($command, $params);

    if ($this->getToken()) {
      $params['access_token'] = $this->getToken();
    }

    // Add default 1000 per page.
    $params['per_page'] = $this->per_page;

    if ($response = $this->getCacheRequest($command, $params)) {
      return $response;
    }

    $this->curl->get(GITHUB_API_URI . '/' . $command, $params);
    $response =  $this->getResponse();
    $this->cacheRequest($command, $params);
    return $response;
  }

  /**
   * Perform PUT request to GitHub API and return answer.
   *
   * @access protected
   * @param $command
   *   String value. GitHub API url with tokens like :owner, :repo and ect.
   * @param $params array
   *   Values for request and tokens for request url. LIke :owner, :repo, :id and etc.
   * @return
   *   FALSE if request failed. Object if success.
   */
  protected function putRequest($command, $params = array()) {

    $params['committer'] = $this->getCommittrer();
    $this->prepareCommand($command, $params);

    $query='';
    if ($this->getToken()) {
      $query = '?access_token=' . $this->getToken();
    }

    $this->curl->put(GITHUB_API_URI . '/' . $command . $query, $params);
    $response =  $this->getResponse();
    return $response;
  }

  /**
   * Perform POST request to GitHub API and return answer.
   *
   * @access protected
   * @param $command
   *   String value. GitHub API url with tokens like :owner, :repo and ect.
   * @param $params array
   *   Values for request and tokens for request url. LIke :owner, :repo, :id and etc.
   * @return
   *   FALSE if request failed. Object if success.
   */
  protected function postRequest($command, $params = array()) {

    $this->prepareCommand($command, $params);
    $query='';
    if ($this->getToken()) {
      $query = '?access_token=' . $this->getToken();
    }
    $this->curl->post(GITHUB_API_URI . '/' . $command . $query, $params);
    $response =  $this->getResponse();
    return $response;
  }

  /**
   * Perform DELETE request to GitHub API and return answer.
   *
   * @access protected
   * @param $command
   *   String value. GitHub API url with tokens like :owner, :repo and ect.
   * @param $params array
   *   Values for request and tokens for request url. LIke :owner, :repo, :id and etc.
   * @return
   *   FALSE if request failed. Object if success.
   */
  protected function deleteRequest($command, $params = array()) {

    $this->prepareCommand($command, $params);

    $query='';
    if ($this->getToken()) {
      $query = '?access_token=' . $this->getToken();
    }

    $this->curl->delete(GITHUB_API_URI . '/' . $command . $query, $params);
    $response =  $this->getResponse();
    return $response;
  }


  /**
   * Replace tokens with values in request url.
   *
   * @access private
   * @param $command
   *   String value. GitHub API url with tokens like :owner, :repo and ect.
   * @param $params array
   *   Values for request and tokens for request url. LIke :owner, :repo, :id and etc.
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
   * Get user organisations.
   * https://developer.github.com/v3/orgs/#list-your-organizations
   *
   * @return
   *   Return user organisations or FALSE if there is no user or no access.
   */
  public function getOrgs() {
    return $this->getRequest('user/orgs');
  }

  /**
   * Get user information.
   * https://developer.github.com/v3/users/#get-a-single-user
   *
   * @param $username
   *   String value. Username.
   * @return
   *   Return user information or FALSE if there is no user or no access.
   */
  public function getUser($username) {
    return $this->getRequest('users/:username', array(':username' => $username));
  }

  /**
   * Get user repositories.
   * https://developer.github.com/v3/repos/#list-your-repositories
   *
   * @param $username
   *   String value. Username.
   * @return
   *   Return user repositories or FALSE if there is no user or no access.
   */
  public function getUserRepos($username) {
    return $this->getRequest('users/:username/repos', array(':username' => $username));
  }

  /**
   * Get organisation information.
   * https://developer.github.com/v3/orgs/#get-an-organization
   *
   * @param $orgname
   *   String value. Organisation name.
   * @return
   *   Return organisation information or FALSE if there is no organisation or no access.
   */
  public function getOrg($orgname) {
    return $this->getRequest('orgs/:org', array(':org' => $orgname));
  }

  /**
   * Get organisation repositories.
   * https://developer.github.com/v3/repos/#list-organization-repositories
   *
   * @param $orgname
   *   String value. Organisation name.
   * @return
   *   Return organisation repositories or FALSE if there is no organisation or no access.
   */
  public function getOrgsRepos($orgname) {
    return $this->getRequest('orgs/:org/repos', array(':org' => $orgname));
  }

  /**
   * Get repository information.
   * https://developer.github.com/v3/repos/#get
   *
   * @return
   *   Return repository information or FALSE if there is no repository or no access.
   *
   * @see SetRepoName
   * @see SetOwnerName
   */
  public function getRepo() {
    return $this->getRequest(
        'repos/:owner/:repo', array(
        ':owner' => $this->owner_name,
        ':repo' => $this->repo_name,
        )
    );
  }

  /**
   * Create hook for repository.
   * https://developer.github.com/v3/repos/hooks/#create-a-hook
   *
   * @param $settings array
   *   An array of settings for hook
   *     - name: Must be passed as "web".
   *     - config: Key/Value:
   *        - url: The URL to which the payloads will be delivered. 
   *        - content_type: The media type used to serialize the payloads. Supported values include json and form. The default is form.
   *        - secret: If provided, payloads will be delivered with an X-Hub-Signature header.
   *        - insecure_ssl: Determines whether the SSL certificate of the host for url will be verified when delivering payloads. 
   *     - events: Determines what events the hook is triggered for. Default: ["push"].
   *     - active: Determines whether the hook is actually triggered on pushes.
   * @return
   *   Return hook information or  FALSE if there is no access.
   *
   * @see SetRepoName
   * @see SetOwnerName
   */
  public function createHook($settings) {
    $settings[':owner'] = $this->owner_name;
    $settings[':repo'] = $this->repo_name;
    return $this->postRequest('repos/:owner/:repo/hooks', $settings);
  }

  /**
   * Delete hook for repository.
   * https://developer.github.com/v3/repos/hooks/#delete-a-hook
   *
   * @param $id integer
   *   Hook id to delete
   * @return
   *   Return empty value if success or FALSE if error.
   *
   * @see SetRepoName
   * @see SetOwnerName
   */
  public function deleteHook($id) {
    return $this->deleteRequest(
        'repos/:owner/:repo/hooks/:id', array(
        ':owner' => $this->owner_name,
        ':repo' => $this->repo_name,
        ':id' => $id,
        )
    );
  }

  /**
   * Create hook for organisation.
   * https://developer.github.com/v3/orgs/hooks/#create-a-hook
   *
   * @param $settings array
   *   An array of settings for hook
   *     - name: Must be passed as "web".
   *     - config: Key/Value:
   *        - url: The URL to which the payloads will be delivered. 
   *        - content_type: The media type used to serialize the payloads. Supported values include json and form. The default is form.
   *        - secret: If provided, payloads will be delivered with an X-Hub-Signature header.
   *        - insecure_ssl: Determines whether the SSL certificate of the host for url will be verified when delivering payloads. 
   *     - events: Determines what events the hook is triggered for. Default: ["push"].
   *     - active: Determines whether the hook is actually triggered on pushes.
   * @return
   *   Return hook information or FALSE if there is no access.
   *
   * @see SetOwnerName
   */
  public function createOrgHook($settings) {
    $settings[':org'] = $this->owner_name;
    return $this->postRequest('orgs/:org/hooks', $settings);
  }

  /**
   * Delete hook for organisation.
   * https://developer.github.com/v3/orgs/hooks/#delete-a-hook
   *
   * @param $id integer
   *   Hook id to delete
   * @return
   *   Return empty value if success or FALSE if error.
   *
   * @see SetOwnerName
   */
  public function deleteOrgHook($id) {
    return $this->deleteRequest(
        'orgs/:org/hooks/:id', array(
        ':org' => $this->owner_name,
        ':id' => $id,
        )
    );
  }

  /**
   * Get a single commit.
   * https://developer.github.com/v3/repos/commits/#get-a-single-commit
   *
   * @param $sha 
   *   hash for commit
   * @return
   *   Return commit information or FALSE if there is no access.
   *
   * @see SetRepoName
   * @see SetOwnerName
   */
  public function getCommit($sha) {
    return $this->getRequest(
        'repos/:owner/:repo/commits/:sha', array(
        ':owner' => $this->owner_name,
        ':repo' => $this->repo_name,
        ':sha' => $sha,
        )
    );
  }

  /**
   * Create a commit comment.
   * https://developer.github.com/v3/repos/comments/#create-a-commit-comment
   *
   * @param $settings array
   *   An array of settings for new comment
   *     - body: The contents of the comment.
   *     - path: Relative path of the file to comment on.
   *     - position: Line index in the diff to comment on.
   *
   * @return
   *   Return comment information or FALSE if there is no access.
   *
   * @see SetRepoName
   * @see SetOwnerName
   */
  public function createComment($settings) {
    $settings[':owner'] = $this->owner_name;
    $settings[':repo'] = $this->repo_name;
    return $this->postRequest('repos/:owner/:repo/commits/:sha/comments', $settings);
  }

  /**
   * Create an issue comment.
   * https://developer.github.com/v3/issues/comments/#create-a-comment
   *
   * @param $settings array
   *   An array of settings for new comment
   *     - body: The contents of the comment.
   *     - :number : Issue number.
   *
   * @return
   *   Return comment information or FALSE if there is no access.
   *
   * @see SetRepoName
   * @see SetOwnerName
   */
   public function createIssueComment($settings) {
    $settings[':owner'] = $this->owner_name;
    $settings[':repo'] = $this->repo_name;
    return $this->postRequest('repos/:owner/:repo/issues/:number/comments', $settings);
  }

  /**
   * Get README.md file content from repository.
   * https://developer.github.com/v3/repos/contents/#get-the-readme
   *
   * @return
   *   Return content of the file already decoded.
   *
   * @see SetRepoName
   * @see SetOwnerName
   */
   public function getReadme() {
    $readme = $this->getRequest('repos/:owner/:repo/readme');
    if (!empty($readme->content)) {
      return base64_decode($readme->content);
    }
    return FALSE;
  }

}
