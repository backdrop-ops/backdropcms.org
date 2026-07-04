<?php
/**
 * Remove the wrappers and title.
 *
 * Added variables:
 * - $account_menu: the user account menu.
 * - $demo_menu: the demo Backdrop CMS menu.
 * - $add_wrapper: whether a wrapper should be added around the menu.
 */
?>
<div class="<?php print implode(' ', $classes); ?>">
  <?php if ($add_wrapper): ?>
  <div class="borg-header-menu menu-main">
  <?php endif; ?>
    <?php print render($content); ?>
  <?php if ($add_wrapper): ?>
  </div>
  <?php endif; ?>

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
