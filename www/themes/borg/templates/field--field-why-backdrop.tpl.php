<?php
/**
 * @file field.tpl.php
 * Removes the colon next to the field label and some divs.
 */
?>
<div class="<?php print implode(' ', $classes); ?>"<?php print backdrop_attributes($attributes); ?>>
  <?php if (!$label_hidden): ?>
    <div class="field-label"><?php print $label ?>&nbsp;</div>
  <?php endif; ?>
  <?php foreach ($items as $delta => $item): ?>
    <?php print render($item); ?>
  <?php endforeach; ?>
</div>
