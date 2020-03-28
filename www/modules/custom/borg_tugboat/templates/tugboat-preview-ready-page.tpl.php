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

<div class="borg-demo-done-page">
  <div class="borg-demo-done-center">
    <p class="borg-demo-thank-you">
      Thank you for creating a new demo sandbox. You can access your new
      Backdrop website here:
    </p>

    <p class="borg-demo-button">
      <?php print l(t('Visit your site'), $login_url, array('attributes' => array('class' => array('button button-large'), 'target' => '_blank'))); ?>
    </p>

    <p class="borg-demo-button-description">
      The button above will take you to the site and automatically log you in as
      the admin user. You may, however, like to note the following login details
      for future reference:
    </p>

    <p class="borg-demo-url">
      <strong>URL:</strong> <?php print l($url, $url, array('attributes' => array('target' => '_blank'))); ?><br />
      <strong>Username:</strong> admin<br />
      <strong>Password:</strong> password
    </p>

    <p class="borg-demo-persist-notice">
      This demo sandbox will persist for <strong><?php print $age; ?></strong>.
    </p>
  </div>

  <div class="borg-demo-done-left">
    <p>
      Note that sending emails from demo sandboxes is not allowed, so
      <strong>password recovery will not work</strong>. If you change the admin
      password, be sure to remember it.
    </p>
    <p>
      Demo sandboxes are setup using the <em>Standard</em> install. Additional
      add-ons can be installed using the Installer module (Administration >
      Functionality > Install new modules).
    </p>
    <p>
      There is no database export functionality in the demo sandbox. <strong>Any
      work you do will be temporary</strong> and will be deleted after
      <?php print $age; ?>.
    </p>

    <figure class="borg-demo-tugboat-information">
      <a href="https://tugboat.qa" target="_blank">
        <?php print theme('image', array(
          'path' => backdrop_get_path('module', 'borg_tugboat') . '/images/tugboat-logo.png',
          'alt' => 'Tugboat QA Logo',
          'width' => 800,
          'height' => 300,
          'attributes' => array('class' => array('tugboat-logo')),
        )); ?>
      </a>
      <figcaption>
        Backdrop CMS demo sandboxes are provided by
        <a href="http://tugboat.qa">Tugboat.qa</a>, a service that can create
        on-demand site previews for pull requests. To learn more about Tugboat,
        visit <a href="https://tugboat.qa">https://tugboat.qa</a>.
      </figcaption>
    </figure>

    <p>
      Interested in how these sandboxes are built or have ideas for making them
      better? You can <a href="https://github.com/backdrop-contrib/tugboat">view
      the source code for this functionality</a> in the GitHub repository. File
      an issue or pull request in the
      <a href="https://github.com/backdrop-contrib/tugboat/issues">issue
      tracker</a>.
    </p>
  </div>
</div>
