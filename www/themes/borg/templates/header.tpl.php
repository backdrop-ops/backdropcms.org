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
<div class="branding <?php print implode(' ', $branding_classes); ?>">
    <a class="site-name" href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" rel="home">
      <span><?php print t('backdrop'); ?></span>
      <?php if ($logo): print $logo; endif; ?>
      <?php if ($site_name): print $site_name; endif; ?>
    </a>
    <?php if ($site_slogan): ?>
      <div class="site-slogan"><?php print $site_slogan; ?></div>
    <?php endif; ?>
</div>
<?php if ($menu): ?>
<div class="borg-navigation <?php print implode(' ', $navigation_classes); ?>">
  <div class="borg-header-menu menu-main">
    <?php print render($menu); ?>
  </div>
  <?php if ($account): ?>
    <div class="borg-header-menu menu-account">
      <?php print render($account); ?>
    </div>
  <?php endif; ?>
  <?php if ($demo): ?>
    <div class="borg-header-menu menu-demo">
      <?php print render($demo); ?>
    </div>
  <?php endif; ?>
</div>
<?php endif; ?>
