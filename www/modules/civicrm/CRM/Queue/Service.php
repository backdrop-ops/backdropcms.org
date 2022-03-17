<?php
/*
 +--------------------------------------------------------------------+
 | Copyright CiviCRM LLC. All rights reserved.                        |
 |                                                                    |
 | This work is published under the GNU AGPLv3 license with some      |
 | permitted exceptions and without any warranty. For full license    |
 | and copyright information, see https://civicrm.org/licensing       |
 +--------------------------------------------------------------------+
 */

/**
 * The queue service provides an interface for creating or locating
 * queues. Note that this approach hides the details of data-storage:
 * different queue-providers may store the queue content in different
 * ways (in memory, in SQL, or in an external service).
 *
 * ```
 * $queue = CRM_Queue_Service::singleton()->create(array(
 *   'type' => 'interactive',
 *   'name' => 'upgrade-tasks',
 * ));
 * $queue->createItem($myData);
 *
 * // Some time later...
 * $item = $queue->claimItem();
 * if ($item) {
 *   if (my_process($item->data)) {
 *     $queue->deleteItem($item);
 *   } else {
 *     $queue->releaseItem($item);
 *   }
 * }
 * ```
 */
class CRM_Queue_Service {

  protected static $_singleton;

  /**
   * FIXME: Singleton pattern should be removed when dependency-injection
   * becomes available.
   *
   * @param bool $forceNew
   *   TRUE if a new instance must be created.
   *
   * @return \CRM_Queue_Service
   */
  public static function &singleton($forceNew = FALSE) {
    if ($forceNew || !self::$_singleton) {
      self::$_singleton = new CRM_Queue_Service();
    }
    return self::$_singleton;
  }

  /**
   * Queues.
   *
   * Format is (string $queueName => CRM_Queue_Queue).
   *
   * @var array
   */
  public $queues;

  /**
   * Class constructor.
   */
  public function __construct() {
    $this->queues = [];
  }

  /**
   * Create a queue. If one already exists, then it will be reused.
   *
   * @param array $queueSpec
   *   Array with keys:
   *   - type: string, required, e.g. `Sql`, `SqlParallel`, `Memory`
   *   - name: string, required, e.g. "upgrade-tasks"
   *   - reset: bool, optional; if a queue is found, then it should be
   *     flushed; default to TRUE
   *   - (additional keys depending on the queue provider).
   *   - is_persistent: bool, optional; if true, then this queue is loaded from `civicrm_queue` list
   *   - is_autorun: bool, optional; if true, then this queue will be auto-scanned
   *     by background task-runners
   *
   * @return CRM_Queue_Queue
   */
  public function create($queueSpec) {
    if (is_object($this->queues[$queueSpec['name']] ?? NULL) && empty($queueSpec['reset'])) {
      return $this->queues[$queueSpec['name']];
    }

    if (!empty($queueSpec['is_persistent'])) {
      $queueSpec = $this->findCreateQueueSpec($queueSpec);
    }
    $queue = $this->instantiateQueueObject($queueSpec);
    $exists = $queue->existsQueue();
    if (!$exists) {
      $queue->createQueue();
    }
    elseif (@$queueSpec['reset']) {
      $queue->deleteQueue();
      $queue->createQueue();
    }
    else {
      $queue->loadQueue();
    }
    $this->queues[$queueSpec['name']] = $queue;
    return $queue;
  }

  /**
   * Find/create the queue-spec. Specifically:
   *
   * - If there is a stored queue, use its spec.
   * - If there is no stored queue, and if we have enough information, then create queue.
   *
   * @param array $queueSpec
   * @return array
   *   Updated queueSpec.
   * @throws \CRM_Core_Exception
   */
  protected function findCreateQueueSpec(array $queueSpec): array {
    $storageFields = ['type', 'is_autorun'];
    $dao = new CRM_Queue_DAO_Queue();
    $dao->name = $queueSpec['name'];
    if ($dao->find(TRUE)) {
      return array_merge($queueSpec, CRM_Utils_Array::subset($dao->toArray(), $storageFields));
    }

    if (empty($queueSpec['type'])) {
      throw new \CRM_Core_Exception(sprintf('Failed to find or create persistent queue "%s". Missing field "%s".',
        $queueSpec['name'], 'type'));
    }
    $queueSpec = array_merge(['is_autorun' => FALSE], $queueSpec);
    $dao->copyValues($queueSpec);
    $dao->insert();

    return $queueSpec;
  }

  /**
   * Look up an existing queue.
   *
   * @param array $queueSpec
   *   Array with keys:
   *   - type: string, required, e.g. `Sql`, `SqlParallel`, `Memory`
   *   - name: string, required, e.g. "upgrade-tasks"
   *   - (additional keys depending on the queue provider).
   *   - is_persistent: bool, optional; if true, then this queue is loaded from `civicrm_queue` list
   *
   * @return CRM_Queue_Queue
   */
  public function load($queueSpec) {
    if (is_object($this->queues[$queueSpec['name']] ?? NULL)) {
      return $this->queues[$queueSpec['name']];
    }
    if (!empty($queueSpec['is_persistent'])) {
      $queueSpec = $this->findCreateQueueSpec($queueSpec);
    }
    $queue = $this->instantiateQueueObject($queueSpec);
    $queue->loadQueue();
    $this->queues[$queueSpec['name']] = $queue;
    return $queue;
  }

  /**
   * Convert a queue "type" name to a class name.
   *
   * @param string $type
   *   - type: string, required, e.g. `Sql`, `SqlParallel`, `Memory`
   * @return string
   *   Class-name
   */
  protected function getQueueClass($type) {
    $type = preg_replace('/[^a-zA-Z0-9]/', '', $type);
    $className = 'CRM_Queue_Queue_' . $type;
    // FIXME: when used with class-autoloader, this may be unnecessary
    if (!class_exists($className)) {
      $classFile = 'CRM/Queue/Queue/' . $type . '.php';
      require_once $classFile;
    }
    return $className;
  }

  /**
   * @param array $queueSpec
   *   See create().
   *
   * @return CRM_Queue_Queue
   */
  protected function instantiateQueueObject($queueSpec) {
    // note: you should probably never do anything else here
    $class = new ReflectionClass($this->getQueueClass($queueSpec['type']));
    return $class->newInstance($queueSpec);
  }

}
