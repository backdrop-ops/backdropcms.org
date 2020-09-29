<?php
/**
 * @file
 * Theme implementation to display a node.
 * Adds a has-picture class to the footer tag.
 */
?>
<article class="node-<?php print $node->nid; ?> <?php print implode(' ', $classes); ?> clearfix"<?php print backdrop_attributes($attributes); ?>>

  <?php print render($title_prefix); ?>
  <?php if (!$page): ?>
    <h2><a href="<?php print $node_url; ?>" rel="bookmark"><?php print $title; ?></a></h2>
  <?php endif; ?>
  <?php print render($title_suffix); ?>

  <?php if ($display_submitted): ?>
    <footer class="<?php if ($user_picture) { print 'has-picture'; } ?>">
      <?php print $user_picture; ?>
      <p class="submitted"><?php print $submitted; ?></p>
      <div class="arrow-down"></div>
    </footer>
  <?php endif; ?>

  <div class="content"<?php print backdrop_attributes($content_attributes); ?>>
    <?php
      // We hide the links now so that we can render them later.
      hide($content['links']);
      print render($content);
    ?>
  </div>

  <?php print render($content['links']); ?>

  <?php if ($page && isset($comments['comments'])): ?>
    <section class="comments">
      <?php if ($comments['comments']): ?>
        <h2 class="title"><?php print t('Comments'); ?></h2>
        <?php print render($comments['comments']); ?>
      <?php endif; ?>

      <?php if ($comments['comment_form']): ?>
        <h2 class="title comment-form"><?php print t('Add new comment'); ?></h2>
        <?php print render($comments['comment_form']); ?>
      <?php endif; ?>
    </section>
  <?php endif; ?>

</article>
