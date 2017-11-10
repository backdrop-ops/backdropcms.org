<?php
/**
 * @file
 * Template for a 2 column layout.
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
 *   - $content['content_sidebar']
 *   - $content['content']
 *   - $content['sidebar']
 *   - $content['drawer']
 *   - $content['footer']
 */
?>
<div class="layout--double-fixed-case <?php print implode(' ', $classes); ?>"<?php print backdrop_attributes($attributes); ?>>
  <div id="skip-link">
    <a href="#main-content" class="element-invisible element-focusable"><?php print t('Skip to main content'); ?></a>
  </div>

  <?php if ($content['header']): ?>
    <header class="l-header" role="banner" aria-label="<?php print t('Site header'); ?>">
      <div class="l-header-inner container container-fluid">
        <?php print $content['header']; ?>
      </div>
    </header>
  <?php endif; ?>

  <?php if (!empty($content['content-top'])): ?>
    <div<?php print backdrop_attributes($top_attributes); ?>>
      <div class="l-top-inner container container-fluid">
        <div class="l-page-title">
          <a id="main-content"></a>
          <?php print render($title_prefix); ?>
          <?php if ($title): ?>
            <h1 class="page-title"><?php print $title; ?></h1>
          <?php endif; ?>
          <?php print render($title_suffix); ?>
        </div>

        <?php print $content['content-top']; ?>
      </div>
    </div>
  <?php endif; ?>

  <div class="l-wrapper">
    <div class="l-wrapper-inner container clearfix">

      <?php if ($messages): ?>
        <div class="l-messages" role="status" aria-label="<?php print t('Status messages'); ?>">
          <?php print $messages; ?>
        </div>
      <?php endif; ?>

      <?php if ($tabs): ?>
        <nav class="tabs" role="tablist" aria-label="<?php print t('Admin content navigation tabs.'); ?>">
          <?php print $tabs; ?>
        </nav>
      <?php endif; ?>

      <?php print $action_links; ?>

      <?php if (!empty($content['top'])): ?>
        <div class="l-top">
          <?php print $content['top']; ?>
        </div>
      <?php endif; ?>

      <div class="row">
        <main class="l-content col-md-8" role="main" aria-label="<?php print t('Main content'); ?>">
          <?php print $content['content']; ?>
        </main>
        <div class="l-sidebar l-sidebar-first col-md-4">
          <?php print $content['content_sidebar']; ?>
        </div>
      </div>

      <?php if ($row_first): ?>
        <div class="row">
          <?php print $row_first; ?>
        </div>
      <?php endif; ?>
      <?php if ($row_second): ?>
        <div class="row">
          <?php print $row_second; ?>
        </div>
      <?php endif; ?>
      <?php if ($row_third): ?>
        <div class="row">
          <?php print $row_third; ?>
        </div>
      <?php endif; ?>
      <?php if ($row_fourth): ?>
        <div class="row">
          <?php print $row_fourth; ?>
        </div>
      <?php endif; ?>
      <?php if ($row_fifth): ?>
        <div class="row">
          <?php print $row_fifth; ?>
        </div>
      <?php endif; ?>
      <?php if ($row_sixth): ?>
        <div class="row">
          <?php print $row_sixth; ?>
        </div>
      <?php endif; ?>
      <?php if ($row_seventh): ?>
        <div class="row">
          <?php print $row_seventh; ?>
        </div>
      <?php endif; ?>
      <?php if ($row_eighth): ?>
        <div class="row">
          <?php print $row_eighth; ?>
        </div>
      <?php endif; ?>
      <?php if ($row_ninth): ?>
        <div class="row">
          <?php print $row_ninth; ?>
        </div>
      <?php endif; ?>
      <?php if ($row_tenth): ?>
        <div class="row">
          <?php print $row_tenth; ?>
        </div>
      <?php endif; ?>

    </div><!-- /.l-wrapper-inner -->
  </div><!-- /.l-wrapper -->

  <?php if ($content['footer']): ?>
    <footer class="l-footer"  role="footer">
      <div class="l-footer-inner container container-fluid">
        <?php print $content['footer']; ?>
      </div>
    </footer>
  <?php endif; ?>
</div>
