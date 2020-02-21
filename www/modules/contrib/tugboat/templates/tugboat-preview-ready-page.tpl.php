<?php
/**
 * Template file for the Ready page.
 *
 * Available variables:
 * - $url: The URL of the new preview site.
 * - $age: How long the preview site will be available for.
 */
?>

<div class="tugboat-ready-page">
  <div class="tugboat-ready-intro">
    <p>Your new preview site is ready! You can access it at:</p>
    <p class="tugboat-ready-url"><?php print l($url, $url); ?></p>
  </div>

  <div class="tugboat-ready-button">
    <?php print l(t('Visit site'), $url, array('attributes' => array('class' => array('button')))); ?>
  </div>

  <div class="tugboat-ready-age">
    <p>This preview site will be available for <strong><?php print $age; ?></strong>. After this it will be automatically deleted.</p>
  </div>
</div>
