<?php

/**
 * Interface for services supporting the "search_api_multi" feature.
 *
 * The interface shouldn't be implemented directly (i.e., with a proper
 * "implements" statement) since this would introduce a needless dependency.
 */
interface SearchApiMultiServiceInterface extends SearchApiServiceInterface {

  /**
   * Executes a search on the server represented by this object.
   *
   * @param SearchApiMultiQueryInterface $query
   *   The search query to execute.
   *
   * @throws SearchApiException
   *   If an error prevented the search from completing.
   *
   * @return array
   *   An associative array containing the search results, as required by
   *   SearchApiMultiQueryInterface::execute().
   */
  public function searchMultiple(SearchApiMultiQueryInterface $query);

}
