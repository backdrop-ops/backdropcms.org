<?php
/**
 * @file
 * Theme implementation to display a node.
 * Adds a has-picture class to the footer tag.
 */
?>
<article class="node-<?php print $node->nid; ?> <?php print implode(' ', $classes); ?> container"<?php print backdrop_attributes($attributes); ?>>
  <div class="row">
    <div class="col-md-10 col-lg-9 project-content">
      <?php print render($title_prefix); ?>
      <?php if (!$page): ?>
        <h2><a href="<?php print $node_url; ?>" rel="bookmark"><?php print $title; ?></a></h2>
      <?php endif; ?>
      <?php print render($title_suffix); ?>

      <?php
        // We hide the links now so that we can render them later.
        hide($content['sidebar']);
        print render($content);
      ?>
    </div>
    <div class="col-md-2 col-lg-3 project-sidebar">
      <?php print render($content['sidebar']); ?>
    </div>
  </div><!-- /.row -->

  <div class="row">
    <div class="col-xs-12 project-foot">
      <?php print $footer; ?>

      <?php if ($display_submitted): ?>
        <?php print $user_picture; ?>
        <p class="submitted"><?php print $submitted; ?></p>
      <?php endif; ?>
    </div>
  </div><!-- /.row -->
</article>
