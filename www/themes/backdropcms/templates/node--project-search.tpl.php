<?php
/**
 * @file
 * Theme implementation to display a node.
 * Adds a has-picture class to the footer tag.
 */
?>
<article class="node-<?php print $node->nid; ?> <?php print implode(' ', $classes); ?> container"<?php print backdrop_attributes($attributes); ?>>

  <div class="project-main">
    <div class="row">
      <div class="col-md-8">
        <?php print render($title_prefix); ?>
        <?php if (!$page): ?>
          <h3><a href="<?php print $node_url; ?>" rel="bookmark"><?php print $title; ?></a></h3>
        <?php endif; ?>
        <?php print render($title_suffix); ?>
      </div>
      <div class="col-md-4 column-download">
        <?php print render($download); ?>
      </div>
    </div><!-- /.row -->

    <div class="row">
      <div class="<?php print implode(' ', $classes_col1); ?>">
        <?php print $image; ?>
      </div>
      <div class="<?php print implode(' ', $classes_col2); ?>">
        <?php print render($content); ?>
      </div>
    </div><!-- /.row -->
  </div><!-- /.project-main -->

  <div class="project-foot">
    <?php print render($footer_info); ?>
  </div><!-- /.project-foot -->

</article>

