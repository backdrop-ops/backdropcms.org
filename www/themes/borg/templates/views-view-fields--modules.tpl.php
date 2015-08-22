<?php
/**
 * @file
 * Custom template for Backdrop Extensions Search Results
 */
?>

<?php print $fields['title']->content; ?>
<?php if(!empty($fields['body']->content)): ?>
<div class="result__description">
  <?php print $fields['body']->content; ?>
</div>
<?php endif; ?>
<ul class="result__info">
  <li class="result__download">
    <?php print $fields['version']->content; ?>
    <?php if(!empty($fields['download_size']->content)): ?>
      <span class="result__version"><?php print $fields['download_size']->content; ?></span>
    <?php endif; ?>
  </li>
  <li class="result__more">
    <?php print $fields['view_node']->content; ?>
  </li>
</ul>
