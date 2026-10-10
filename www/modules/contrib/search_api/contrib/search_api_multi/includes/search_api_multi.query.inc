<?php
/**
 * @file
 * Contains SearchApiMultiQueryInterface and SearchApiMultiQuery.
 */

/**
 * Interface representing a search query on multiple Search API indexes.
 *
 * For discerning from which index a certain field should be used (for filtering
 * or specifying the fulltext fields, for instance), all field identifiers have
 * to be prefixed with their index' machine name, seperated by a colon.
 * For example, to filter on the "author:name" field from the index with the
 * machine name "default_node_index", use "default_node_index:author:name" as
 * the identifier.
 *
 * Methods not returning something else will return the object itself, so calls
 * can be chained.
 */
interface SearchApiMultiQueryInterface {

  /**
   * Constructs a new query object.
   *
   * @param array $options
   *   Associative array of options configuring this query. Recognized options
   *   are:
   *   - conjunction: The type of conjunction to use for this query - either
   *     'AND' or 'OR'. 'AND' by default. This only influences the search keys,
   *     filters will always use AND by default.
   *   - 'parse mode': The mode with which to parse the $keys variable, if it
   *     is set and not already an array. See SearchApiMultiQuery::parseModes()
   *     for recognized parse modes.
   *   - languages: The languages to search for, as an array of language IDs.
   *     If not specified, all languages will be searched. Language-neutral
   *     content (LANGUAGE_NONE) is always searched.
   *   - offset: The position of the first returned search results relative to
   *     the whole result on the server.
   *   - limit: The maximum number of search results to return. -1 means no
   *     limit.
   *   - 'filter class': Can be used to change the SearchApiQueryFilterInterface
   *     implementation to use.
   *   - 'search id': A string that will be used as the identifier when storing
   *     this search in the static cache.
   *   All options are optional.
   *
   * @throws SearchApiException
   *   If a search with these options won't be possible.
   */
  public function __construct(array $options = array());

  /**
   * Retrieve a list of all parse modes supported by this query class.
   *
   * @return array
   *   An associative array of parse modes recognized by objects of this class.
   *   The keys are the parse modes' ids, values are associative arrays
   *   containing the following entries:
   *   - name: The translated name of the parse mode.
   *   - description: (optional) A translated text describing the parse mode.
   */
  public function parseModes();

  /**
   * Method for creating a filter to use with this query object.
   *
   * @param string $conjunction
   *   The conjunction to use for the filter - either 'AND' or 'OR'.
   * @param string[] $tags
   *   (Optional) An arbitrary set of tags. Can be used to identify this filter
   *   down the line if necessary. This is primarily used by the facet system
   *   to support OR facet queries.
   *
   * @return SearchApiQueryFilterInterface
   *   A filter object that is set to use the specified conjunction.
   */
  public function createFilter($conjunction = 'AND', array $tags = array());

  /**
   * Sets the keys to search for.
   *
   * If this method is not called on the query before execution, or it is called
   * with NULL, this will be a filter-only query.
   *
   * @param string|array|null $keys
   *   A string with the unparsed search keys, or NULL to use no search keys.
   *
   * @return SearchApiMultiQueryInterface
   *   The called object.
   */
  public function keys($keys = NULL);

  /**
   * Sets the fields that will be searched for the search keys.
   *
   * If this method is not called on the query before execution, or it is called
   * with NULL, all fulltext fields should be searched.
   *
   * @param array|null $fields
   *   An array containing fulltext fields that should be searched, or NULL if
   *   all fields should be searched.
   *
   * @throws SearchApiException
   *   If one of the fields isn't of type "text".
   *
   * @return SearchApiMultiQueryInterface
   *   The called object.
   */
  public function fields($fields = NULL);

  /**
   * Adds a subfilter to this query's filter.
   *
   * @param SearchApiQueryFilterInterface $filter
   *   A SearchApiQueryFilter object that should be added as a subfilter.
   *
   * @return SearchApiMultiQueryInterface
   *   The called object.
   */
  public function filter(SearchApiQueryFilterInterface $filter);

  /**
   * Add a new ($field $operator $value) condition filter.
   *
   * @param string $field
   *   The field to filter on. Either a field specification as detailed in the
   *   class comment, or the special field "search_api_multi_index" which means
   *   a filter on the searched indexes.
   * @param mixed $value
   *   The value the field should have (or be related to by the operator).
   * @param string $operator
   *   The operator to use for checking the constraint. The following operators
   *   are supported for primitive types: "=", "<>", "<", "<=", ">=", ">". They
   *   have the same semantics as the corresponding SQL operators.
   *   If $field is a fulltext field, $operator can only be "=" or "<>", which
   *   are in this case interpreted as "contains" or "doesn't contain",
   *   respectively.
   *   If $value is NULL, $operator also can only be "=" or "<>", meaning the
   *   field must have no or some value, respectively.
   *
   * @return SearchApiMultiQueryInterface
   *   The called object.
   */
  public function condition($field, $value, $operator = '=');

  /**
   * Add a sort directive to this search query.
   *
   * If no sort is manually set, the results will be sorted descending by
   * relevance.
   *
   * How sorts on index-specific fields are handled may differ between service
   * backends.
   *
   * @param string $field
   *   The field to sort by. The special fields 'search_api_relevance' (sort by
   *   relevance) and 'search_api_id' (sort by item id) may be used.
   * @param string $order
   *   The order to sort items in - either 'ASC' or 'DESC'.
   *
   * @throws SearchApiException
   *   If the field is multi-valued or of a fulltext type.
   *
   * @return SearchApiMultiQueryInterface
   *   The called object.
   */
  public function sort($field, $order = 'ASC');

  /**
   * Adds a range of results to return. This will be saved in the query's
   * options. If called without parameters, this will remove all range
   * restrictions previously set.
   *
   * @param int|null $offset
   *   The zero-based offset of the first result returned.
   * @param int|null $limit
   *   The number of results to return.
   *
   * @return SearchApiMultiQueryInterface
   *   The called object.
   */
  public function range($offset = NULL, $limit = NULL);

  /**
   * Executes this search query.
   *
   * @return array
   *   An associative array containing the search results. The following keys
   *   are standardized:
   *   - 'result count': The overall number of results for this query, without
   *     range restrictions. Might be approximated, for large numbers.
   *   - results: An array of results, ordered as specified. The array keys are
   *     arbitrary unique keys, values are arrays containing the following keys:
   *     - id: The item's ID.
   *     - index_id: The machine name of the index this item was found on.
   *     - score: A float measuring how well the item fits the search.
   *     - fields: (optional) If set, an array containing some field values
   *       already ready-to-use, keyed by their field identifiers (without index
   *       prefix). This allows search engines (or postprocessors) to store
   *       extracted fields so other modules don't have to extract them again.
   *       This fields should always be checked by modules that want to use
   *       field contents of the result items.
   *     - entity (optional): If set, the fully loaded result item. This field
   *       should always be used by modules using search results, to avoid
   *       duplicate item loads.
   *     - excerpt (optional): If set, an HTML text containing highlighted
   *       portions of the fulltext that match the query.
   *   - warnings: A numeric array of translated warning messages that may be
   *     displayed to the user.
   *   - ignored: A numeric array of search keys that were ignored for this
   *     search (e.g., because of being too short or stop words).
   *   - performance: An associative array with the time taken (as floats, in
   *     seconds) for specific parts of the search execution:
   *     - complete: The complete runtime of the query.
   *     - hooks: Hook invocations and other client-side preprocessing.
   *     - preprocessing: Preprocessing of the service class.
   *     - execution: The actual query to the search server, in whatever form.
   *     - postprocessing: Preparing the results for returning.
   *   Additional metadata may be returned in other keys. Only 'result count'
   *   and 'result' always have to be set, all other entries are optional.
   */
  public function execute();

  /**
   * Retrieves the searched indexes.
   *
   * @return SearchApiIndex[]
   *   An array of SearchApiIndex objects representing all indexes that will be
   *   used for this search, keyed by machine names.
   */
  public function getIndexes();

  /**
   * Retrieves the search keys.
   *
   * @return array|string|null
   *   This object's search keys - either a string or an array specifying a
   *   complex search expression.
   *   An array will contain a '#conjunction' key specifying the conjunction
   *   type, and search strings or nested expression arrays at numeric keys.
   *   Additionally, a '#negation' key might be present, which means – unless it
   *   maps to a FALSE value – that the search keys contained in that array
   *   should be negated, i.e. not be present in returned results.
   */
  public function &getKeys();

  /**
   * Retrieves the original, unprocessed search keys.
   *
   * @return array|string|null
   *   The unprocessed search keys, exactly as passed to this object. Has the
   *   same format as getKeys().
   */
  public function getOriginalKeys();

  /**
   * Retrieves the searched fulltext fields.
   *
   * @return array|null
   *   An array containing the fields that should be searched for the search
   *   keys.
   */
  public function &getFields();

  /**
   * Retrieves the query's filter object.
   *
   * @return SearchApiQueryFilterInterface
   *   This object's associated filter object.
   */
  public function getFilter();

  /**
   * Retrieves the set sorts.
   *
   * @return array
   *   An array specifying the sort order for this query. Array keys are the
   *   field names in order of importance, the values are the respective order
   *   in which to sort the results according to the field.
   */
  public function &getSort();

  /**
   * Retrieves a single option.
   *
   * @param string $name
   *   The name of an option.
   * @param mixed $default
   *   The default in case the option isn't set.
   *
   * @return mixed
   *   The value of the option with the specified name, if set; $default
   *   otherwise.
   */
  public function getOption($name, $default = NULL);

  /**
   * Sets an option.
   *
   * @param string $name
   *   The name of an option.
   * @param mixed $value
   *   The new value of the option.
   *
   * @return mixed
   *   The option's previous value.
   */
  public function setOption($name, $value);

  /**
   * Retrieves all options for this query.
   *
   * @return array
   *   An associative array of query options.
   */
  public function &getOptions();

}

/**
 * Standard implementation of SearchApiMultiQueryInterface.
 *
 * If the search involves only a single server which supports the
 * "search_api_multi" feature, the methods for this feature are used. Otherwise,
 * generic code allows the searching of multiple indexes.
 */
class SearchApiMultiQuery implements SearchApiMultiQueryInterface {

  /**
   * All indexes which are used in this search.
   *
   * This is first loaded with all indexes, and only restricted to the used ones
   * during preExecute().
   *
   * @var array
   */
  protected $indexes = array();

  /**
   * The indexes which are currently used in this search.
   *
   * This collects the index IDs (in the keys) of indexes as they are used in
   * the search, so the appropriate ones can be kept in $this->indexes during
   * preExecute().
   *
   * @var array
   */
  protected $used_indexes = array();

  /**
   * All indexes which are used in this search, ordered by their servers.
   *
   * The array contains server machine names mapped to an array of all their
   * searched indexes.
   *
   * @var array
   */
  protected $servers = array();

  /**
   * The search keys. If NULL, this will be a filter-only search.
   *
   * @var mixed
   */
  protected $keys;

  /**
   * The unprocessed search keys, as passed to the keys() method.
   *
   * @var mixed
   */
  protected $orig_keys;

  /**
   * The fields that will be searched for the keys.
   *
   * @var array|null
   */
  protected $fields;

  /**
   * The fields that will be searched, grouped by index.
   *
   * @var array
   */
  protected $index_fields = array();

  /**
   * The search filter associated with this query.
   *
   * @var SearchApiQueryFilterInterface
   */
  protected $filter;

  /**
   * The sort associated with this query.
   *
   * @var array
   */
  protected $sort = array();

  /**
   * Search options configuring this query.
   *
   * @var array
   */
  protected $options;

  /**
   * Flag for whether preExecute() was already called for this query.
   *
   * @var bool
   */
  protected $pre_execute = FALSE;

  /**
   * {@inheritdoc}
   */
  public function __construct(array $options = array()) {
    if (isset($options['parse mode'])) {
      $modes = $this->parseModes();
      if (!isset($modes[$options['parse mode']])) {
        throw new SearchApiException(t('Unknown parse mode: @mode.', array('@mode' => $options['parse mode'])));
      }
    }
    $this->options = $options + array(
      'conjunction' => 'AND',
      'parse mode' => 'terms',
      'filter class' => 'SearchApiQueryFilter',
      'search id' => __CLASS__,
    );
    $this->filter = $this->createFilter('AND');
    $this->indexes = search_api_index_load_multiple(FALSE, array('enabled' => TRUE));
    foreach ($this->indexes as $index_id => $index) {
      $this->servers[$index->server][$index_id] = $index;
    }
  }

  /**
   * {@inheritdoc}
   */
  public function parseModes() {
    $modes['direct'] = array(
      'name' => t('Direct query'),
      'description' => t("Don't parse the query, just hand it to the search server unaltered. Might fail if the query contains syntax errors in regard to the specific server's query syntax."),
    );
    $modes['single'] = array(
      'name' => t('Single term'),
      'description' => t('The query is interpreted as a single keyword, maybe containing spaces or special characters.'),
    );
    $modes['terms'] = array(
      'name' => t('Multiple terms'),
      'description' => t('The query is interpreted as multiple keywords seperated by spaces. Keywords containing spaces may be "quoted". Quoted keywords must still be seperated by spaces.'),
    );
    return $modes;
  }

  /**
   * Parses the keys string according to a certain parse mode.
   *
   * @param string|array|null $keys
   *   The keys as passed to keys().
   * @param string $mode
   *   The parse mode to use. Must be one of the keys from parseModes().
   *
   * @return string|array|null
   *   The parsed keys.
   */
  protected function parseKeys($keys, $mode) {
    if ($keys == NULL || is_array($keys)) {
      return $keys;
    }
    $keys = '' . $keys;
    switch ($mode) {
      case 'direct':
        return $keys;

      case 'single':
        return array('#conjunction' => $this->options['conjunction'], $keys);

      case 'terms':
        $ret = explode(' ', $keys);
        $ret['#conjunction'] = $this->options['conjunction'];
        $quoted = FALSE;
        $str = '';
        foreach ($ret as $k => $v) {
          if (!$v) {
            continue;
          }
          if ($quoted) {
            if ($v[backdrop_strlen($v) - 1] == '"') {
              $v = substr($v, 0, -1);
              $str .= ' ' . $v;
              $ret[$k] = $str;
              $quoted = FALSE;
            }
            else {
              $str .= ' ' . $v;
              unset($ret[$k]);
            }
          }
          elseif ($v[0] == '"') {
            $len = backdrop_strlen($v);
            if ($len > 1 && $v[$len - 1] == '"') {
              $ret[$k] = substr($v, 1, -1);
            }
            else {
              $str = substr($v, 1);
              $quoted = TRUE;
              unset($ret[$k]);
            }
          }
        }
        if ($quoted) {
          $ret[] = $str;
        }
        return array_filter($ret);

      default:
        throw new SearchApiException(t('Unrecognized parse mode %mode.', array('%mode' => $mode)));
    }
  }

  /**
   * {@inheritdoc}
   */
  public function createFilter($conjunction = 'AND', array $tags = array()) {
    $filter_class = $this->options['filter class'];
    return new $filter_class($conjunction, $tags);
  }

  /**
   * {@inheritdoc}
   */
  public function keys($keys = NULL) {
    $this->orig_keys = $keys;
    if (isset($keys)) {
      $this->keys = $this->parseKeys($keys, $this->options['parse mode']);
    }
    else {
      $this->keys = NULL;
    }
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function fields($fields = NULL) {
    $this->index_fields = array();
    if ($fields) {
      foreach ($fields as $spec) {
        list($index_id, $field) = explode(':', $spec, 2);
        $index = $this->indexes[$index_id];
        if (empty($index->options['fields'][$field]) || !search_api_is_text_type($index->options['fields'][$field]['type'])) {
          throw new SearchApiException(t('Trying to search on field @field which is no indexed fulltext field.', array('@field' => $field)));
        }
        $this->used_indexes[$index_id] = TRUE;
        $this->index_fields[$index_id][] = $field;
      }
    }
    $this->fields = $fields;
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function filter(SearchApiQueryFilterInterface $filter) {
    $this->filter->filter($filter);
    $indexes = $this->checkFilterIndexes($filter);
    $this->used_indexes += $indexes;
    // Since the filter is added with AND to the query, the query will be
    // restricted to the indexes encountered within it.
    $this->indexes = array_intersect_key($this->indexes, $indexes);
    return $this;
  }

  /**
   * Checks a filter object for filters on the used indexes.
   *
   * @param SearchApiQueryFilterInterface $filter
   *   The filter whose indexes should be added.
   *
   * @return array
   *   An array mapping the machine names of all indexes used in the filter to
   *   TRUE.
   */
  protected function checkFilterIndexes(SearchApiQueryFilterInterface $filter) {
    $indexes = array();
    // Remember all the indexes of fields used in any filters, so we can later
    // restrict the search to only those. Also, restrict the search correctly if
    // the "search_api_multi_index" field is used.
    foreach ($filter->getFilters() as $f) {
      if (is_array($f)) {
        if ($f[0] == 'search_api_multi_index') {
          if ($f[2] == '=') {
            $indexes[$f[1]] = TRUE;
          }
          else {
            foreach ($this->indexes as $id => $index) {
              if ($id != $f[1]) {
                $indexes[$id] = TRUE;
              }
            }
          }
        }
        elseif ($f[2] != '<>' && strpos($f[0], ':')) {
          list($index_id) = explode(':', $f[0], 2);
          $indexes[$index_id] = TRUE;
        }
      }
      else {
        $indexes += $this->checkFilterIndexes($f);
      }
    }
    return $indexes;
  }

  /**
   * {@inheritdoc}
   */
  public function condition($field, $value, $operator = '=') {
    if ($field == 'search_api_multi_index') {
      if ($operator == '=') {
        if (isset($this->indexes[$value])) {
          $this->indexes = array($value => $this->indexes[$value]);
        }
        else {
          throw new SearchApiException(t('Trying to filter multi-index query on two indexes simultaneously.'));
        }
      }
      else {
        unset($this->indexes[$value]);
      }
    }
    else {
      $this->filter->condition($field, $value, $operator);
      if ($operator != '<>' && strpos($field, ':')) {
        list($index_id) = explode(':', $field, 2);
        $this->used_indexes[$index_id] = TRUE;
      }
    }
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function sort($field, $order = 'ASC') {
    if ($field != 'search_api_relevance' && $field != 'search_api_id') {
      list($index_id, $f) = explode(':', $field, 2);
      $index = $this->indexes[$index_id];
      $fields = $index->options['fields'];
      if (empty($fields[$f])) {
        throw new SearchApiException(t('Trying to sort on unknown field @field.', array('@field' => $f)));
      }
      $type = $fields[$f]['type'];
      if (search_api_is_list_type($type) || search_api_is_text_type($type)) {
        throw new SearchApiException(t('Trying to sort on field @field of illegal type @type.', array(
          '@field' => $f,
          '@type' => $type,
        )));
      }
      $this->used_indexes[$index_id] = TRUE;
    }
    $order = strtoupper(trim($order)) == 'DESC' ? 'DESC' : 'ASC';
    $this->sort[$field] = $order;
    return $this;
  }

  /**
   * {@inheritdoc}
   */
  public function range($offset = NULL, $limit = NULL) {
    $this->options['offset'] = $offset;
    $this->options['limit'] = $limit;
    return $this;
  }

  /**
   * Implements SearchApiMultiQueryInterface::execute().
   *
   * Uses a server's searchMultiple() method, where possible.
   */
  public final function execute() {
    $start = microtime(TRUE);

    // Call pre-execute hook.
    $this->preExecute();

    // Let modules alter the query.
    backdrop_alter('search_api_multi_query', $this);

    $pre_search = microtime(TRUE);

    // Execute query.
    if (count($this->servers) == 1) {
      $server = search_api_server_load(key($this->servers));
      if ($server && $server->supportsFeature('search_api_multi')) {
        $response = $server->searchMultiple($this);
      }
    }
    if (!isset($response)) {
      $response = $this->searchMultiple();
    }

    $post_search = microtime(TRUE);

    // Call post-execute hook.
    $this->postExecute($response);

    $end = microtime(TRUE);
    $response['performance']['complete'] = $end - $start;
    $response['performance']['hooks'] = $response['performance']['complete'] - ($post_search - $pre_search);

    // Store search for later retrieval for facets, etc.
    search_api_multi_current_search(NULL, $this, $response);

    return $response;
  }

  /**
   * Helper method for adding a language filter.
   *
   * @param array $languages
   *   The languages which the query should include.
   */
  protected function addLanguages(array $languages) {
    if (array_search(LANGUAGE_NONE, $languages) === FALSE) {
      $languages[] = LANGUAGE_NONE;
    }

    $filter = $this->createFilter('OR');
    foreach ($languages as $lang) {
      foreach ($this->indexes as $index_id => $index) {
        $filter->condition("$index_id:search_api_language", $lang);
      }
    }
    $this->filter($filter);
  }

  /**
   * Searches multiple indexes with this query.
   *
   * Workaround if there is no server's searchMultiple() method available.
   *
   * @return array
   *   Search results as specified by SearchApiMultiQueryInterface::execute().
   */
  protected function searchMultiple() {
    // Prepare options/range.
    $options = $this->options;
    if (!empty($options['offset']) || isset($options['limit'])) {
      $options['limit'] = isset($options['limit']) ? $options['offset'] + $options['limit'] : NULL;
      $options['offset'] = 0;
    }
    // Prepare a normal Search API query for all contained indexes.
    /** @var SearchApiQuery[] $queries */
    $queries = array();
    foreach ($this->getIndexes() as $index_id => $index) {
      try {
        $queries[$index_id] = search_api_query($index_id, $options);
      }
      catch (SearchApiException $e) {
        watchdog_exception('search_api_multi', $e);
      }
    }

    // Set the filters appropriately.
    $this->addFilters($this->filter->getFilters(), $queries, $queries);

    // Prepare and execute the search on every index available.
    foreach ($queries as $index_id => $query) {
      if (isset($this->orig_keys)) {
        if (empty($this->index_fields[$index_id])) {
          continue;
        }
        $query->keys($this->orig_keys);
        $query->fields($this->index_fields[$index_id]);
      }
      foreach ($this->sort as $field => $order) {
        if (strpos($field, ':') !== FALSE) {
          list($field_index_id, $field) = explode(':', $field, 2);
          if ($field_index_id != $index_id) {
            continue;
          }
        }
        $query->sort($field, $order);
      }

      $response = $query->execute();

      if (!empty($response['results'])) {
        // Adapt the results array to the multi-index format.
        $results = array();
        foreach ($response['results'] as $key => $result) {
          $key = "$index_id:$key";
          $results[$key] = $result;
          $results[$key]['index_id'] = $index_id;
        }
        $response['results'] = $results;
      }

      if (!isset($return)) {
        $return = array(
          'result count' => 0,
          'results' => array(),
          'performance' => array(),
        );
      }

      // Add the new result count.
      $return['result count'] += $response['result count'];

      // Merge results.
      if (!empty($response['results'])) {
        $return['results'] = array_merge($return['results'], $response['results']);
      }

      // Merge performance.
      if (!empty($response['performance'])) {
        foreach ($response['performance'] as $measure => $time) {
          $return['performance'] += array($measure => 0);
          $return['performance'][$measure] += $time;
        }
      }

      // Merge any additional keys. We can only guess what to do here, but we
      // opt to merge array-valued keys together, and store all other kinds of
      // data in a new array keyed by index ID.
      unset($response['result count'], $response['results'], $response['performance']);
      foreach ($response as $key => $value) {
        if (is_array($value)) {
          $return[$key] = isset($return[$key]) ? array_merge($value, $return[$key]) : $value;
        }
        else {
          $return[$key][$index_id] = $value;
        }
      }
    }

    if (isset($return)) {
      if (!empty($return['results'])) {
        // Add default sorting by score, if it isn't included already.
        if ($this->keys && !isset($this->sort['search_api_relevance'])) {
          $this->sort['search_api_relevance'] = 'DESC';
        }
        // Sort the results.
        if ($this->sort) {
          $this->ensureSortFields($return['results']);
          uasort($return['results'], array($this, 'compareResults'));
        }
        // Apply range.
        $offset = $this->getOption('offset', 0);
        $limit = $this->getOption('limit', NULL);
        $return['results'] = array_slice($return['results'], $offset, $limit, TRUE);
      }
      return $return;
    }

    return array('result count' => 0);
  }

  /**
   * Helper method for adding a filter to index-specific queries.
   *
   * @param SearchApiQueryFilterInterface[]|array[] $filters
   *   An array of filters to add, as returned by
   *   SearchApiQueryFilterInterface::getFilters().
   * @param SearchApiQuery[] $parents
   *   The query or filter objects to which the filters should be applied, keyed
   *   by index ID.
   * @param SearchApiQuery[] $queries
   *   The queries used, keyed by index ID.
   */
  protected function addFilters(array $filters, array $parents, array $queries) {
    foreach ($filters as $filter) {
      if (is_array($filter)) {
        if ($filter[0] == 'search_api_multi_index') {
          continue;
        }
        list($index_id, $field) = explode(':', $filter[0], 2);
        if (!empty($parents[$index_id])) {
          $parents[$index_id]->condition($field, $filter[1], $filter[2]);
        }
      }
      else {
        /** @var SearchApiQueryFilterInterface[] $nested */
        $nested = array();
        foreach ($parents as $index_id => $query) {
          $nested[$index_id] = $queries[$index_id]->createFilter($filter->getConjunction());
        }
        $this->addFilters($filter->getFilters(), $nested, $queries);
        foreach ($nested as $index_id => $nested_filter) {
          if ($nested_filter->getFilters()) {
            $parents[$index_id]->filter($nested_filter);
          }
        }
      }
    }
  }

  /**
   * Ensure that all results have all fields needed for sorting.
   *
   * @param array $results
   *   The results array, as in the 'results' key of the return value of
   *   SearchApiMultiQueryInterface::execute().
   */
  protected function ensureSortFields(array &$results) {
    $sort = array_keys($this->sort);
    // Eliminate special fields which are always included.
    foreach ($sort as $i => $key) {
      if ($key == 'search_api_id' || $key == 'search_api_relevance') {
        unset($sort[$i]);
      }
    }
    if (!$sort) {
      return;
    }
    // Determine what fields we need from items of each index.
    $fields = array();
    foreach ($sort as $key) {
      list($index_id, $field) = explode(':', $key, 2);
      if (!empty($this->indexes[$index_id])) {
        $fields[$index_id][$field] = $this->indexes[$index_id]->options['fields'][$field];
      }
    }
    if (!$fields) {
      return;
    }
    // Determine for which items we need the entity.
    $to_load = array();
    foreach ($results as $i => $result) {
      $results[$i] = $result += array('fields' => array(), 'entity' => NULL);
      if (empty($fields[$result['index_id']]) || $result['entity']) {
        continue;
      }
      foreach ($fields[$result['index_id']] as $field => $info) {
        if (!array_key_exists($field, $result['fields'])) {
          $to_load[$this->indexes[$result['index_id']]->item_type][$i] = $result['id'];
          break;
        }
      }
    }
    // Load items, as necessary.
    foreach ($to_load as $type => $ids) {
      $type_items = search_api_get_datasource_controller($type)->loadItems($ids);
      foreach ($ids as $i => $id) {
        if (isset($type_items[$id])) {
          $results[$i]['entity'] = $type_items[$id];
        }
      }
    }
    // Now extract the fields for each item.
    foreach ($results as $i => $result) {
      if (empty($fields[$result['index_id']])) {
        continue;
      }
      $item_fields = $fields[$result['index_id']];
      if (empty($result['entity'])) {
        $results[$i]['fields'] += array_fill_keys(array_keys($item_fields), NULL);
        continue;
      }
      $item_fields = array_diff_key($item_fields, $result['fields']);
      if ($item_fields) {
        $wrapper = $this->indexes[$result['index_id']]->entityWrapper($result['entity']);
        $item_fields = search_api_extract_fields($wrapper, $item_fields);
        foreach ($item_fields as $field => $info) {
          $results[$i]['fields'][$field] = $info['value'];
        }
      }
    }
  }

  /**
   * Compare two result arrays.
   *
   * Callback for uasort() within searchMultiple().
   *
   * @param array $a
   *   One result.
   * @param array $b
   *   The other result.
   *
   * @return int
   *   A negative number if $a should come before $b, 0 if both compare equal
   *   and a positive number otherwise.
   */
  protected function compareResults(array $a, array $b) {
    foreach ($this->sort as $key => $order) {
      // Get the sorting for this specific field.
      if ($key == 'search_api_relevance') {
        $comp = $a['score'] - $b['score'];
      }
      elseif ($key == 'search_api_id') {
        if (is_numeric($a['id']) && is_numeric($b['id'])) {
          $comp = $a['id'] - $b['id'];
        }
        else {
          $comp = strnatcasecmp($a['id'], $b['id']);
        }
      }
      else {
        list($index_id, $field) = explode(':', $key, 2);
        $a_applies = ($a['index_id'] == $index_id);
        $b_applies = ($b['index_id'] == $index_id);
        if ($a_applies == $b_applies) {
          if (!$a_applies) {
            continue;
          }
          $value_a = $a['fields'][$field];
          $value_b = $b['fields'][$field];
          if (is_numeric($value_a) && is_numeric($value_b)) {
            $comp = $value_a - $value_b;
          }
          else {
            $comp = strnatcasecmp($value_a, $value_b);
          }
        }
        else {
          $comp = $a_applies ? -1 : 1;
          // When the sort only applies to one of the two results, we always
          // want it in front of the other, regardless of $order.
          $order = 'ASC';
        }
      }

      // Now apply the specified order and either return or continue.
      if (!$comp) {
        continue;
      }
      return (int) ($order == 'ASC' ? $comp : -$comp);
    }
    return 0;
  }

  /**
   * Pre-execute hook for modifying search behaviour.
   */
  public function preExecute() {
    // Make sure to only execute this once per query.
    if (!$this->pre_execute) {
      $this->pre_execute = TRUE;

      // Add filter for languages.
      if (isset($this->options['languages'])) {
        $this->addLanguages($this->options['languages']);
      }

      // Filter indexes to those used. If no index was explicitly used, include
      // all of them.
      if ($this->used_indexes) {
        $this->indexes = array_intersect_key($this->indexes, $this->used_indexes);
      }

      // Add fulltext fields, unless set.
      if ($this->fields === NULL) {
        $this->fields = $this->index_fields = array();
        foreach ($this->indexes as $index_id => $index) {
          foreach ($index->getFulltextFields() as $f) {
            $this->fields[] = "$index_id:$f";
            $this->index_fields[$index_id][] = $f;
          }
        }
      }
      // If both keys and fields are given, indexes with no fields searched
      // should not be included.
      elseif ($this->keys) {
        $this->indexes = array_intersect_key($this->indexes, $this->index_fields);
      }

      // Filter the $servers property according to the used indexes.
      foreach ($this->servers as $server_id => $indexes) {
        foreach ($indexes as $index_id => $index) {
          if (!isset($this->indexes[$index_id])) {
            unset($this->servers[$server_id][$index_id]);
          }
        }
      }
      $this->servers = array_filter($this->servers);
    }
  }

  /**
   * Post-execute hook for modifying search behaviour.
   *
   * @param array $results
   *   The results returned by the server, which may be altered.
   */
  public function postExecute(array &$results) {}

  /**
   * {@inheritdoc}
   */
  public function getIndexes() {
    return $this->indexes;
  }

  /**
   * {@inheritdoc}
   */
  public function &getKeys() {
    return $this->keys;
  }

  /**
   * {@inheritdoc}
   */
  public function getOriginalKeys() {
    return $this->orig_keys;
  }

  /**
   * {@inheritdoc}
   */
  public function &getFields() {
    return $this->fields;
  }

  /**
   * {@inheritdoc}
   */
  public function getFilter() {
    return $this->filter;
  }

  /**
   * {@inheritdoc}
   */
  public function &getSort() {
    return $this->sort;
  }

  /**
   * {@inheritdoc}
   */
  public function getOption($name, $default = NULL) {
    return isset($this->options[$name]) ? $this->options[$name] : $default;
  }

  /**
   * {@inheritdoc}
   */
  public function setOption($name, $value) {
    $old = $this->getOption($name);
    $this->options[$name] = $value;
    return $old;
  }

  /**
   * {@inheritdoc}
   */
  public function &getOptions() {
    return $this->options;
  }

  /**
   * Implements the magic __toString() method to simplify debugging.
   */
  public function __toString() {
    $ret = '';
    if ($this->indexes) {
      $indexes = array();
      foreach ($this->indexes as $index) {
        $indexes[] = $index->machine_name;
      }
      $ret .= 'Indexes: ' . implode(', ', $indexes) . "\n";
    }
    $ret .= 'Keys: ' . str_replace("\n", "\n  ", var_export($this->orig_keys, TRUE)) . "\n";
    if (isset($this->keys)) {
      $ret .= 'Parsed keys: ' . str_replace("\n", "\n  ", var_export($this->keys, TRUE)) . "\n";
      $ret .= 'Searched fields: ' . (isset($this->fields) ? implode(', ', $this->fields) : '[ALL]') . "\n";
    }
    if ($filter = (string) $this->filter) {
      $filter = str_replace("\n", "\n  ", $filter);
      $ret .= "Filters:\n  $filter\n";
    }
    if ($this->sort) {
      $sort = array();
      foreach ($this->sort as $field => $order) {
        $sort[] = "$field $order";
      }
      $ret .= 'Sorting: ' . implode(', ', $sort) . "\n";
    }
    $options = $this->sanitizeOptions($this->options);
    $options = str_replace("\n", "\n  ", var_export($options, TRUE));
    $ret .= 'Options: ' . $options . "\n";
    return $ret;
  }

  /**
   * Sanitizes an array of options in a way that plays nice with var_export().
   *
   * @param array $options
   *   An array of options.
   *
   * @return array
   *   The sanitized options.
   */
  protected function sanitizeOptions(array $options) {
    foreach ($options as $key => $value) {
      if (is_object($value)) {
        $options[$key] = 'object (' . get_class($value) . ')';
      }
      elseif (is_array($value)) {
        $options[$key] = $this->sanitizeOptions($value);
      }
    }
    return $options;
  }

}
