<?php

/**
 * @file
 * Default theme implementation for displaying search results.
 *
 * This template collects each invocation of theme_search_result(). This and
 * the child template are dependent to one another sharing the markup for
 * definition lists.
 *
 * Note that modules may implement their own search type and theme function
 * completely bypassing this template.
 *
 * Available variables:
 * - $search_results: Array of data for each result
 * - $module: The machine-readable name of the module (tab) being searched, such
 *   as "node" or "user".
 *
 *
 * @see template_preprocess_search_results()
 *
 * @ingroup themeable
 */
?>
<?php if (!empty($search_results)): ?>
  <h2><?php print t('Search results');?></h2>
  <ol class="search-results <?php print $module; ?>-results">
    <?php foreach ($search_results as $result): ?>
      <li <?php print $result['attributes']; ?>>
        <?php print render($result['title_prefix']); ?>
        <h3 class="title">
          <a href="<?php print $result['url']; ?>"><?php print $result['title']; ?></a>
        </h3>
        <?php print render($result['title_suffix']); ?>
        <?php if ($result['snippet']): ?>
          <p class="search-snippet"><?php print $result['snippet']; ?></p>
        <?php endif; ?>
      </li>
    <?php endforeach; ?>
  </ol>
  <?php print $pager; ?>
<?php else : ?>
  <h2><?php print t('Your search yielded no results');?></h2>
  <ul>
    <li><?php print t('Check if your spelling is correct.'); ?></li>
    <li><?php print t('Remove quotes around phrases to search for each word individually. <em>bike shed</em> will often show more results than <em>&quot;bike shed&quot;</em>.'); ?></li>
    <li><?php print t('Consider loosening your query with <em>OR</em>. <em>bike OR shed</em> will often show more results than <em>bike shed</em>.'); ?></li>
  </ul>
<?php endif; ?>
