<?php
/**
 * @file
 * Template for outputting the default block styling within a Layout.
 *
 * Variables available:
 * - $classes: Array of classes that should be displayed on the block's wrapper.
 * - $title: The title of the block.
 * - $title_prefix/$title_suffix: A prefix and suffix for the title tag. This
 *   is important to print out as administrative links to edit this block are
 *   printed in these variables.
 * - $content: The actual content of the block.
 */
?>
<div class="b-resources__outer-wrapper <?php print implode(' ', $classes); ?>"<?php print backdrop_attributes($attributes); ?>>
  <div class="section-wrapper b-resources">
    <div class="b-resources__body">
      <?php print render($title_prefix); ?>
      <?php if ($title): ?>
        <h2><?php print $title; ?></h2>
      <?php endif; ?>
      <?php print render($title_suffix); ?>
      <div><?php print $copy; ?></div>

      <ul>
        <li><?php print $projects; ?></li>
        <li><?php print $members; ?> Community Members</li>
        <li><?php print $commits; ?> Commits</li>
      </ul>
    </div><!-- /.b-resources__body -->

    <?php print $resources; ?>
  </div><!-- /.section-wrapper  -->
</div>