<?php
/**
 * Remove the title from all system (menu) blocks.
 */
?>
<div class="<?php print implode(' ', $classes); ?>">
  <div class="borg-header-menu menu-main">
    <?php print render($content); ?>
  </div>
  <div class="borg-header-menu menu-account">
    <?php print render($account); ?>
  </div>
  <div class="borg-header-menu menu-demo">
    <?php print render($demo); ?>
  </div>
</div>
