<?php

function gitlc_webhook_admin_list() {
  $repos = db_select('gitlc_webhook_repositories', 'wr')
    ->fields('wr')
    ->orderBy('owner', 'DESC')
    ->orderBy('repo', 'DESC')
    ->execute()
    ->fetchAll();

  $rows = array();
  foreach ($repos as $repo) {
    $row = array();
    $row[] = array('data' => $repo->owner);
    $row[] = array('data' => $repo->repo);
    $links['delete'] = array('title' => t('Delete'),
      'href' => "admin/config/system/gitlc_webhook/" . $repo->id . "/delete",
    );
    $row[] = array(
      'data' => array(
        '#type' => 'operations',
        '#links' => $links,
      ),
    );
    $rows[] = $row;
  }
  $output['form'] = backdrop_get_form('gitlc_webhook_hook');
  $header = array(t('Owner'), t('Repository'), t('Operations'));

  $output['table'] = array(
    '#theme' => 'table__gitlc_webhook_repos',
    '#rows' => $rows,
    '#header' => $header,
    '#empty' => t('No API integration with GitLC for repositories yet.'),
  );
  return $output;
}

function gitlc_webhook_hook() {
  $form['owner_repo_path'] = array(
    '#type' => 'textfield',
    '#title' => t('Please provide owner/repo'),
    '#description' => t('Please put in format owner/repo.'),
  );
  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Hook repo'),
  );

  return $form;

}

/**
 * Validate handler for the gitlc_webhook_hook() form.
 */
function gitlc_webhook_hook_validate($form, &$form_state) {
  $owner_repo = explode("/", $form_state['values']['owner_repo_path']);
  if (count($owner_repo) != 2) {
    form_set_error('owner_repo_path', t('Please provide info in OWNER/REPO format'));
  }
  
  $webhook = db_select('gitlc_webhook_repositories', 'gh')
    ->fields('gh')
    ->condition('owner', $owner_repo[0])
    ->condition('repo', $owner_repo[1])
    ->execute()
    ->fetchObject();
    
    if($webhook) {
      form_set_error('owner_repo_path', t('This repo already hooked up.'));
    }
    
  // TODO. Need to validate if repo already hooked!
}

/**
 * Submit handler for the gitlc_webhook_hook() form.
 */
function gitlc_webhook_hook_submit($form, &$form_state) {
  $owner_repo = explode("/", $form_state['values']['owner_repo_path']);
  $settings = array(
    'owner' => $owner_repo[0],
    'repo' => $owner_repo[1]
  );
  $GitlcAPI = gitlc_webhook_get_class($settings);
  $hook = $GitlcAPI->getHook();
  $secret = backdrop_random_key();
  
  if(FALSE === $hook){
    return;
  }

  if (empty($hook)) {
    // Setup GitHub webhook for Gitlc and signup for integration with Gitlc.
    $setup_settings = array(
      'url' => url(GITLC_WEBHOOK_PAYLOAD_PATH, array('absolute' => TRUE)),
      'secret' => $secret,
    );
    $GitlcAPI->reInitCurl();
    $hook = $GitlcAPI->createHook($setup_settings);
  }
  else {
    // Signup for integration with Gitlc.
    $setup_settings = array(
      'url' => url(GITLC_WEBHOOK_PAYLOAD_PATH, array('absolute' => TRUE)),
      'secret' => $secret,
    );
    $GitlcAPI->reInitCurl();
    $hook = $GitlcAPI->updateHook($setup_settings);
  }
  
  if($hook){
    $webhook = array(
      'owner' => $owner_repo[0],
      'repo' => $owner_repo[1],
      'secret' => $secret,
      'public_key' => $hook['public_key'],
    );
    backdrop_write_record('gitlc_webhook_repositories', $webhook);
    backdrop_set_message(t('Gitlc webhook created for !owner/!repo', array(
      '!owner' => $owner_repo[0],
      '!repo' => $owner_repo[1]
    )));
    // Add ability to act on new created webhook.
    module_invoke_all('gitlc_webhook', 'created', $webhook);
  }
}

function gitlc_webhook_hook_remove($form, &$form_state, $hook_id){
  $webhook = db_select('gitlc_webhook_repositories', 'gh')
    ->fields('gh')
    ->condition('id', $hook_id)
    ->execute()
    ->fetchObject();

  if(empty($webhook)){
    backdrop_goto('admin/config/system/gitlc_webhook/list');
  }

  $form_state['webhook'] = $webhook;

  $form['message'] = array(
    '#markup' => t('Are you sure want to delete this webhook?'),
  );
  $form['actions']['#type'] = 'actions';
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
  );

  return $form;

}

/**
 * Submit handler for the githubapi_hook_remove() form.
 */
function gitlc_webhook_hook_remove_submit($form, &$form_state) {
  $webhook =  $form_state['webhook'];
  $settings = array(
    'owner' => $webhook->owner,
    'repo' => $webhook->repo,
  );
  $GitlcAPI = gitlc_webhook_get_class($settings);
  
  $result = $GitlcAPI->deleteHook();
  if($result !== FALSE && !is_array($result)){
    db_delete('gitlc_webhook_repositories')
      ->condition('id', $webhook->id)
      ->execute();
    backdrop_set_message(t('Hook deleted!'));
    // Add ability to act on deleted webhook.    
    module_invoke_all('gitlc_webhook', 'deleted', $webhook);    
  }else{
    backdrop_set_message(t('We got an error: !message', array('!message' => $GitlcAPI->getGitlcAPIErrorMessage())) . print_r($result,true), 'error');
  }
  backdrop_goto('admin/config/system/gitlc_webhook/list');
}
