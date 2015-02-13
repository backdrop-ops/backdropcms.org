<?php
/**
 * Page callbacks for BackdropCMS.org GitHub integration.
 */

/**
 * Page callback for listening to all GitHub events on a contrib project.
 */
function borg_github_callback() {
  watchdog('GitHub callback received. Post: <pre>@post</pre> Args: <pre>@args</pre>', array('@post' => backdrop_var_export($_POST)), '@args' => backdrop_var_export(func_get_args())));
}

/**
 * Access callback for all GitHub event webhooks.
 */
function borg_github_access() {
  return TRUE;
}
