<?php
/**
 * @file
 * Display generic site information such as logo, site name, etc.
 *
 * Available variables:
 *
 * - $base_path: The base path of the Backdrop installation. At the very
 *   least, this will always default to /.
 * - $directory: The directory the template is located in, e.g. modules/system
 *   or themes/bartik.
 * - $is_front: TRUE if the current page is the front page.
 * - $logged_in: TRUE if the user is registered and signed in.
 * - $logo: The path to the logo image, as defined in theme configuration.
 * - $front_page: The URL of the front page. Use this instead of $base_path, when
 *   linking to the front page. This includes the language domain or prefix.
 * - $site_name: The name of the site, empty when display has been disabled.
 * - $site_slogan: The site slogan, empty when display has been disabled.
 * - $menu: The menu for the header (if any), as an HTML string.
 *
 * Added:
 * - $account_menu: the user account menu.
 * - $demo_menu: the demo Backdrop CMS menu.
 */
?>
<div class="branding col-xs-6 col-sm-4 col-md-3 col-lg-2">
  <?php if ($logo): ?>
    <a class="wordmark site-name" href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" rel="home">
      <span><?php print t('backdrop'); ?></span><?php print $logo; ?>
    </a>
  <?php endif; ?>
</div>

<div class="borg-navigation col-xs-6 col-sm-8 col-md-9 col-lg-10">
  <div class="borg-header-menu name-and-slogan">
    <?php if ($site_name): ?>
      <div class="site-name"><?php print $site_name; ?></div>
    <?php endif; ?>
    <?php if ($site_slogan): ?>
      <div class="site-slogan"><?php print $site_slogan; ?></div>
    <?php endif; ?>
  </div>

  <?php if ($menu): ?>
    <div class="borg-header-menu menu-account">
      <?php print render($account); ?>
    </div>
    <div class="borg-header-menu menu-demo">
      <?php print render($demo); ?>
    </div>
  <?php endif; ?>
</div>
