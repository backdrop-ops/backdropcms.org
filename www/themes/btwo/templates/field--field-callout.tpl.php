<?php foreach ($items as $delta => $item): ?>
  <h2 class="<?php print implode(' ', $classes); ?>"<?php print backdrop_attributes($attributes); ?>><?php print render($item); ?></h2>
<?php endforeach; ?>
