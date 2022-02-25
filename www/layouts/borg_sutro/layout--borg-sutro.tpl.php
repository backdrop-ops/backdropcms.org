<?php
/**
 * @file
 * Template for the Sutro layout.
 *
 * *** CHANGES: ***
 * - Moved messages just below page title.
 * - Moved top region to above title, outside .l-wrapper.
 * - Moved bottom region to above footer, outside .l-wrapper.
 * - Added a div with the "row" class to both header and footer.
 */
?>
<div class="layout--borg-sutro <?php print implode(' ', $classes); ?>"<?php print backdrop_attributes($attributes); ?>>
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
      <div class="l-top-inner <?php print $spacer_class?>">
        <?php print $content['top']; ?>
      </div>
    </div>
  <?php endif; ?>

  <?php if (!empty($content['bottom'])): ?>
    <div class="l-bottom">

      <?php if (!empty($content['top1']) || !empty($content['top2']) || !empty($content['top3'])): ?>
      <div class="l-cut-cormers container container-fluid">
        <div class="row">
          <?php if (!empty($content['top1'])): ?>
            <div class="<?php print implode(' ', $top_column_classes); ?>">
              <?php print $content['top1']; ?>
            </div>
          <?php endif; ?>
          <?php if (!empty($content['top2'])): ?>
            <div class="<?php print implode(' ', $top_column_classes); ?>">
              <?php print $content['top2']; ?>
            </div>
          <?php endif; ?>
          <?php if (!empty($content['top3'])): ?>
            <div class="<?php print implode(' ', $top_column_classes); ?>">
              <?php print $content['top3']; ?>
            </div>
          <?php endif; ?>
        </div>
      </div>
      <?php endif; ?>

      <?php print $content['bottom']; ?>
    </div>
  <?php endif; ?>

  <div class="l-wrapper">
    <div class="l-wrapper-inner container container-fluid">

      <?php if ($messages): ?>
        <div class="l-messages" role="status" aria-label="<?php print t('Status messages'); ?>">
          <?php print $messages; ?>
        </div>
      <?php endif; ?>

      <div class="l-page-title">
        <a id="main-content"></a>
        <?php print render($title_prefix); ?>
        <?php if ($title): ?>
          <h1 class="page-title"><?php print $title; ?></h1>
        <?php endif; ?>
        <?php print render($title_suffix); ?>
      </div>

      <?php if ($tabs): ?>
        <nav class="tabs" role="tablist" aria-label="<?php print t('Admin content navigation tabs.'); ?>">
          <?php print $tabs; ?>
        </nav>
      <?php endif; ?>

      <?php print $action_links; ?>

      <?php if (!empty($content['content'])): ?>
        <div class="l-content" role="main" aria-label="<?php print t('Main content'); ?>">
          <?php print $content['content']; ?>
        </div>
      <?php endif; ?>

      <?php if ($content['half1'] || $content['half2']): ?>
        <div class="l-middle l-middle-top l-halves row">
          <div class="l-halves-region col-md-8">
            <?php print $content['half1']; ?>
          </div>
          <div class="l-sidebar l-sidebar-first col-md-4">
            <?php print $content['half2']; ?>
          </div>
        </div><!-- /.l-middle -->
      <?php endif; ?>

    </div><!-- /.l-wrapper-inner -->
  </div><!-- /.l-wrapper -->

  <?php if (!empty($content['below'])): ?>
    <div class="l-below">
      <?php print $content['below']; ?>
    </div>
  <?php endif; ?>

  <?php if ($content['below1'] || $content['below2']): ?>
    <div class="l-wrapper">
      <div class="l-wrapper-inner container container-fluid">

        <div class="l-middle l-middle-bottom l-halves row">
          <div class="l-halves-region col-md-6">
            <?php print $content['below1']; ?>
          </div>
          <div class="l-halves-region col-md-6">
            <?php print $content['below2']; ?>
          </div>
        </div><!-- /.l-middle -->

      </div><!-- /.l-wrapper-inner -->
    </div><!-- /.l-wrapper -->
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
</div><!-- /.layout--sutro -->
