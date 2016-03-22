<?php

function gitlc_webhook_admin_list(){
  $repos = db_select('gitlc_webhook_repositories', 'wr')
    ->fields('wr')
    ->orderBy('owner', 'DESC')
    ->orderBy('repo', 'DESC')
    ->execute()
    ->fetchAll();

  $rows = array();
  foreach ($repos as $repo) {
    $row = array();
    $row[] = array( 'data' => $repo->owner );
    $row[] = array( 'data' => $repo->repo );
    $links['delete'] = array(
      'title' => t('Delete'),
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

function gitlc_webhook_hook(){
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
  if(count($owner_repo) != 2){
    form_set_error('owner_repo_path', t('Please provide info in OWNER/REPO format'));
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
  
  if(empty($hook)){
    // Setup GitHub webhook for Gitlc and signup for integration with Gitlc.
    $setup_settings = array(
      'url' => url(GITLC_WEBHOOK_PAYLOAD_PATH),
      'secret' => $secret,
    );
    $GitlcAPI->reInitCurl();
    $hook = $GitlcAPI->createHook($setup_settings); 
  }else{
    backdrop_set_message('<pre>' . htmlentities(print_r($hook,true)) . '</pre>');

    // Signup for integration with Gitlc.
    $setup_settings = array(
      'url' => url(GITLC_WEBHOOK_PAYLOAD_PATH),
      'secret' => $secret,
    );
    $GitlcAPI->reInitCurl();
    $hook = $GitlcAPI->updateHook($setup_settings); 
  }
  backdrop_set_message('<pre>' . htmlentities(print_r($hook,true)) . '</pre>');
}
