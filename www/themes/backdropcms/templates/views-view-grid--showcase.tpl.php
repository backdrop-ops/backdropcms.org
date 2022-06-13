<?php
/**
 * @file
 * Default view template to display a rows in a grid.
 *
 * - $title: The title of this group of rows.  May be empty.
 * - $classes: An array of classes to apply to the grid, based on settings.
 * - $attributes: An array of additional HTML attributes for the grid.
 * - $caption: The caption for this grid. May be empty.
 * - $rows: A nested array of rows. Each row contains an array of columns.
 * - $row_classes: An array of classes to apply to each row, indexed by row
 *   number. This matches the index in $rows.
 * - $column_classes: An array of classes to apply to each column, indexed by
 *   row number, then column number. This matches the index in $rows.
 *
 * @ingroup views_templates
 */
?>
<?php if (!empty($title)) : ?>
  <h3><?php print $title; ?></h3>
<?php endif; ?>
<div class="<?php print implode(' ', $classes); ?>"<?php print backdrop_attributes($attributes); ?>>
  <?php foreach ($rows as $row_number => $columns): ?>
    <div <?php if (!empty($row_classes[$row_number])) { print 'class="' . implode(' ', $row_classes[$row_number]) .'"';  } ?>>
      <?php foreach ($columns as $column_number => $item): ?>
        <?php if (!empty($item)) { ?>
          <div <?php if ($column_classes[$row_number][$column_number]) { print 'class="col ' . implode(' ', $column_classes[$row_number][$column_number]) .'"';  } ?>>
            <div class="col-inner">
              <?php print $item; ?>
            </div>
          </div>
        <?php } ?>
      <?php endforeach; ?>
    </div>
  <?php endforeach; ?>
</div>
