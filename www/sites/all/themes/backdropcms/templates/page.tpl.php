  <div id="page">

    <div id="main-wrapper" class="column <?php print $wrapper_classes; ?>"><div id="main" class="clearfix">

      <div id="content"><div class="section">
        <?php print $messages; ?>
        <?php if ($tabs): ?><div class="tabs"><?php print render($tabs); ?></div><?php endif; ?>
        <?php if ($action_links): ?><ul class="action-links"><?php print render($action_links); ?></ul><?php endif; ?>

        <a id="main-content"></a>
        <?php print render($title_prefix); ?>
        <?php if ($title): ?><h1 class="title" id="page-title"><?php print $title; ?></h1><?php endif; ?>
        <?php print render($title_suffix); ?>

        <?php print render($page['content']); ?>
        <?php print $feed_icons; ?>
      </div></div> <!-- /.section, /#content -->

      <div id="footer"><div class="section">
        <?php print render($page['footer']); ?>
        <a href="/thank-you">thank you</a>
      </div></div> <!-- /.section, /#footer -->

    </div></div> <!-- /#main, /#main-wrapper -->

    <div id="sidebar" class="column"><div class="section">
      <div id="header">
        <?php if ($logo): ?>
          <a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" rel="home" id="logo">
            <img src="<?php print $logo; ?>" alt="<?php print t('Home'); ?>" />
          </a>
        <?php endif; ?>
        <?php if ($site_name): ?>
          <div id="site-name">
            <strong><a href="<?php print $front_page; ?>" title="<?php print t('Home'); ?>" rel="home"><?php print $site_name; ?></a></strong>
          </div>
        <?php endif; ?>
      </div> <!-- /#header -->

      <nav id="main-menu" class="navigation">
        <?php print $main_menu; ?>
      </nav> <!-- /#main-menu -->

      <?php print render($page['sidebar_first']); ?>
    </div></div> <!-- /.section, /#sidebar -->

    <div id="drawer" class="<?php print $drawer_classes; ?>"><div class="section">
      <nav id="handbook-menu" class="navigation">
        <?php print $handbook_menu; ?>
      </nav>
      <?php print render($page['sidebar_second']); ?>
    </div></div> <!-- /.section, /#drawer -->

  </div> <!-- /#page, -->
