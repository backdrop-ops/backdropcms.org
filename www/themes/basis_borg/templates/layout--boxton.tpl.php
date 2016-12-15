<?php
/**
 * @file
 * Template for the Boxton layout.
 *
 * Variables:
 * - $title: The page title, for use in the actual HTML content.
 * - $messages: Status and error messages. Should be displayed prominently.
 * - $tabs: Tabs linking to any sub-pages beneath the current page
 *   (e.g., the view and edit tabs when displaying a node.)
 * - $action_links: Array of actions local to the page, such as 'Add menu' on
 *   the menu administration interface.
 * - $classes: Array of CSS classes to be added to the layout wrapper.
 * - $attributes: Array of additional HTML attributes to be added to the layout
 *     wrapper. Flatten using backdrop_attributes().
 * - $content: An array of content, each item in the array is keyed to one
 *   region of the layout. This layout supports the following sections:
 *   - $content['header']
 *   - $content['top']
 *   - $content['content']
 *   - $content['bottom']
 *   - $content['footer']
 */
?>
<div class="layout--boxton <?php print implode(' ', $classes); ?>"<?php print backdrop_attributes($attributes); ?>>
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

  <main class="l-wrapper">

    <div class="l-top">
      <div class="l-top-inner container container-fluid">
        <div class="l-page-title">
          <a id="main-content"></a>
          <?php print render($title_prefix); ?>
          <?php if ($title): ?>
            <h1 class="page-title"><?php print $title; ?></h1>
          <?php endif; ?>
          <?php print render($title_suffix); ?>
        </div>

        <?php if (!empty($content['top'])): ?>
          <?php print $content['top']; ?>
        <?php endif; ?>
      </div>
    </div>

    <?php if ($messages): ?>
      <div class="l-messages" role="status" aria-label="<?php print t('Status messages'); ?>">
        <div class="l-messages-inner container container-fluid">
          <?php print $messages; ?>
        </div>
      </div>
    <?php endif; ?>

    <div class="l-content" aria-label="<?php print t('Main content'); ?>">
      <div class="l-content-inner container container-fluid">
        <?php if ($tabs): ?>
          <nav class="tabs" role="tablist" aria-label="<?php print t('Admin content navigation tabs.'); ?>">
            <?php print $tabs; ?>
          </nav>
        <?php endif; ?>

        <?php print $action_links; ?>

        <?php print $content['content']; ?>
      </div>
    </div>

    <?php if (!empty($content['bottom'])): ?>
      <div class="l-bottom">
        <div class="l-bottom-inner container container-fluid">
          <?php print $content['bottom']; ?>
        </div>
      </div>
    <?php endif; ?>

  </main><!-- /.l-wrapper -->

  <?php if ($content['footer']): ?>
    <footer class="l-footer"  role="footer">
      <div class="l-footer-inner container container-fluid">
        <?php print $content['footer']; ?>
      </div><!-- /.container -->
    </footer>
  <?php endif; ?>
</div><!-- /.layout--boxton -->
