<?php
/**
 * Template file for the Ready page.
 *
 * Available variables:
 * - $preview_id: The ID of the Tugboat preview (can be used to make API calls
 *   to get more data about the preview site).
 * - $url: The URL of the new preview site.
 * - $age: How long the preview site will be available for.
 */
?>

<div class="tugboat-ready-page">
  <div class="tugboat-ready-intro">
    <p><?php print t('Your new preview site is ready! You can access it at:'); ?></p>
    <p class="tugboat-ready-url"><?php print l($url, $url); ?></p>
  </div>

  <div class="tugboat-ready-button">
    <?php print l(t('Visit site'), $url, array('attributes' => array('class' => array('button')))); ?>
  </div>

  <div class="tugboat-ready-age">
    <p><?php print t('This preview site will be available for <strong>@age</strong>. After this it will be automatically deleted.', array('@age' => $age)); ?></p>
  </div>
</div>
