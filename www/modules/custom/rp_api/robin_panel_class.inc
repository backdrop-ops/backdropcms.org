<?php
/**
 * @file
 * Class RobinPanel.
 *
 * Add event to EHD.
 */

class RobinPanel {
  protected $user;
  protected $password;
  protected $host;
  protected $port;
  protected $debug;
  public $sid;
  public $aid;

  function __construct($user = NULL, $password = NULL, $host = NULL, $port = NULL, $debug = FALSE) {
    set_time_limit(0);

    include_once 'XML/Serializer.php';
    include_once 'XML/Unserializer.php';

    call_user_func_array(array($this, 'init'), func_get_args());
  }

  public function init($user = NULL, $password = NULL, $host = NULL, $port = NULL, $debug = 0) {
    $res = '';
    if (is_null($user)) {
      $res = 'RobinPanel: give parameter user.';
    }

    if (is_null($password)) {
      $res = 'RobinPanel: give parameter password.';
    }

    if (is_null($host)) {
      $res = 'RobinPanel: give parameter host.';
    }

    $this->user = $user;
    $this->password = $password;
    $this->host = 'ssl://' . $host;
    $this->port = (is_null($port)) ? 10010 : $port;
    $this->debug = $debug;

    return $res;
  }

  public function getUsername() {
    return $this->user;
  }

  public function getPassword() {
    return $this->password;
  }

  protected function parseXML($data) {
    $options = array(
      'parseAttributes' => FALSE
    );

    $unserializer = new XML_Unserializer($options);
    $unserializer->unserialize($data);

    $data = $unserializer->getUnserializedData();

    return $data;
  }

  protected function createXML($data) {
    $options = array(
      "indent" => "  ",
      "linebreak" => "\n",
      "addDecl" => FALSE,
      "addDoctype" => TRUE,
      "rootName" => 'ehd',
      "defaultTagName" => 'item',
      XML_SERIALIZER_OPTION_MODE => XML_SERIALIZER_MODE_SIMPLEXML,
    );

    $serializer = new XML_Serializer($options);
    $serializer->serialize($data);
    $data = $serializer->getSerializedData();

    return $data;
  }

  public function execute($command, $data, $message_return = FALSE, $now = 360) {
    $handle = NULL;
    $error_message = '';
    $result = '';

    $data_arr['password'] = $this->password;
    $data_arr['username'] = $this->user;
    $data_arr['cmd'] = $command;
    $data_arr['waittime'] = $now;
    $data_arr['data'] = $data;

    $data_line = $this->createXML($data_arr);

    if ($this->debug) {
	    $log_file = '/tmp/rp.log';
	    if(isset($this->logfile) && !empty($this->logfile)){
		    $log_file = $this->logfile;	
	    }
      $handle = fopen($log_file, 'a');
      $error_message = "[$command]\n";
      $error_message .= print_r($data_arr, TRUE);
      $error_message .= $data_line;
    }

    $socket = fsockopen($this->host, $this->port, $errno, $errstr);

    if (!$socket) {
      if ($this->debug) {
        $error_message .= "Socket error: $errno - $errstr ";
        $error_message .= "\n---------------\n";
        fwrite($handle, $error_message);
        fclose($handle);
      }

      return '';
    }

    stream_set_timeout($socket, 36000);
    stream_set_write_buffer($socket, 0);

    if ($this->debug) {
      $error_message .= "socket state:" . print_r(stream_get_meta_data($socket), TRUE) . "\n";
    }

    fwrite($socket, $data_line . "\0");
    if ($this->debug) {
      $error_message .= $data_line . "\n";
      $error_message .= "socket state:" . print_r(stream_get_meta_data($socket), TRUE) . "\n";
    }
    $buf = "";

    $once = 1;
    while ($peace = fgets($socket, 81920)) {
      $buf .= $peace;
      if ($this->debug) {
        $error_message .= "received[" . $peace . "]\n";

        if ($command == 'SetCronjob' && $once) {
          $error_message .= "socket state:" . print_r(stream_get_meta_data($socket), TRUE) . "\n";
          $once = 0;
        }
      }
    }

    if ($this->debug) {
      $error_message .= "received[" . $buf . "]\n";
      $error_message .= "socket state:" . print_r(stream_get_meta_data($socket), TRUE) . "\n";
    }

    if (!empty($buf)) {
      $result = $this->parseXML($buf);
    }

    if ($this->debug) {
      $error_message .= "\n---------------\n";
      fwrite($handle, $error_message);
      fclose($handle);
    }

    return $this->resultTransform($result, $message_return);
  }

  protected function resultTransform($result, $message_return = FALSE) {
    if (isset($result->error)) {
      if ($error = $this->errorByCodeGet((int) $result['code'])) {
        ;
      }

      return $error;
    }

    $data = $result;

    if ($message_return) {
      if ($message = $this->messageByCodeGet((int) $result['code'])) {
        $data = array(
          'Data' => $data,
          'Message' => $message,
        );
      }
    }

    return $data;
  }

  protected function messageByCodeGet($code) {
    $messages_arr = array(
      '1001' => 'Command is accepted',
      '1002' => 'Command is executed',
      '1003' => 'Command is in exec process',
    );

    if (!isset($messages_arr[$code])) {
      return 'Unknown Message code ' . $code;
    }

    return $messages_arr[$code];
  }

  protected function errorByCodeGet($code) {
    $errors_arr = array(
      '1001' => 'Command execution error',
      '1002' => 'Data transsmition error',
      '1003' => 'Login or password error',
      '1004' => 'End waiting error',
      '1005' => 'Command wasn\'t accepted',
    );

    if (!isset($errors_arr[$code])) {
      return 'Unknown error code ' . $code;
    }

    return $errors_arr[$code];
  }

  /**
   * @param int $taskid
   *   Task ID.
   *
   * @return array
   */
  public function cmdStatusGet($taskid) {
    return $this->execute('CmdStatusGet', array('taskid' => $taskid), FALSE, 0);
  }

  public function showUserInfo($username, $message_return = FALSE, $wait = 0) {
    return $this->execute('ShowUserInfo', array('user' => $username), $message_return, $wait);
  }

  /**
   * Create new domain.
   *
   * @param array $setting
   *   Data for new domain:
   *     - user
   *     - domain
   *     - wildcard
   *     - accesslog
   *     - errorlog
   *     - awstats
   *     - strict
   *     - ssl
   *     - ip
   *     - ns
   *
   * @return array
   *   Result create domain.
   */
  public function addDomain($setting) {
    $setting += array(
      'wildcard' => FALSE,
      'accesslog' => TRUE,
      'errorlog' => TRUE,
      'awstats' => FALSE,
      'strict' => FALSE,
      'ssl' => FALSE,
      'ip' => '*',
      'ns' => TRUE,
    );

    return $this->execute('AddDomain', $setting, FALSE, 0);
  }

  /**
   * @param array $setting
   *   - user
   *   - domain
   *   - ns
   *
   * @return array
   */
  public function delDomain($setting) {
    $setting += array(
      'ns' => TRUE,
    );

    return $this->execute('DelDomain', $setting, FALSE, 0);
  }

  /**
   * @param array $setting
   *   - user
   *   - domain
   *   - parkdomain
   *   - ip
   *   - ns
   *
   * @return array
   */
  public function addParked($setting) {
    $setting += array(
      'ip' => '*',
      'ns' => TRUE,
    );

    return $this->execute('AddParked', $setting, FALSE, 0);
  }

  /**
   * @param array $setting
   *   - user
   *   - parkdomain
   *   - ns
   *
   * @return array
   */
  public function delParked($setting) {
    $setting += array(
      'ns' => TRUE,
    );

    return $this->execute('DelParked', $setting, FALSE, 0);
  }


  public function addFTPUser($user, $domain, $ftpuser, $password, $quota, $dir) {
    $setting = array();
    $setting['user'] = $user;
    $setting['domain'] = $domain;
    $setting['ftpuser'] = $ftpuser;
    $setting['password'] = $password;
    $setting['quota'] = $quota;
    $setting['dir'] = $dir;

    return $this->execute('AddFtpUser', $setting, FALSE, 0);
  }

  public function delFTPUser($user, $domain, $ftpuser) {
    $setting = array();
    $setting['user'] = $user;
    $setting['domain'] = $domain;
    $setting['ftpuser'] = $ftpuser;

    return $this->execute('DelFtpUser', $setting, FALSE, 0);
  }

  public function changeFtpPass($user, $domain, $ftpuser, $ftppass) {
    $setting = array();
    $setting['user'] = $user;
    $setting['domain'] = $domain;
    $setting['ftpuser'] = $ftpuser;
    $setting['ftppass'] = $ftppass;

    return $this->execute('ChangeFtpPass', $setting, FALSE, 0);
  }

  public function changeFtpQuota($user, $domain, $ftpuser, $ftpquota) {
    $setting = array();
    $setting['user'] = $user;
    $setting['domain'] = $domain;
    $setting['ftpuser'] = $ftpuser;
    $setting['ftpquota'] = $ftpquota;

    return $this->execute('ChangeFtpQuota', $setting, FALSE, 0);
  }

  public function addEmail($user, $domain, $email, $password, $quota) {
    $setting = array();
    $setting['user'] = $user;
    $setting['domain'] = $domain;
    $setting['email'] = $email;
    $setting['password'] = $password;
    $setting['quota'] = $quota;

    return $this->execute('AddEmail', $setting, FALSE, 0);
  }

  public function delEmail($user, $domain, $email) {
    $setting = array();
    $setting['user'] = $user;
    $setting['domain'] = $domain;
    $setting['email'] = $email;

    return $this->execute('DelEmail', $setting, FALSE, 0);
  }

  public function changeEmailPass($user, $domain, $email, $passwd) {
    $setting = array();
    $setting['user'] = $user;
    $setting['domain'] = $domain;
    $setting['email'] = $email;
    $setting['passwd'] = $passwd;

    return $this->execute('ChangeEmailPass', $setting, FALSE, 0);
  }

  public function changeEmailQuota($user, $domain, $email, $quota) {
    $setting = array();
    $setting['user'] = $user;
    $setting['domain'] = $domain;
    $setting['email'] = $email;
    $setting['quota'] = $quota;

    return $this->execute('ChangeEmailQuota', $setting, FALSE, 0);
  }

  public function mysqlAddDB($user, $db) {
    $setting = array();
    $setting['user'] = $user;
    $setting['db'] = $db;

    return $this->execute('MysqlAddDB', $setting, FALSE, 0);
  }

  public function mysqlDelDB($user = FALSE, $db) {
    $setting = array();
    $setting['user'] = !$user ? $this->user : $user;
    $setting['db'] = $db;

    return $this->execute('MysqlDelDB', $setting, FALSE, 0);
  }

  public function mysqlAddUser($user, $dbname, $dbuser, $dbpasswd, $dbhost, $dbacl) {
    $setting = array();
    $setting['user'] = $user;
    $setting['dbname'] = $dbname;
    $setting['dbuser'] = $dbuser;
    $setting['dbpasswd'] = $dbpasswd;
    $setting['dbhost'] = $dbhost;
    $setting['dbacl'] = $dbacl;

    return $this->execute('MysqlAddUser', $setting, FALSE, 0);
  }

  public function mysqlDelUser($user = FALSE, $dbuser, $host = FALSE) {
    $setting = array();
    $setting['user'] = !$user ? $this->user : $user;
    $setting['dbuser'] = $dbuser;
    $setting['host'] = !$host ? 'localhost' : $host;

    return $this->execute('MysqlDelUser', $setting, FALSE, 0);
  }

  public function mysqlUserGrant($user, $name, $dbuser, $host, $acl) {
    $setting = array();
    $setting['user'] = $user;
    $setting['name'] = $name;
    $setting['dbuser'] = $dbuser;
    $setting['host'] = $host;
    $setting['acl'] = $acl;

    return $this->execute('MysqlUserGrant', $setting, FALSE, 0);
  }

  public function changePassByUser($user, $oldpassword, $password) {
    $setting = array();
    $setting['user'] = $user;
    $setting['oldpassword'] = $oldpassword;
    $setting['password'] = $password;

    return $this->execute('ChangePassByUser', $setting, FALSE, 0);
  }

  public function addUser($setting, $message_return = FALSE, $wait = 0) {
    if (empty($setting)) {
      return FALSE;
    }

    $account = array(
      'demo' => 0,
      'type' => 'user',
      'quotas' => array(
        'domain' => -1,
        'parkdomain' => -1,
        'email' => -1,
        'ftp' => -1,
        'mysql' => -1,
        'ips' => '*',
        'shell' => FALSE,
      ),
    );

    foreach($setting as $key => $val) {
      $account[$key] = $val;
    }

    return $this->execute('AddUser', $account, $message_return, $wait);
  }

  /**
   * Delete user.
   *
   * @param string $user
   *   Username.
   *
   * @param bool $ns
   *   Status remove DNS.
   *
   * @return array|string
   */
  public function delUser($user, $ns = TRUE) {
    $setting = array();
    $setting['user'] = $user;
    $setting['quotas'] = $ns;

    return $this->execute('DelUser', $setting, FALSE, 0);
  }

  public function changeUserQuota($user, $quotas) {
    $setting = array();
    $setting['user'] = $user;
    $setting['quotas'] = $quotas;

    return $this->execute('ChangeUserQuota', $setting, FALSE, 0);
  }

  public function changeUserPassword($user, $password) {
    $setting = array();
    $setting['user'] = $user;
    $setting['password'] = $password;

    return $this->execute('ChangeUserPass', $setting, FALSE, 0);
  }
}
