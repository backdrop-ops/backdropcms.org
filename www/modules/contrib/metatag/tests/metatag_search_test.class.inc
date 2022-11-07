<?php

/**
 * Dummy Search API service class.
 */
class MetatagSearchTestSearchApiService extends SearchApiAbstractService {

  /**
   * @inheritdoc
   */
  public function indexItems(SearchApiIndex $index, array $items) {
    state_set('metatag_search_test_keywords', FALSE);
    foreach (array_values($items) as $item) {
      if (isset($item['metatag_keywords']['value'])) {
        state_set('metatag_search_test_keywords', $item['metatag_keywords']['value']);
      }
    }
    return array_keys($items);
  }

  /**
   * @inheritdoc
   */
  public function deleteItems($ids = 'all', SearchApiIndex $index = NULL) {}

  /**
   * @inheritdoc
   */
  public function search(SearchApiQueryInterface $query) {}

}
