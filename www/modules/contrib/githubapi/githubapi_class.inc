<?php
use \Curl\Curl;

require_once 'libraries/php-curl-class/src/Curl/CaseInsensitiveArray.php';
require_once 'libraries/php-curl-class/src/Curl/Curl.php';



class GithubAPI
{

  protected $curl = FALSE;
  protected $token = FALSE;
  protected $cache_folder = FALSE;
  protected $debug = FALSE;
  protected $current_cache_folder = FALSE;
  protected $cache_expiration = FALSE;
  protected $disable_cache = FALSE;
  protected $owner_name = FALSE;
  protected $repo_name = FALSE;
  protected $age = FALSE;
  public $per_page = 1000;


  public function __construct() {
    $this->reInitCurl();
    $this->setDebug();
  }

  public function reInitCurl() {
    $this->curl = new Curl();
    $this->curl->setHeader('Content-Type', 'application/json');
    $this->curl->setUserAgent('http://git.lc application');
    $this->curl->setHeader('Accept', '*/*');
  }

  public function setOwnerName($owner_name) {
    $this->owner_name = $owner_name;
  }

  public function setRepoName($repo_name) {
    $this->repo_name = $repo_name;
  }

  public function setDebug($debug = FALSE)
  {
    $this->debug = $debug;
  }

  public function setToken($token = '') {
    $this->token = $token;
    $this->prepareCacheFolder();
    return $this->token;
  }

  public function getToken() {
    return $this->token;
  }

  public function isError() {
    return $this->curl->curlError;
  }

  public function testingGetHeaders() {
    return array (
        'code' => $this->curl->curlErrorCode,
        'message' => $this->curl->curlErrorMessage,
        'request_headers' => $this->curl->requestHeaders,
        'response_headers' => $this->curl->responseHeaders,
        'curl' => $this->curl,
      );
  }

  public function getErrors() {
    if($this->isError()){
      return array (
        'code' => $this->curl->curlErrorCode,
        'message' => $this->curl->curlErrorMessage,
        'request_headers' => $this->curl->requestHeaders,
        'response_headers' => $this->curl->responseHeaders,
      );
    }
    return FALSE;
  }

  public function disableCache() {
    $this->disable_cache = TRUE;
  }

  protected function prepareCacheFolder() {
    $root_folder = 'private://github_cache/';

    if(!$client_folder = $this->getToken()){
      $client_folder = 'uid-0';
    }

    $this->current_cache_folder = $root_folder . $client_folder;
    file_prepare_directory($this->current_cache_folder, FILE_CREATE_DIRECTORY);

  }

  public function setAge($age) {
    $this->age = $age;
  }

  private function getResponseAge() {
    global $user;
    if($this->age){
      return $this->age;
    }
    if($age_header = $this->curl->responseHeaders['Cache-Control']){
      list($type, $maxage, $smaxage) = explode(',', $age_header);
      list($name,$age) = explode('=', $maxage);
      if($user->uid == 0){
        // Default max age is 60. Let's cache for anonymous users for 5 min.
        $age = $age * 5;
      }
      return $age;
    }
    return 0;
  }

  private function cacheRequest($command, $params) {
    if($this->disable_cache){
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

  private function getCacheRequest($command, $params) {
    if($this->disable_cache){
      return FALSE;
    }
    $serialize_object = serialize(array('command' => $command, 'params' => $params));
    $file_name = hash('sha256', $serialize_object);

    $uri = $this->current_cache_folder . '/' . $file_name;

    $filename = drupal_realpath($uri);

    if (file_exists($filename)) {
      $timestamp = filemtime($filename);
      if($contents = @json_decode(file_get_contents($filename))){
        if(($timestamp + $contents->age) > REQUEST_TIME){

          $this->cache_expiration = $timestamp + $contents->age;

          if($this->debug) {
            backdrop_set_message('Cache returned!');
          }
          return $contents->response;
        }else{
          if($this->debug) {
            backdrop_set_message('No cache returned!'. ($timestamp + $contents->age) . '>'.REQUEST_TIME);
          }
        }
      }
    }
    return FALSE;
  }

  public function isCached() {
    return $this->cache_expiration;
  }

  protected function getResponse() {
    if($this->debug) {
      backdrop_set_message('<pre>'.print_r($this->testingGetHeaders(), true).'</pre>');
    }
    if($this->isError()){
      return FALSE;
    }

    return $this->curl->response;
  }

  protected function getRequest($command, $params = array()) {

    $this->prepareCommand($command, $params);

    if($this->getToken()){
      $params['access_token'] = $this->getToken();
    }

    // Add default 1000 per page.
    $params['per_page'] = $this->per_page;

    if($response = $this->getCacheRequest($command, $params)){
      return $response;
    }

    $this->curl->get(GITHUB_API_URI . '/' . $command, $params);
    $response =  $this->getResponse();
    $this->cacheRequest($command, $params);
    return $response;
  }

  protected function putRequest($command, $params = array()) {

    $params['committer'] = $this->getCommittrer();
    $this->prepareCommand($command, $params);

    $query='';
    if($this->getToken()){
      $query = '?access_token=' . $this->getToken();
    }

    $this->curl->put(GITHUB_API_URI . '/' . $command . $query, $params);
    $response =  $this->getResponse();
    return $response;
  }

  protected function postRequest($command, $params = array()) {

    $this->prepareCommand($command, $params);
    $query='';
    if($this->getToken()){
      $query = '?access_token=' . $this->getToken();
    }
    $this->curl->post(GITHUB_API_URI . '/' . $command . $query, $params);
    $response =  $this->getResponse();
    return $response;
  }

  protected function deleteRequest($command, $params = array()) {

    $this->prepareCommand($command, $params);

    $query='';
    if($this->getToken()){
      $query = '?access_token=' . $this->getToken();
    }

    $this->curl->delete(GITHUB_API_URI . '/' . $command . $query, $params);
    $response =  $this->getResponse();
    return $response;
  }


  private function prepareCommand(&$command, &$params) {
    foreach($params as $key => $val){
      if($key[0] == ':'){
        $command = str_replace($key, $val, $command);
        unset($params[$key]);
      }
    }
  }

  public function getOrgs() {
    return $this->getRequest('user/orgs');
  }

  public function getUser($user_name) {
    return $this->getRequest(
        'users/:username', array(
        ':username' => $user_name,
        )
    );
  }

  public function getUserRepos($user_name) {
    return $this->getRequest(
        'users/:username/repos', array(
        ':username' => $user_name,
        )
    );
  }

  public function getOrg($org_name) {
    return $this->getRequest(
        'orgs/:org', array(
        ':org' => $org_name,
        )
    );
  }

  public function getOrgsRepos($org_name) {
    return $this->getRequest(
        'orgs/:org/repos', array(
        ':org' => $org_name,
        )
    );
  }

  public function getRepo() {
    return $this->getRequest(
        'repos/:owner/:repo', array(
        ':owner' => $this->owner_name,
        ':repo' => $this->repo_name,
        )
    );
  }

  public function createHook($settings) {
    $settings[':owner'] = $this->owner_name;
    $settings[':repo'] = $this->repo_name;
    return $this->postRequest('repos/:owner/:repo/hooks', $settings);
  }

  public function deleteHook($id) {
    return $this->deleteRequest(
        'repos/:owner/:repo/hooks/:id', array(
        ':owner' => $this->owner_name,
        ':repo' => $this->repo_name,
        ':id' => $id,
        )
    );
  }


  public function createOrgHook($settings) {
    $settings[':org'] = $this->owner_name;
    return $this->postRequest('orgs/:org/hooks', $settings);
  }

  public function deleteOrgHook($id) {
    return $this->deleteRequest(
        'orgs/:org/hooks/:id', array(
        ':org' => $this->owner_name,
        ':id' => $id,
        )
    );
  }

  public function getCommit($sha) {
    return $this->getRequest(
        'repos/:owner/:repo/commits/:sha', array(
        ':owner' => $this->owner_name,
        ':repo' => $this->repo_name,
        ':sha' => $sha,
        )
    );
  }
  
  public function createComment($settings) {
    $settings[':owner'] = $this->owner_name;
    $settings[':repo'] = $this->repo_name;
    return $this->postRequest('repos/:owner/:repo/commits/:sha/comments', $settings);
  }

  public function createIssueComment($settings) {
    $settings[':owner'] = $this->owner_name;
    $settings[':repo'] = $this->repo_name;
    return $this->postRequest('repos/:owner/:repo/issues/:number/comments', $settings);
  }
  
  public function getReadme(){
    $readme = $this->getRequest('repos/:owner/:repo/readme');
    if(!empty($readme->content)){
      return base64_decode($readme->content);
    }
    return FALSE;
  }


}
