<?php
/**
 * @file
 * Template for a 2 column layout.
 *
 * This template provides a two column layout with the sidebar on the right and
 * a roughly 60/40 split.
 *
 * Variables:
 * - $title: The page title, for use in the actual HTML content.
 * - $messages: Status and error messages. Should be displayed prominently.
 * - $tabs: Tabs linking to any sub-pages beneath the current page
 *   (e.g., the view and edit tabs when displaying a node).
 * - $action_links: Array of actions local to the page, such as 'Add menu' on
 *   the menu administration interface.
 * - $classes: Array of CSS classes to be added to the layout wrapper.
 * - $attributes: Array of additional HTML attributes to be added to the layout
 *     wrapper. Flatten using backdrop_attributes().
 * - $content: An array of content, each item in the array is keyed to one
 *   region of the layout. This layout supports the following sections:
 *   - $content['content']
 *   - $content['sidebar']
 *   - $content['drawer']
 *   - $content['footer']
 */
?>
<div class="layout--drawer <?php print implode(' ', $classes); ?>"<?php print backdrop_attributes($attributes); ?>>
  <div id="skip-link">
    <a href="#main-content" class="element-invisible element-focusable"><?php print t('Skip to main content'); ?></a>
  </div>

  <div class="l-container">
    <main class="l-content" role="main">
      <div class="l-content-inner">
        <a id="main-content"></a>
        <?php print render($title_prefix); ?>
        <?php if ($title): ?>
          <h1 class="title" id="page-title">
            <?php print $title; ?>
          </h1>
        <?php endif; ?>
        <?php print render($title_suffix); ?>

        <?php if ($tabs): ?>
          <div class="tabs">
            <?php print $tabs; ?>
          </div>
        <?php endif; ?>

        <?php print $action_links; ?>

        <?php if ($messages): ?>
          <section class="l-messages">
            <?php print $messages; ?>
          </section>
        <?php endif; ?>

        <?php print $content['content']; ?>
      </div>
      <?php if ($content['footer']): ?>
        <div class="l-footer">
            <?php print $content['footer']; ?>
        </div>
      <?php endif; ?>
    </main>

    <?php if ($content['sidebar']): ?>
      <div class="l-sidebar">
        <?php print $content['sidebar']; ?>
      </div>
    <?php endif; ?>

    <?php if ($content['drawer']): ?>
      <div class="l-drawer">
        <?php print $content['drawer']; ?>
      </div>
    <?php endif; ?>
  </div>
</div>
