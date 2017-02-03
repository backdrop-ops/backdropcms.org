<?php if ($forhire = drupal_render($item)) : ?>
  <div class="<?php print implode(' ', $classes); ?>"<?php print backdrop_attributes($attributes); ?>>
    <?php foreach ($items as $delta => $item): ?>
      <?php print $forhire; ?>
    <?php endforeach; ?>
  </div>
<?php endif; ?>
