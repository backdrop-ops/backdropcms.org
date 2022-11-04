<?php
namespace Lurker\Event;

use Lurker\Resource\TrackedResource;
use Lurker\Exception\InvalidArgumentException;
use Lurker\Resource\ResourceInterface;
use Lurker\Resource\FileResource;
use Lurker\Resource\DirectoryResource;

trait FilesystemEventTrait
{

    private $tracked;
    private $resource;
    private $type;

    protected static $types = array(
    1 => 'create',
    2 => 'modify',
    4 => 'delete',
    );

  /**
   * Initializes resource event.
   *
   * @param TrackedResource   $tracked  resource, that being tracked
   * @param ResourceInterface $resource resource instance
   * @param integer           $type     event type bit
   */
    public function __construct(TrackedResource $tracked, ResourceInterface $resource, $type)
    {
        if (!isset(self::$types[$type])) {
            throw new InvalidArgumentException('Wrong event type providen');
        }

        $this->tracked  = $tracked;
        $this->resource = $resource;
        $this->type     = $type;
    }

  /**
   * Returns resource, that being tracked while event occured.
   *
   * @return int
   */
    public function getTrackedResource()
    {
        return $this->tracked;
    }

  /**
   * Returns changed resource.
   *
   * @return ResourceInterface
   */
    public function getResource()
    {
        return $this->resource;
    }

  /**
   * Returns true is resource, that fired event is file.
   *
   * @return Boolean
   */
    public function isFileChange()
    {
        return $this->resource instanceof FileResource;
    }

  /**
   * Returns true is resource, that fired event is directory.
   *
   * @return Boolean
   */
    public function isDirectoryChange()
    {
        return $this->resource instanceof DirectoryResource;
    }

  /**
   * Returns event type.
   *
   * @return integer
   */
    public function getType()
    {
        return $this->type;
    }

  /**
   * Returns event type string representation.
   *
   * @return string
   */
    public function getTypeString()
    {
        return self::$types[$this->getType()];
    }
}
