<?php
/**
 * @file
 * Theme implementation for comments.
 * Adds a has-picture class to the footer tag, removes the title.
 */
?>
<article class="<?php print implode(' ', $classes); ?> clearfix"<?php print backdrop_attributes($attributes); ?>>
  <?php print render($title_prefix); ?>
  <?php print render($title_suffix); ?>

  <footer class="<?php if ($user_picture) { print 'has-picture'; } ?>">
    <?php print $permalink; ?>
    <?php print $user_picture; ?>
    <p class="submitted">
      <?php print $submitted; ?>
      <?php if ($new): ?>
        <mark class="new"><?php print $new; ?>!</mark>
      <?php endif; ?>
    </p>
    <div class="arrow-down"></div>
  </footer>

  <div class="content"<?php print backdrop_attributes($content_attributes); ?>>
    <?php
      // We hide the links now so that we can render them later.
      hide($content['links']);
      print render($content);
    ?>
    <?php if ($signature): ?>
    <div class="user-signature">
      <?php print $signature; ?>
    </div>
    <?php endif; ?>
  </div>

  <?php print render($content['links']) ?>
</article>
