#Githubapi - Github API integration module
This module provide API and settings to connect your website with your github organisations and repositories via github API and Github Application.

## Under active development
It's early alpha release. GitHubAPI class is limited now to fullfill backdropcms.org needs only.
When issue #8 will be implemented, I am going to release Beta1.


##Installation
  - Install this module using the official Backdrop CMS instructions at
  https://backdropcms.org/guide/modules
  - Install https://github.com/php-curl-class/php-curl-class to libraries
  
  ```bash
  cd libraries
  git clone https://github.com/php-curl-class/php-curl-class.git
  ```
  - Create Github Application via https://github.com
    
    There is two way to do so. 
    - Via your profile: https://github.com/settings/applications/new
    - Via your organization profile: https://github.com/organizations/ORGNAME/settings/applications/new
  
  - Save settings on admin/config/system/githubapi/settings
  
    Optionally you can store client_id and client_secret via settings.php
    ```
    $settings['githubapi_client_id'] = 'YOUR_APP_CLIENT_ID';
    $settings['githubapi_client_secret'] = 'YOUR_APP_CLIENT_SECRET';
    ```
    When client_id or client_secret stored via settings.php , you are not able to edit it on settings page.
    
  - setup private filesystem path. We are caching each GET request to GithubAPI for AGE provided by answer.
  
  - Now you need to provide token to your github Application, and there is two way:
    - Just click "Authorize an Application" on admin/config/system/githubapi/settings page and grant access to your account token.
    This way any comments or other actions via GithubAPI will be from your name.
    
    - Create an account on github.com like "MyApp BOT". Then give this user admin access to your organization or/and your repositories. Then click "Authorize an Application" and do so via your "MyApp Bot" account.
    
##Using examples
 After you properly configured module, your website become a backend for your github APP. Now you can install webhook via admin/config/system/githubapi to your repositories or organisations.
 Type orgname or orgname/reponame into hook form at the top.
 When you click "Hook up", Webhook will be automatically created for your Organisation or Repository if "MyApp BOT" user (or whatever you used to Authorize your app) has admin access to this organisation or repository.
 
 Time to create custom module that implements hook_githubapi_payload($event_name, $record, $repo). Example:
 ```
/**
 * Implements hook_githubapi_payload().
 */
function github_issues_githubapi_payload($event_name, $record, $repo){
  $expr = '/(?<!\S)#([0-9]*)/i';

  switch ($event_name) {
    case 'push':
      foreach($payload['data']['commits'] as $commit){
        $match = NULL;
        $message = '';
        preg_match_all($expr, $commit['message'], $match);
        if(!empty($match[1])){
          foreach($match[1] as $issue){
            $message .= GITHUB_ISSUES_ISSUES_REPO . '#' . $issue . " ";
          }
          if(!empty($message)){
            borg_github_issues_commit_comment_add($commit['id'], $message, $repo);
          }
        }
      }
      break;
    case 'pull_request':
      if($payload['data']['action'] == 'opened'){
        $message = '';
        preg_match_all($expr, $payload['data']['pull_request']['title'] . ' ' . $payload['pull_request']['body'], $match);
        if(!empty($match[1])){
          foreach($match[1] as $issue){
            $message .= GITHUB_ISSUES_ISSUES_REPO . '#' . $issue . " ";
          }

          if(!empty($message)){
            borg_github_issues_issue_comment_add($payload['number'], $message, $repo);
          }
        }
      }
      break;
  }
}
```


##License
This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.
