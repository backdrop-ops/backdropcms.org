<div class="<?php print implode(' ', $classes); ?>"<?php print backdrop_attributes($attributes); ?>>
  <?php if (!$label_hidden): ?>
    <div class="field-label"><?php print $label ?>:&nbsp;</div>
  <?php endif; ?>
  <ul class="field-items"<?php print backdrop_attributes($content_attributes); ?>>
    <?php foreach ($items as $delta => $item): ?>
      <li class="field-item <?php print $delta % 2 ? 'odd' : 'even'; ?>"<?php print backdrop_attributes($item_attributes[$delta]); ?>><?php print render($item); ?></li>
    <?php endforeach; ?>
  </ul>
</div>
