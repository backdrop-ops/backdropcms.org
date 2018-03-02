<?php
/**
 * @file
 * Template for the Harris layout.
 *
 * *** CHANGES: ***
 * - Moved messages just below page title.
 * - Moved top region to above title, outside .l-wrapper.
 * - Moved bottom region to above footer, outside .l-wrapper.
 * - Added a div with the "row" class to both header and footer.
 */
?>
<div class="layout--harris <?php print implode(' ', $classes); ?>"<?php print backdrop_attributes($attributes); ?>>
  <div id="skip-link">
    <a href="#main-content" class="element-invisible element-focusable"><?php print t('Skip to main content'); ?></a>
  </div>

  <?php if ($content['header']): ?>
    <header class="l-header" role="banner" aria-label="<?php print t('Site header'); ?>">
      <div class="l-header-inner container container-fluid">
        <div class="row">
          <?php print $content['header']; ?>
        </div>
      </div>
    </header>
  <?php endif; ?>

  <?php if (!empty($content['top'])): ?>
    <div class="l-top">
      <div class="l-top-inner container container-fluid">
        <?php print $content['top']; ?>
      </div>
    </div>
  <?php endif; ?>

  <div class="l-wrapper">
    <div class="l-wrapper-inner container container-fluid">

      <div class="l-page-title">
        <a id="main-content"></a>
        <?php print render($title_prefix); ?>
        <?php if ($title): ?>
          <h1 class="page-title"><?php print $title; ?></h1>
        <?php endif; ?>
        <?php print render($title_suffix); ?>
      </div>

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

      <div class="l-middle row">
        <main class="l-content col-md-6 col-md-push-3" role="main" aria-label="<?php print t('Main content'); ?>">
          <?php print $content['content']; ?>
        </main>
        <div class="l-sidebar l-sidebar-first col-md-3 col-md-pull-6">
          <?php print $content['sidebar']; ?>
        </div>
        <div class="l-sidebar l-sidebar-second col-md-3">
          <?php print $content['sidebar2']; ?>
        </div>
      </div><!-- /.l-middle -->

    </div><!-- /.l-wrapper-inner -->
  </div><!-- /.l-wrapper -->

  <?php if (!empty($content['bottom'])): ?>
    <div class="l-bottom">
      <div class="l-bottom-inner container container-fluid">
        <div class="row">
          <?php print $content['bottom']; ?>
        </div>
      </div>
    </div>
  <?php endif; ?>

  <?php if ($content['footer']): ?>
    <footer class="l-footer"  role="footer">
      <div class="l-footer-inner container container-fluid">
        <div class="row">
          <?php print $content['footer']; ?>
        </div>
      </div>
    </footer>
  <?php endif; ?>
</div><!-- /.layout--harris -->
