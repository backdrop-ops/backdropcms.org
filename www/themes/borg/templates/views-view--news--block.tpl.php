<?php
/**
 * @file
 * Main view template.
 *
 * Variables available:
 * - $classes: An array of classes determined in
 *   template_preprocess_views_view(). Default classes are:
 *     .view
 *     .view-[css_name]
 *     .view-id-[view_name]
 *     .view-display-id-[display_name]
 *     .view-dom-id-[dom_id]
 * - $css_name: A css-safe version of the view name.
 * - $css_class: The user-specified classes names, if any
 * - $header: The view header
 * - $footer: The view footer
 * - $rows: The results of the view query, if any
 * - $empty: The empty text to display if the view is empty
 * - $pager: The pager next/prev links to display, if any
 * - $exposed: Exposed widget form/info to display
 * - $feed_icon: Feed icon to display, if any
 * - $more: A link to view more, if any
 *
 * @ingroup views_templates
 */
?>
<div class="<?php print implode(' ', $classes); ?> container">
  <div class="row">
    <div class="view-content-wrapper col-md-8">
      <h2>Latest News</h2>
      <?php if ($rows): ?>
        <div class="view-content">
          <?php print (is_array($rows)) ? backdrop_render($rows) : $rows; ?>
          <?php if ($feed_icon): ?>
            <div class="feed-icon">
              <?php print $feed_icon; ?>
            </div>
          <?php endif; ?>
        </div>
      <?php elseif ($empty): ?>
        <div class="view-empty">
          <?php print $empty; ?>
        </div>
      <?php endif; ?>
    </div>

    <div class="view-footer-wrapper col-md-4">
      <?php if ($footer): ?>
        <div class="view-footer">
          <?php print $footer; ?>
        </div>
      <?php endif; ?>
    </div>
  </div>
</div><?php /* class view */ ?>
