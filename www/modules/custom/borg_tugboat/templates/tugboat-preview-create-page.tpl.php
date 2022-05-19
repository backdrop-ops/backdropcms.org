<?php
/**
 * Template file for the Create page.
 *
 * Available variables:
 * - $form: The form array for the 'Create' button.
 */
?>

<div class="borg-demo-create-page">
  <h2><?php print t("See what it's like to use Backdrop CMS."); ?></h2>
  <p class="borg-demo-create-intro"><?php print t("Use your own, free, demonstration website to take Backdrop CMS for a spin."); ?></p>

  <?php print render($form); ?>
  <p><?php print t("Creating a new sandbox may take a moment."); ?></p>

  <hr>

  <p><?php print t("Or try out the very latest changes with our:"); ?></p>
  <p><?php print l(t('Dev preview'), 'demo/dev', array(
    'attributes' => array(
      'class' => array('button'),
    ),
  )); ?></p>
</div>
