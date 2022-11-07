<?php
/**
 * @file
 * Class for OAuth2 authorization of Google mail client.
 */

class GmailXOAuth2 extends OAuth {

  protected $oauthUserEmail = '';
  private $googleClient = '';

  public function __construct($UserEmail, $google_client) {
    $this->googleClient = $google_client;
    $this->oauthUserEmail = $UserEmail;
  }

  /**
   * @returns $google_client object
   */
  private function getClient() {
    return $this->googleClient;
  }

  /**
   * checks the credentials for the access token, if present; it returns that
   * or refreshes it if expired.
   * if the credentials file is empty, it will return the authorization url to which you must redirect too
   * for user user authorization
   */
  public static function authenticate () {
    $client = GmailXOAuth2::getClient();
    if ($client->isAccessTokenExpired()) {
      if ($refresh_token = $client->getRefreshToken()) {
        $client->fetchAccessTokenWithRefreshToken($refresh_token);
      }
      else {
        $client->fetchAccessTokenWithAssertion();
      }
    }
    $accessToken = $client->getAccessToken();
    return json_decode($accessToken, true);
  }

  /**
   * GetOauth64
   *
   * encode the user email related to this request along with the token in base64
   * this is used for authentication, in the phpmailer smtp class
   *
   * @return string
   */
  public function getOauth64 () {
    $accessToken = $this->googleClient->getAccessToken();
    return base64_encode("user=" . $this->oauthUserEmail . "\001auth=Bearer " . $accessToken['access_token'] . "\001\001");
  }

}
