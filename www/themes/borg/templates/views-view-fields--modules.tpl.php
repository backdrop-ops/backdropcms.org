<?php
/**
 * @file
 * Custom template for Backdrop Extensions Search Results
 */
?>
<?php if(!empty($fields['title']->content)): ?>
<h3>
  <?php print $fields['title']->content; ?>
  <?php unset($fields['title']); ?>
</h3>
<?php endif; ?>

<?php if (!empty($fields['created_1']->content)): ?>
<div class="since">
  <?php print $fields['created_1']->label_html; ?><?php print $fields['created_1']->content; ?>
  <?php unset($fields['created_1']); ?>
</div>
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
    <li class="<?php print backdrop_clean_css_identifier($id); ?>"><?php print $field->label_html; ?><?php print $field->content; ?></li>
  <?php endif; ?>
<?php endforeach; ?>
</ul>
