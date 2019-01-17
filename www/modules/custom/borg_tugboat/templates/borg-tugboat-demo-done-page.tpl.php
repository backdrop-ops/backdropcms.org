<?php
/**
 * Template for the demo is created page.
 */
?>

<div class="borg-demo-done-page">
  <p class="borg-demo-thank-you">
    Thank you for creating a new demo site! You can access your demo at:
  </p>

  <p class="borg-demo-url">
    <?php print l($url, $url); ?>
  </p>
  <p class="borg-demo-button">
    <?php print l(t('Visit your site'), $url, array('attributes' => array('class' => array('button')))); ?>
  </p>

  <p class="borg-demo-persist-notice">
    Your demo site will persist for <strong><?php print $duration; ?></strong>.
  </p>

  <p class="borg-demo-email-notice">
    This demo acts exactly as a new Backdrop installation as it would if you
    downloaded it, including running the installer. You'll set an administrator
    account as part of the setup. Note that sending email from a demo sandbox
    is not allowed, so <strong>password recovery will not work</strong>. Be sure
    to remember your password.</p>
</div>