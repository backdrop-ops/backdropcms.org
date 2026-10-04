<?php

/**
 * @file
 * Theme the button for the date component date popup.
 */
?>
<input type="image" aria-hidden="true" role="presentation" tabindex="-1" src="<?php print base_path() . backdrop_get_path('module', 'webform') . '/images/calendar.png'; ?>" class="<?php print implode(' ', $calendar_classes); ?>" alt="<?php print t('Open popup calendar'); ?>" title="<?php print t('Open popup calendar'); ?>" />
