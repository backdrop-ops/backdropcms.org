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
 * Submit handler for the githubapi_hook() form.
 */
function gitlc_webhook_hook_submit($form, &$form_state) {
  
}
