<?php
/**
 * @file
 * Theme implementation to display a feature.
 * - Heading does not link.
 */
?>
<article class="node-<?php print $node->nid; ?> <?php print implode(' ', $classes); ?> clearfix"<?php print backdrop_attributes($attributes); ?>>
  <?php print render($title_prefix); ?>
  <?php if (!$page): ?>
    <h3><i class="fa fa-2x <?php print $icon_class; ?>" aria-hidden="true"></i><?php print $title; ?></h3>
  <?php endif; ?>
  <?php print render($title_suffix); ?>

  <div class="content"<?php print backdrop_attributes($content_attributes); ?>>
    <?php
      // We hide the links now so that we can render them later.
      hide($content['links']);
      print render($content);
    ?>
  </div>

  <?php print render($content['links']); ?>
</article>
