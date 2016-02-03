<?php
/**
 * @file
 * Custom template for Backdrop Extensions Search Results
 */
?>
<?php if(!empty($fields['body']->content)): ?>
  <?php print $fields['title']->content; ?>
  <?php unset($fields['title']); ?>
<?php endif; ?>
<?php if(!empty($fields['body']->content)): ?>
<div class="result__description">
  <?php print $fields['body']->content; ?>
  <?php unset($fields['body']); ?>
</div>
<?php endif; ?>

<ul class="result__info">
<?php foreach ($fields as $id => $field): ?>
  <?php if(!empty($field->content)): ?>
    <li class="<?php print backdrop_clean_css_identifier($id); ?>"><?php print $field->content; ?></li>
  <?php endif; ?>
<?php endforeach; ?>
</ul>
