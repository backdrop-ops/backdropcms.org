<?php
/**
 * Template for the demo is created page.
 */
?>

<div class="borg-demo-done-page">
  <p class="borg-demo-thank-you">
    Thank you for creating a new demo sandbox! You can access your website at:
  </p>

  <p class="borg-demo-url">
    <?php print l($url, $url); ?>
  </p>
  <p class="borg-demo-button">
    <?php print l(t('Visit your site'), $url, array('attributes' => array('class' => array('button button-large')))); ?>
  </p>

  <p class="borg-demo-persist-notice">
    This demo sandbox will persist for <strong><?php print $duration; ?></strong>.
  </p>

  <p class="borg-demo-install-notice">
    This demo sandbox will act exactly as a new Backdrop installation would.
    You will need to run the installer and set an administrator account as part
    of the setup.
  </p>
  <p class="borg-demo-email-notice">
    Note that sending email from demo sandboxis not allowed, so
    <strong>password recovery will not work</strong>. Be sure to remember your
    password.
  </p>
  <p class="borg-demo-addon-notice">
    Demos sandboxes will show you the Standard install only. There are no
    additional add-ons included, and no content beyond what comes with Backdrop.
    Additionally, <strong>the Installer module will not work</strong> so no
    additional add-ons can be added on.
  </p>
  <p class="borg-demo-time-notice">
    There is no export functionality in the demo sandbox. <strong>Any work you
    do will be temporary</strong> and be deleted after <?php print $duration; ?>.
  </p>

  <figure class="borg-demo-tugboat-information">
    <a href="https://tugboat.qa">
    <?php print theme('image', array(
      'path' => backdrop_get_path('module', 'borg_tugboat') . '/images/tugboat-logo.png',
      'alt' => 'Tugboat QA Logo',
      'width' => 800,
      'height' => 300,
      'attributes' => array('class' => array('tugboat-logo')),
    )); ?>
    </a>
    <figcaption>
      Backdrop CMS demo sandboxes are provided by <a href="http://tugboat.qa">Tugboat.qa</a>,
      a service that can create on-demand site previews for pull requests. To learn more
      about Tugboat, visit <a href="https://tugboat.qa">https://tugboat.qa</a>.
    </figcaption>
  </figure>

  <p class="borg-demo-contribute">
    Interested in how these sandboxes are built or have ideas for making them
    better? You can <a href="https://github.com/backdrop-ops/backdropcms.org/tree/master/www/modules/custom/borg_tugboat">view the source code for this functionality</a>
    in the GitHub repository. File an issue or pull request in the
    <a href="https://github.com/backdrop-ops/backdropcms.org/issues">BackdropCMS.org issue tracker</a>.
  </p>
</div>
