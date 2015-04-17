<?php

/**
 * @file
 * Code necessary for the menu callbacks on pages that display usage data.
 */

/**
 * Helper function to display project usage help.
 *
 * @param $key
 *   URL argument that either identifies a project or a release node (either a
 *   node ID or a project short name), or NULL for the site-wide usage page.
 */
function _project_usage_help($key) {
  if (empty($key)) {
    return '<p>' . t('This page summarizes the usage of all projects on @site_name. For each week beginning on the given date the figures show the number of sites that reported they are using (any version of) the project. Detailed usage information for each release of a project is available by clicking the project name.', array('@site_name' => config_get('system.core', 'site_name'))) . '</p>';
  }
  elseif (is_numeric($key)) {
    $node = node_load($key);
  }
  else {
    $node = project_load($key);
  }
  if (project_node_is_project($node)) {
    return '<p>' . t('This page provides information about the usage of the %project_title project, including summaries across all versions and details for each release. For each week beginning on the given date the figures show the number of sites that reported they are using a given version of the project. ', array('%project_title' => $node->title)) . '</p>';
  }
  elseif (project_release_node_is_release($node)) {
    return '<p>' . t('For each week beginning on a given date, the figures show the number of sites that reported they are using the %release_title release. ', array('%release_title' => $node->title)) . '</p>';
  }
}

/**
 * Menu handler for project URLs.
 *
 * @param $key
 *   Node id or project machine_name. project nids and machine_names get the
 *   project usage page, release nids get the release usage page, and
 *   everything else gets a not found. In addition, if a user does not have
 *   permission to view the project or release node they've requested, they
 *   get an access denied page.
 */
function project_usage_dispatch($key) {
  // Load the node the user has requested. We want to only use project_load()
  // if the $key parameter is not numeric because project_load() will only
  // return a project node, and we want to allow $node to also be a release
  // node.
  if (is_numeric($key)) {
    $node = node_load($key);
  }
  else {
    $node = project_load($key);
  }

  if (!empty($node->nid)) {
    // Make sure that the user has the permission to view this project/
    // project_release node.
    if (node_access('view', $node)) {
      if (project_node_is_project($node)) {
        return project_usage_project_page($node);
      }
      if (project_release_node_is_release($node)) {
        return project_usage_release_page($node);
      }
    }
    else {
      return backdrop_access_denied();
    }
  }
  return backdrop_not_found();
}

/**
 * Display an overview of usage for all modules.
 */
function project_usage_overview() {
  backdrop_add_css(backdrop_get_path('module', 'project_usage') . '/project_usage.css');
  // Load the API functions we need for manipulating dates and timestamps.
  module_load_include('inc', 'project_usage', 'project_usage.date');

  // Grab an array of active week timestamps.
  $weeks = project_usage_get_active_weeks();

  // In order to get the project usage data into a sortable table, we've gotta
  // write a pretty evil query:
  //
  // - We need to create a separate column for each week to allow sorting by
  //   usage in any week (the TableSort extender requires that anything you can
  //   sort on has a distinct field in the underlying query). However, some
  //   weeks may not have any usage data, forcing us to use a LEFT JOIN,
  //   rather than the more efficient INNER JOIN.
  // - The LEFT JOINs mean we have to limit the entries in {node} so that
  //   we're not including things like forum posts, hence the WHERE IN below.
  // - Each project may have multiple records in {project_usage_week_project}
  //   to track usage for API version. We need to SUM() them to get a total
  //   count forcing us to GROUP BY. Sadly, I can't explain why we need
  //   SUM(DISTINCT)... it just works(TM).
  //
  // Also, because the machine_name is an entity field, and there's no clean
  // way to work that into an SQL query, then:
  // - Load all entities that have project usage data
  // - Cycle through them to build a map of entity_id -> machine_name
  // - Stuff the machine name into the project link when assembling the rows.

  $headers = array(
    t('#'),
    array(
      'field' => 'n.title',
      'data' => t('Project'),
    ),
  );
  $rows = array();

  $usage_week_nids = db_query('SELECT DISTINCT(nid) FROM {project_usage_week_project}')->fetchCol();
  if (!empty($usage_week_nids)) {
    $query = db_select('node', 'n')
      ->fields('n', array('nid', 'title'))
      ->condition('n.nid', $usage_week_nids, 'IN')
      ->condition('n.status', NODE_PUBLISHED)
      ->groupBy('n.nid')
      ->groupBy('n.title')
      ->addTag('node_access');

    foreach ($weeks as $i => $week) {
      // Note that "{$i}" in these query snippets are used to add a week integer
      // to the table and field aliases so we can uniquely identify each column
      // for sorting purposes.
      $query->addExpression("SUM(DISTINCT p{$i}.count)", "week{$i}");
      $query->leftJoin('project_usage_week_project', "p{$i}", "n.nid = p{$i}.nid AND p{$i}.timestamp = :timestamp{$i}", array(":timestamp{$i}" => $week));

      $header = array(
        'field' => "week{$i}",
        'data' => format_date($week, 'custom', PROJECT_USAGE_DATE_SHORT),
        'class' => array('project-usage-numbers'),
      );
      if ($i == 0) {
        $header['sort'] = 'desc';
      }
      $headers[] = $header;
    }

    // Check for a cached page. The cache id needs to take into account the sort
    // column and order.
    $sort = tablesort_init($headers);
    $cid = 'overview:' . $sort['sql'] . ':' . $sort['sort'] . ':' . pager_find_page();
    $items_per_page = 100;
    if ($cached = cache_get($cid, 'cache_project_usage')) {
      $rows = $cached->data;
      if ($total = cache_get('overview:total', 'cache_project_usage')) {
        pager_default_initialize($total->data, $items_per_page);
      }
    }
    else {
      $result = $query
        ->extend('TableSort')
        ->orderByHeader($headers)
        ->extend('PagerDefault')
        ->limit($items_per_page)
        ->execute();

      $number = pager_find_page() * $items_per_page + 1;
      $rows = array();
      foreach ($result as $line) {
        $machine_name = project_get_machine_name_from_nid($line->nid);
        $row = array(
          array('data' => $number++),
          array('data' => l($line->title, 'project/usage/' . $machine_name)),
        );
        foreach ($weeks as $i => $week) {
          $row[] = array(
            'data' => number_format($line->{"week{$i}"}),
            'class' => array('project-usage-numbers'),
          );
        }
        $rows[] = $row;
      }

      // Cache the completed page.
      cache_set($cid, $rows, 'cache_project_usage', project_usage_cache_time());
      // Cache number of projects
      global $pager_total_items;
      cache_set('overview:total', reset($pager_total_items), 'cache_project_usage', project_usage_cache_time());
    }
  }

  return array(
    'usage' => array(
      '#theme' => 'table',
      '#header' => $headers,
      '#rows' => $rows,
      '#attributes' => array('id' => 'project-usage-all-projects'),
      '#empty' => t('No items available'),
    ),
    'pager' => array(
      '#theme' => 'pager',
    ),
  );
}

/**
 * Display the usage history of a project node.
 */
function project_usage_project_page($node) {
  backdrop_add_css(backdrop_get_path('module', 'project_usage') . '/project_usage.css');
  // Load the API functions we need for manipulating dates and timestamps.
  module_load_include('inc', 'project_usage', 'project_usage.date');

  project_project_set_breadcrumb($node, TRUE);
  backdrop_set_title(t('Usage statistics for %project', array('%project' => $node->title)), PASS_THROUGH);

  // In order to keep the database load down we need to cache these pages.
  // Because the release usage table is sortable, the cache id needs to take
  // into account the sort parameters. The easiest way to ensure we have valid
  // sorting parameters is to build the table headers and let the tablesort
  // functions do it. This means we end up doing most of the work to build the
  // page's second table early on. We might as well finish the job, then build
  // the other table and output them in the correct order.

  // Grab an array of active week timestamps.
  $weeks = project_usage_get_active_weeks();

  $releases = project_release_query_releases($node->nid);

  // If there are no releases for this project, we can skip the rest
  // of this function.
  if (empty($releases)) {
    return theme('project_usage_project_page', array('project' => $node));
  }

  // Build a table showing this week's usage for each release. In order to get
  // the release usage data into a sortable table, we need another evil query:
  // - We need to create a separate column for each week to allow sorting by
  //   usage in any week (the TableSort extender requires that anything you can
  //   sort on has a distinct field in the underlying query). However, some
  //   weeks may not have any usage data, forcing us to use a LEFT JOIN,
  //   rather than the more efficient INNER JOIN.
  // - We need to create a column for each week but some weeks may not have any
  //   usage data, forcing us to use a LEFT JOIN, rather than the more
  //   efficient INNER JOIN.
  // - The LEFT JOINs mean we have to limit the entries in {node} so that we're
  //   not including things like forum posts, hence the WHERE IN below.

  $query = db_select('node', 'n')
    ->fields('n', array('nid'))
    ->condition('n.nid', array_keys($releases), 'IN')
    ->condition('n.status', 1)
    ->addTag('node_access');

  $release_header = array(array(
      'field' => 'n.title',
      'data' => t('Release'),
      'sort' => 'desc',
    ));

  foreach ($weeks as $i => $week) {
    // Note that "{$i}" in these query snippets are used to add a week integer
    // to the table and field aliases so we can uniquely identify each column
    // for sorting purposes.
    $query->addField('p' . $i, 'count', 'week' . $i);
    $query->leftJoin('project_usage_week_release', "p{$i}", "n.nid = p{$i}.nid AND p{$i}.timestamp = :timestamp{$i}", array(":timestamp{$i}" => $week));

    $release_header[] = array(
      'field' => "week{$i}",
      'data' => format_date($week, 'custom', PROJECT_USAGE_DATE_SHORT),
      'class' => array('project-usage-numbers'),
    );
  }

  // Check for a cached page. The cache id needs to take into account the sort
  // column and order.
  $sort = tablesort_init($release_header);
  $cid = 'project:' . $node->nid . ':' . $sort['sql'] . ':' . $sort['sort'];
  if ($cached = cache_get($cid, 'cache_project_usage')) {
    $release_rows = $cached->data['release_rows'];
    $project_header = $cached->data['project_header'];
    $project_rows = $cached->data['project_rows'];
  }
  else {
    $result = $query
      ->extend('TableSort')
      ->orderByHeader($release_header)
      ->execute();

    $release_rows = array();
    foreach ($result as $line) {
      $sum = 0;
      $row = array(array('data' => l($releases[$line->nid]->project_release['version'], 'project/usage/' . $line->nid)));
      foreach ($weeks as $i => $week) {
        $sum += $line->{"week{$i}"};
        $row[] = array(
          'data' => number_format($line->{"week{$i}"}),
          'class' => array('project-usage-numbers'),
        );
      }
      // Skip any release with no usage.
      if ($sum) {
        $release_rows[] = $row;
      }
    }

    // Build a table of the weekly usage data with a column for each API version.
    // Get an array of the weeks going back as far as we have data...
    $oldest = db_query("SELECT MIN(puwp.timestamp) FROM {project_usage_week_project} puwp WHERE puwp.nid = :nid", array(':nid' => $node->nid))->fetchField();
    if ($oldest === NULL) {
      $weeks = array();
    }
    else {
      $weeks = project_usage_get_weeks_since($oldest);
      // ...ignore the current week, since we won't have usage data for that and
      // reverse the order so it's newest to oldest.
      array_pop($weeks);
      $weeks = array_reverse($weeks);
    }

    // The number of columns varies depending on how many different API versions
    // are in use. Set up the header and a blank, template row, based on the
    // number of distinct API versions in use.
    $project_header = array(0 => array('data' => t('Week')));
    $blank_row = array(0 => array('data' => ''));
    $result = db_query("SELECT DISTINCT p.version_api FROM {project_usage_week_project} p WHERE p.nid = :nid ORDER BY p.version_api", array(':nid' => $node->nid));
    foreach ($result as $row) {
      $project_header[$row->version_api] = array(
        'data' => check_plain($row->version_api),
        'class' => array('project-usage-numbers'),
      );
      $blank_row[$row->version_api] = array(
        'data' => 0,
        'class' => array('project-usage-numbers'),
      );
    }
    $project_header['total'] = array(
      'data' => t('Total'),
      'class' => array('project-usage-numbers'),
    );
    $blank_row['total'] = array(
      'data' => 0,
      'class' => array('project-usage-numbers'),
    );

    // Now create a blank table with a row for each week and formatted date in
    // the first column...
    $project_rows = array();
    foreach ($weeks as $week) {
      $project_rows[$week] = $blank_row;
      $project_rows[$week][0]['timestamp'] = $week;
      $project_rows[$week][0]['data'] = format_date($week, 'custom', PROJECT_USAGE_DATE_LONG);
    }

    // ...then fill it in with our data.
    $result = db_query("SELECT timestamp, version_api, count FROM {project_usage_week_project} WHERE nid = :nid", array(':nid' => $node->nid));
    foreach ($result as $row) {
      $project_rows[$row->timestamp][$row->version_api]['data'] = $row->count;
    }

    // ...and add the weekly totals across all API versions
    foreach ($project_rows as $timestamp => $row) {
      $total = 0;
      foreach ($row as $version_api => $data) {
        if (!empty($version_api) && !empty($data['data'])) {
          $total += $data['data'];
          $project_rows[$timestamp][$version_api]['data'] = number_format($data['data']);
        }
      }
      $project_rows[$timestamp]['total']['data'] = number_format($total);
    }
  }

  // Cache the completed page.
  $cache['release_rows'] = $release_rows;
  $cache['project_header'] = $project_header;
  $cache['project_rows'] = $project_rows;
  cache_set($cid, $cache, 'cache_project_usage', project_usage_cache_time());

  return theme('project_usage_project_page', array('project' => $node, 'release_header' => $release_header, 'release_rows' => $release_rows, 'project_header' => $project_header, 'project_rows' => $project_rows));
}

/**
 * Display the usage history of a release node.
 */
function project_usage_release_page($node) {
  backdrop_add_css(backdrop_get_path('module', 'project_usage') . '/project_usage.css');

  // Load the API functions we need for manipulating dates and timestamps.
  module_load_include('inc', 'project_usage', 'project_usage.date');

  $project = node_load(project_release_get_release_project_nid($node));
  project_project_set_breadcrumb($project, TRUE, array(l(t('Usage'), 'project/usage/' . $project->nid)));
  backdrop_set_title(t('Usage statistics for %release', array('%release' => $node->title)), PASS_THROUGH);

  // Check for a cached page.
  $cid = "release:{$node->nid}";
  if ($cached = cache_get($cid, 'cache_project_usage')) {
    $rows = $cached->data;
  }
  else {
    // Table displaying the usage back through time.
    $rows = array();
    $query = db_query("SELECT timestamp, count FROM {project_usage_week_release} WHERE nid = :nid ORDER BY timestamp DESC", array(':nid' => $node->nid));
    foreach ($query as $row) {
      $rows[] = array(
        array(
          'data' => format_date($row->timestamp, 'custom', PROJECT_USAGE_DATE_LONG),
          'timestamp' => $row->timestamp,
        ),
        array(
          'data' => number_format($row->count),
          'class' => array('project-usage-numbers'),
        ),
      );
    }
  }
  $header = array(
    array('data' => t('Week starting')),
    array(
      'data' => t('Count'),
      'class' => array('project-usage-numbers'),
    ),
  );

  // Cache the completed page.
  cache_set($cid, $rows, 'cache_project_usage', project_usage_cache_time());

  return theme('project_usage_release_page', array('project' => $project, 'release' => $node, 'header' => $header, 'rows' => $rows));
}

/**
 * Theme the output of project/usage/<project> page.
 *
 * @param $variables
 *   An array of theme information, with the following keys:
 *
 *     project:
 *       A fully loaded $node object for a project.
 *     release_header:
 *       A table header for the release usage table.
 *     release_rows:
 *       Table rows for the release usage table.
 *     project_header:
 *       A table header for the weekly project usage table.
 *     project_rows:
 *       Table rows for the weekly project usage table.
 */
function theme_project_usage_project_page($variables) {
  $project = $variables['project'];
  $release_header = $variables['release_header'];
  $release_rows = $variables['release_rows'];
  $project_header = $variables['project_header'];
  $project_rows = $variables['project_rows'];
  $output = theme('project_usage_header_links', array('project' => $project));

  if (empty($release_rows)) {
    // There are no published releases for a project that the user has access
    // to view.
    $output .= '<p>' . t('There is no usage information for any release of this project.') . '</p>';
    return $output;
  }

  $output .= '<h3>' . t('Weekly project usage') . '</h3>';
  $output .= theme('project_usage_chart_by_release', array('header' => $project_header, 'rows' => $project_rows));
  $output .= theme('table', array('header' => $project_header, 'rows' => $project_rows, 'attributes' => array('id' => 'project-usage-project-api')));
  $output .= '<h3>' . t('Recent release usage') . '</h3>';
  $output .= theme('table', array('header' => $release_header, 'rows' => $release_rows, 'attributes' => array('id' => 'project-usage-project-releases')));
  return $output;
}

/**
 * Theme the output of the project/usage/<release nid> page.
 *
 * @param $variables
 *   An array of theme information, with the following keys:
 *
 *     project:
 *       A fully loaded $node object for a project.
 *     release:
 *       A fully loaded $node object for a release.
 *     header:
 *       A table header for the release usage table.
 *     rows:
 *       Table rows for the release usage table.
 */
function theme_project_usage_release_page($variables) {
  $project = $variables['project'];
  $release = $variables['release'];
  $header = $variables['header'];
  $rows = $variables['rows'];
  $output = theme('project_usage_header_links', array('project' => $project, 'release' => $release));

  // If there is no usage information for a release, don't just
  // display an empty usage table.
  if (empty($rows)) {
    $output .= '<p>' . t('There is no usage information for this release.') . '</p>';
    return $output;
  }

  $output .= theme('project_usage_chart_by_release', array('header' => $header, 'rows' => $rows));
  $output .= theme('table', array('header' => $header, 'rows' => $rows, 'attributes' => array('id' => 'project-usage-release')));
  return $output;
}

/**
 * Create list of links at the top of a usage statistics page.
 *
 * @param $variables
 *   An array of theme information, with the following keys:
 *
 *     project:
 *       A fully loaded $node object for a project.
 *     release:
 *       If the current statistics page is for a release, the fully loaded
 *       $node object for that release.
 *
 * @return
 *   Themed HTML of a list of links for the top of a statistics page.
 */
function theme_project_usage_header_links($variables) {
  $project = $variables['project'];
  $release = $variables['release'];
  $links = array();
  $links[] = l(t('%project_name project page', array('%project_name' => $project->title)), 'node/' . $project->nid, array('html' => TRUE));
  if (!empty($release)) {
    $links[] = l(t('%release_name release page', array('%release_name' => $release->title)), 'node/' . $release->nid, array('html' => TRUE));
    $links[] = l(t('All %project_name usage statistics', array('%project_name' => $project->title)), 'project/usage/' . $project->project['name'], array('html' => TRUE));
  }
  $links[] = l(t('Usage statistics for all projects'), 'project/usage');
  return theme('item_list', array('items' => $links));
}

/**
 * Convert the usage table data into a graph.
 *
 * First column should be the weeks, each subsequent column should be a data
 * series.
 *
 * @param $variables
 *   An array of theme information, with the following keys:
 *     header:
 *       A table header for the weekly usage table.
 *     rows:
 *       Table rows for the weekly usage table.
 * @return
 *   An HTML IMG element, or empty string if there is an error such as
 *   insufficient data.
 */
function theme_project_usage_chart_by_release($variables) {
  $header = $variables['header'];
  $rows = $variables['rows'];
  // Make sure we have enough data to make a useful chart.
  if (count($rows) < 2 || count($header) < 2) {
    return '';
  }

  // Make sure we have flot.
  if (!module_exists('flot')) {
    return '';
  }

  // Reverse the order of the rows so it's oldest to newest.
  $rows = array_reverse($rows);

  // Pull the API versions from the table header for use as a legend. Since the
  // table is keyed strangely, make note of the version_api order so we
  // can efficiently iterate over the columns.
  $series = array();
  $mapping = array();
  foreach ($header as $version_api => $cell) {
    $series[] = array(
      'label' => $cell['data'],
    );
    $mapping[] = $version_api;
  }
  // Drop the date column from the legend and mapping since it's the other axis.
  unset($series[0]);
  unset($mapping[0]);

  // Rotate the table so each series is in a row in the array and grab the
  // dates for use as axis labels.
  foreach (array_values($rows) as $i => $row) {
    $timestamp = $row[0]['timestamp'];
    foreach ($mapping as $j => $version_api) {
      // FIXME: The table values have commas in them from number_format(). We
      // need to remove them because we'll use commas to separate the values
      // in the URL string. It might be better to pass in clean number values
      // and format them here rather than have to uncook them.
      $series[$j]['data'][] = array($timestamp * 1000, (int) str_replace(',', '', $row[$version_api]['data']));
    }
  }

  return theme('flot_graph', array(
    'data' => array_values($series),
    'options' => array(
      'xaxis' => array(
        'mode' => 'time',
      ),
      'colors' => array('#EDAA00', '#0062A0', '#A17300', '#ED8200', '#38B4BA', '#215D6E'), // Might need more colors than this.
      'series' => array(
        'shadowSize' => 0,
      ),
      'legend' => array(
        'show' => 1,
        'position' => 'nw',
        'noColumns' => 2,
        'labelBoxBorderColor' => 'transparent',
      ),
      'grid' => array(
        'borderWidth' => 0,
      ),
    )
  ));
}

/**
 * Extract a subset of labels at regular intervals from a set.
 *
 * @param $labels
 *   Array of values to choose from.
 * @param $n
 *   Number of segments in the set. This number should divide $labels with no
 *   remander.
 */
function project_usage_chart_label_subset($labels, $n) {
  $subset = array();
  $count = count($labels) - 1;
  // We can't give them back more labels that we're given.
  $n = min($count, $n - 1);
  for ($i = 0; $i <= $n; $i++) {
    $subset[] = $labels[(int) ($count * ($i / $n))];
  }
  return $subset;
}

/**
 * Add axis labels to the chart arguments.
 *
 * @param $args
 *   The array where the chart is being built.
 * @param $labels
 *   Array keyed by axis (x, t, y, r) the value is an array of labels for that
 *   axis.
 * @see http://code.google.com/apis/chart/labels.html#multiple_axes_labels
 */
function project_usage_chart_axis_labels(&$args, $labels) {
  $keys = array_keys($labels);
  $args['chxt'] = implode(',', $keys);

  $l = array();
  foreach ($keys as $i => $key) {
    $l[$i] = $i . ':|' . implode('|', $labels[$key]);
  }
  $args['chxl'] = implode('|', $l);
}

