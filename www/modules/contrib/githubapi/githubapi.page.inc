<?php
/**
 * @file
 * Administration pages provided by github API module.
 */

/**
 * Menu callback for admin/config/system/githubapi/list.
 */
function githubapi_repos() {
  $repos = db_select('githubapi_repositories', 'gr')
    ->fields('gr')
    ->orderBy('id', 'DESC')
    ->execute()
    ->fetchAll();

  $rows = array();
  foreach ($repos as $repo) {
    $row = array();
    $row[] = array('data' => $repo->owner);
    $row[] = array('data' => $repo->name);
    $links['delete'] = array(
      'title' => t('Delete'),
      'href' => "admin/config/system/githubapi/" . $repo->id . "/delete",
    );
    $links['download'] = array(
      'title' => t('Download'),
      'href' => "admin/config/system/githubapi/" . $repo->id . "/download",
    );
    $row[] = array(
      'data' => array(
        '#type' => 'operations',
        '#links' => $links,
      ),
    );
    $rows[] = $row;
  }
  $output['form'] = backdrop_get_form('githubapi_hook');
  $header = array(t('Owner'), t('Repository'), t('Operations'));

  $output['table'] = array(
    '#theme' => 'table__githubapi_repos',
    '#rows' => $rows,
    '#header' => $header,
    '#empty' => t('No github repo hooked yet.'),
  );
  return $output;
}

function githubapi_repo_download($form, &$form_state, $rid) {
  $repo = gitlc_github_rid_load($rid);

  $form_state['repo'] = $repo;

  $githubapi = githubapi_get_class($repo);

  if ($releases = $githubapi->getReleases()) {
    $options = array();

    foreach ($releases as $release) {
      $options[$release->tag_name] = $release->name;
    }

    /**
     * form
     *
     * @var mixed
     * @access public
     */
    $form['release'] = array(
      '#type' => 'select',
      '#title' => t('Select release to download'),
      '#options' => $options,
      '#description' => t('Please select release to download.'),
    );
    $form['actions']['#type'] = 'actions';


    $form['actions']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Download'),
    );

  }
  else {
    backdrop_set_message(t('This repo contain no releases'), 'error');
  }
  return $form;
}

/**
 * Submit handler for the githubapi_repo_download_hook() form.
 */
function githubapi_repo_download_submit($form, &$form_state) {
  $repo = $form_state['repo'];

  $githubapi = githubapi_get_class($repo);
  if ($code = $githubapi->getArchive(array(':ref' => $form_state['values']['release']))) {
    backdrop_add_http_header('Pragma', 'public');
    backdrop_add_http_header('Expires', '0');
    backdrop_add_http_header('Cache-Control', 'must-revalidate, post-check=0, pre-check=0');
    backdrop_add_http_header('Content-Type', 'application/zip');
    backdrop_add_http_header('Content-Disposition', 'attachment; filename=' . $form_state['values']['release'] . '.zip' . ';');
    backdrop_add_http_header('Content-Transfer-Encoding', 'binary');
    backdrop_add_http_header('Content-Length', strlen($code));
    echo $code;
    backdrop_exit();
  }

}

function githubapi_hook_remove($form, &$form_state, $rid) {
  $repo = gitlc_github_rid_load($rid);

  $hook = db_select('githubapi_hooks', 'gh')
    ->fields('gh')
    ->condition('rid', $rid)
    ->execute()
    ->fetchAssoc();

  if (empty($repo)) {
    backdrop_goto('admin/config/system/githubapi/list');
  }

  $form_state['repo'] = $repo;
  $form_state['hook'] = $hook;

  $form['message'] = array(
    '#markup' => t('Are you sure want to delete this repo from this app?'),
  );
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
  );

  return $form;
}

/**
 * Submit handler for the githubapi_hook_remove() form.
 */
function githubapi_hook_remove_submit($form, &$form_state) {
  $repo = $form_state['repo'];
  $hook = $form_state['hook'];

  module_load_include('inc', 'githubapi', 'githubapi_class');
  $config = config('githubapi.settings');

  $token = settings_get('githubapi_token');
  if (empty($token)) {
    $token = $config->get('token');
  }

  $githubapi = new GitHubAPI();

  $githubapi->setOwnerName($repo['owner']);
  $githubapi->setRepoName($repo['name']);
  $githubapi->setToken($token);

  if ($repo['name'] == '*') {
    $answer = $githubapi->deleteOrgHook($hook['hook_id']);
  }
  else {
    $answer = $githubapi->deleteHook($hook['hook_id']);
  }
  if (empty($answer)) {
    db_delete('githubapi_hooks')
      ->condition('rid', $repo['id'])
      ->execute();

    db_delete('githubapi_repositories')
      ->condition('id', $repo['id'])
      ->execute();

    backdrop_set_message(t('Hook deleted!'));
  }
  else {
    backdrop_set_message(t('We do not have permissions to remove hook!'), 'error');
  }
}

function githubapi_hook() {
  $form['owner_repo_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Please provide owner or owner/repo'),
    '#description' => t('Please provide owner or owner/repo to install webhook.'),
  );
  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Hook repo'),
  );

  return $form;

}

/**
 * Submit handler for the githubapi_hook() form.
 */
function githubapi_hook_submit($form, &$form_state) {

  module_load_include('inc', 'githubapi', 'githubapi_class');
  $config = config('githubapi.settings');
  $token = settings_get('githubapi_token');
  if (empty($token)) {
    $token = $config->get('token');
  }

  $githubapi = new GitHubAPI();
  $githubapi->setToken($token);

  global $base_url;
  global $base_path;

  $hook = FALSE;
  $secret = backdrop_random_key();

  $is_owner_repo = strpos($form_state['values']['owner_repo_path'], '/');
  if (FALSE !== $is_owner_repo) {
    // Hook a repo.
    $owner_repo = explode("/", $form_state['values']['owner_repo_path']);
    $githubapi->setOwnerName($owner_repo[0]);
    $githubapi->setRepoName($owner_repo[1]);

    if ($data = $githubapi->getRepo()) {
      // Make sure we do have access to repo.
      $settings = array(
        'name' => GITHUB_HOOK_NAME,
        'config' => (object) array(
          'url' => $base_url . $base_path . GITHUB_PAYLOAD_PATH,
          'content_type' => GITHUB_PAYLOAD_CONTENT_TYPE,
          'secret' => $secret,
        ),
        'events' => array(
          'push',
          'pull_request',
          'create',
          'delete',
          'release',
          'issues',
          'issue_comment',
          'follow',
          'fork',
          'status',
          'commit_comment',
          'pull_request_review_comment'
        ),
        'active' => TRUE,
      );
      $githubapi->reInitCurl();
      $hook = $githubapi->createHook($settings);
    }
  }
  else {
    // Hook an user or organisation.
    $org = $form_state['values']['owner_repo_path'];
    $org_data = $githubapi->getOrg($org);
    if ($org_data->message == 'Not Found') {
      backdrop_set_message(t('There is no !org organisation', array('!org' => $org)), 'error');
      return;
    }
    $githubapi->setOwnerName($org);
    $settings = array(
      'name' => GITHUB_HOOK_NAME,
      'config' => (object) array(
        'url' => $base_url . $base_path . GITHUB_PAYLOAD_PATH,
        'content_type' => GITHUB_PAYLOAD_CONTENT_TYPE,
        'secret' => $secret,
      ),
      'events' => array(
        'push',
        'pull_request',
        'create',
        'delete',
        'release',
        'issues',
        'issue_comment',
        'follow',
        'fork',
        'status',
        'commit_comment',
        'pull_request_review_comment'
      ),
      'active' => TRUE,
    );
    $githubapi->reInitCurl();
    $hook = $githubapi->createOrgHook($settings);
    if ($hook->message == 'Not Found') {
      backdrop_set_message(t('You don\'t have right to add hook to !org organisation', array('!org' => $org)), 'error');
      return;
    }

    $data = new stdClass;
    $data->id = 0;
    $data->owner->login = $org;
    $data->name = '*';
    $data->private = TRUE;
    $data->cache = $org_data;
  }


  if ($hook) {
    $rid = db_insert('githubapi_repositories')
      ->fields(
        array(
          'repo_id' => $data->id,
          'owner' => $data->owner->login,
          'name' => $data->name,
          'private' => (int) $data->private,
          'cache' => isset($data->cache) ? serialize($data->cache) : serialize($data),
          'secret' => $secret,
          'timestamp' => REQUEST_TIME,
        )
      )
      ->execute();

    db_insert('githubapi_hooks')
      ->fields(
        array(
          'rid' => $rid,
          'hook_id' => $hook->id,
          'child' => (int) ($hook->url === TRUE),
          'timestamp' => REQUEST_TIME,
        )
      )
      ->execute();
    if (FALSE !== $is_owner_repo) {
      backdrop_set_message(t('Repository !owner/!repo hooked', array(
        '!owner' => $owner_repo[0],
        '!repo' => $owner_repo[1]
      )));
    }
    else {
      backdrop_set_message(t('Owner !owner hooked', array('!owner' => $form_state['values']['owner_repo_path'])));
    }

  }
}
