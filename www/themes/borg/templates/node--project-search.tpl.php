<?php
/**
 * @file
 * Theme implementation to display a node.
 * Adds a has-picture class to the footer tag.
 */
?>
<article class="node-<?php print $node->nid; ?> <?php print implode(' ', $classes); ?> container"<?php print backdrop_attributes($attributes); ?>>
  <div class="project-main">
    <?php print render($title_prefix); ?>
    <?php if (!$page): ?>
      <h3><a href="<?php print $node_url; ?>" rel="bookmark"><?php print $title; ?></a></h3>
    <?php endif; ?>
    <?php print render($title_suffix); ?>

    <?php print $image; ?>

    <?php print render($content); ?>

    <?php print $more; ?>
  </div><!-- /.project-main -->

  <div class="project-foot">
    <div class="row">
      <div class="col-sm-6">
        <?php print $stats; ?>
      </div>
      <div class="col-sm-6">
        <?php print $release_info; ?>
      </div>
    </div>
  </div>
</article>
