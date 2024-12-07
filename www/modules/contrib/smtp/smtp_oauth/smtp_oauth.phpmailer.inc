<?php
/**
 * PHPMailerOAuth - PHP email transport class
 * NOTE: Requires PHP version 5 or later
 * @package PHPMailer
 * @copyright 2004 - 2009 Andy Prevost
 */

class PHPMailerOAuth extends PHPMailer {

  /**
   * The OAuth user's email address
   * @type string
   */
  public $oauthUserEmail = '';

  /**
   * The Google client object.
   * @type \Google_Client
   */
  public $googleClient = '';

  /**
   * @type string
   */
  public $AuthType;

  /**
   * @type array
   */
  public $SMTPOptions;

  /**
   * An instance of the GmailXOAuth2 class.
   * @type GmailXOAuth2
   * @access protected
   */
  protected $oauth = null;

  /**
   * Constructor.
   *
   * @param bool $exceptions Should we throw external exceptions?
   */
  public function __construct($userEmail, $googleClient) {
    $this->oauthUserEmail = $userEmail;
    $this->googleClient = $googleClient;
    $this->isSMTP();
    //Set AuthType to use XOAUTH2
    $this->AuthType = 'XOAUTH2';

    // Whether to use SMTP authentication
    $this->SMTPAuth = TRUE;

    //Set the encryption mechanism to use - STARTTLS or SMTPS
    //$this->SMTPSecure = PHPMailer::ENCRYPTION_STARTTLS;

    //Set the hostname of the mail server
    $this->Host = 'smtp.gmail.com';

    //Set the SMTP port number - 587 for authenticated TLS, a.k.a. RFC4409 SMTP submission
    $this->Port = 587;

    parent::__construct();
  }

  /**
   * Get an OAuth instance to use.
   * @return GmailXOAuth2
   */
  public function getOAUTHInstance() {
    if (!is_object($this->oauth)) {
      /* this is the only part that differs,
       * we create an object of our class GmailXOAuth2 instead of the original OAuth class
       */
      $this->oauth = new GmailXOAuth2 (
        $this->oauthUserEmail,
        $this->googleClient
      );
    }
    return $this->oauth;
  }

  /**
   * Initiates a connection to an SMTP server.
   * Returns FALSE if the operation failed.
   * @param array $options An array of options compatible with stream_context_create()
   * @uses SMTP
   * @access public
   * @return bool
   */
  public function SmtpConnect($options = null) {
    if (is_null($this->smtp)) {
      $this->smtp = $this->getSMTPInstance();
    }

    if (is_null($this->oauth)) {
      $this->oauth = $this->getOAUTHInstance();
    }

    // if no options are provided, use whatever is set in the instance
    if (is_null($options)) {
        $options = $this->SMTPOptions;
    }

    $this->smtp->do_debug = $this->SMTPDebug;
    $hosts = explode(';', $this->Host);
    $index = 0;
    $connection = $this->smtp->Connected();
    $lastexception = NULL;

    // Retry while there is no connection
    while ($index < count($hosts) && !$connection) {
      try {
        $hostinfo = array();
        if (preg_match('/^(.+):([0-9]+)$/', $hosts[$index], $hostinfo)) {
          $host = $hostinfo[1];
          $port = $hostinfo[2];
        }
        else {
          $host = $hosts[$index];
          $port = $this->Port;
        }

        $tls = ($this->SMTPSecure == 'tls');
        $ssl = ($this->SMTPSecure == 'ssl');

        if ($this->smtp->Connect(($ssl ? 'ssl://':'') . $host, $port, $this->Timeout, $options)) {

          $hello = ($this->Helo != '' ? $this->Helo : $this->ServerHostname());
          $this->smtp->Hello($hello);

          if ($tls) {
            if (!$this->smtp->StartTLS()) {
              throw new phpmailerException(t('StartTLS not supported by server or could not initiate session.'));
            }

            //We must resend HELO after tls negotiation
            $this->smtp->Hello($hello);
          }

          $connection = TRUE;
          if ($this->SMTPAuth) {
            if (!$this->smtp->AuthenticateOAuth($this->oauth)) {
              throw new phpmailerException(t('SMTP Error: Could not authenticate.'));
            }
          }
        }
      } catch (phpmailerException $e) {
        if ($connection) {
          $this->SmtpClose();
          $connection = FALSE;
        }

        $lastexception = $e;
      }

      $index++;
    }
    if (!$connection) {
      if ($lastexception != NULL) {
        throw $lastexception;
      }
      else {
        throw new phpmailerException(t('SMTP Error: Could not connect to SMTP host.'));
      }
    }
    return TRUE;
  }

  /**
   * Get the OAuth instance.
   *
   * @return OAuth
   */
  public function getOAuth()
  {
    return $this->oauth;
  }

  /**
   * Set an OAuth instance.
   */
  public function setOAuth(OAuth $oauth)
  {
    $this->oauth = $oauth;
  }
  /**
   * Get an instance to use for SMTP operations.
   * Override this function to load your own SMTP implementation,
   * or set one with setSMTPInstance.
   *
   * @return SMTP
   */
  public function getSMTPInstance()
  {
    if (!is_object($this->smtp)) {
      $this->smtp = new SMTP();
    }

    return $this->smtp;
  }

  /**
   * Provide an instance to use for SMTP operations.
   *
   * @return SMTP
   */
  public function setSMTPInstance(SMTP $smtp)
  {
    $this->smtp = $smtp;

    return $this->smtp;
  }

  /**
   * Get the server hostname.
   * Returns 'localhost.localdomain' if unknown.
   *
   * @return string
   */
  protected function serverHostname()
  {
    $result = '';
    if (!empty($this->Hostname)) {
      $result = $this->Hostname;
    } elseif (isset($_SERVER) && array_key_exists('SERVER_NAME', $_SERVER)) {
      $result = $_SERVER['SERVER_NAME'];
    } elseif (function_exists('gethostname') && gethostname() !== false) {
      $result = gethostname();
    } elseif (php_uname('n') !== false) {
      $result = php_uname('n');
    }
    if (!static::isValidHost($result)) {
      return 'localhost.localdomain';
    }

    return $result;
  }
  /**
   * Validate whether a string contains a valid value to use as a hostname or IP address.
   * IPv6 addresses must include [], e.g. `[::1]`, not just `::1`.
   *
   * @param string $host The host name or IP address to check
   *
   * @return bool
   */
  public static function isValidHost($host)
  {
    //Simple syntax limits
    if (empty($host)
      || !is_string($host)
      || strlen($host) > 256
      || !preg_match('/^([a-zA-Z\d.-]*|\[[a-fA-F\d:]+])$/', $host)
    ) {
      return false;
    }
    //Looks like a bracketed IPv6 address
    if (strlen($host) > 2 && substr($host, 0, 1) === '[' && substr($host, -1, 1) === ']') {
      return filter_var(substr($host, 1, -1), FILTER_VALIDATE_IP, FILTER_FLAG_IPV6) !== false;
    }
    //If removing all the dots results in a numeric string, it must be an IPv4 address.
    //Need to check this first because otherwise things like `999.0.0.0` are considered valid host names
    if (is_numeric(str_replace('.', '', $host))) {
      //Is it a valid IPv4 address?
      return filter_var($host, FILTER_VALIDATE_IP, FILTER_FLAG_IPV4) !== false;
    }
    if (filter_var('http://' . $host, FILTER_VALIDATE_URL) !== false) {
      //Is it a syntactically valid hostname?
      return true;
    }

    return false;
  }

}
