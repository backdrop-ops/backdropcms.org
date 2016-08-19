<div class="<?php print implode(' ', $classes); ?>"<?php print backdrop_attributes($attributes); ?>>
  <?php foreach ($items as $delta => $item): ?>
    <div class="field-item <?php print $delta % 2 ? 'odd' : 'even'; ?>"<?php print backdrop_attributes($item_attributes[$delta]); ?>><?php print render($item); ?></div>
  <?php endforeach; ?>
  <?php if (!$label_hidden): ?>
    <div class="field-label"><?php print $label ?></div>
  <?php endif; ?>
</div>
