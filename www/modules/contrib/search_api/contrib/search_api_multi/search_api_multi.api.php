<?php
/**
 * @file
 * Hooks provided by the Search API multi-index searches module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Alters a search query before executing it.
 *
 * @param SearchApiMultiQueryInterface $query
 *   The executed search query.
 */
function hook_search_api_multi_query_alter(SearchApiMultiQueryInterface $query) {
  $indexes = $query->getIndexes();
  if (isset($indexes['default_node_index'])) {
    $query->condition('default_node_index:author', 0, '!=');
  }
}

/**
 * @} End of "addtogroup hooks".
 */
