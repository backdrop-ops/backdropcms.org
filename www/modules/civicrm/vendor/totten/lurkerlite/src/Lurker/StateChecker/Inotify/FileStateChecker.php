<?php

namespace Lurker\StateChecker\Inotify;

use Lurker\Resource\FileResource;
use Lurker\Event\FilesystemEvent;

/**
 * File state checker.
 *
 * @author Yaroslav Kiliba <om.dattaya@gmail.com>
 */
class FileStateChecker extends ResourceStateChecker
{
    /**
     * Initializes checker.
     *
     * @param CheckerBag   $bag
     * @param FileResource $resource
     * @param int          $eventsMask
     */
    public function __construct(CheckerBag $bag, FileResource $resource, $eventsMask = FilesystemEvent::ALL)
    {
        parent::__construct($bag, $resource, $eventsMask);
    }

    /**
     * {@inheritdoc}
     */
    public function setEvent($mask, $name = '')
    {
        $this->event = $mask;
    }

    /**
     * {@inheritdoc}
     */
    public function getChangeset()
    {
        $changeset = array();

        $this->handleItself();

        if ($this->fromInotifyMask($this->event)) {
            $changeset[] =
                array(
                    'resource' => $this->getResource(),
                    'event'    => $this->fromInotifyMask($this->event)
                );
        }
        $this->setEvent(false);

        return $changeset;
    }

    /**
     * Handles event related to itself.
     */
    protected function handleItself()
    {
        if ($this->isMoved($this->event)) {
            if ($this->getResource()->exists() && $this->addWatch() === $this->id) {
                return;
            }

            $this->unwatch($this->id);
        }

        if ($this->getResource()->exists()) {
            if ($this->isIgnored($this->event) || $this->isMoved($this->event) || !$this->id) {
                $this->setEvent($this->id ? IN_MODIFY : IN_CREATE);
                $this->watch();
            }
        } elseif ($this->id) {
            $this->event = IN_DELETE;
            $this->getBag()->remove($this);
            $this->unwatch($this->id);
            $this->id = null;
        }
    }
}
