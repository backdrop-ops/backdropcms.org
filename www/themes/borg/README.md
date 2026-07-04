BORG
====

This is the theme used on backdropcms.org, forum.backdropcms.org and other
Backdrop web properties. Also, resistance is futile.


Installation
------------

- Install this theme using the official Backdrop CMS instructions at
  https://backdropcms.org/guide/themes

- There are no theme settings for this theme.

Documentation
-------------

### Full-width hero region with grid background.

To Have a hero region with a grid background image as seen on backdropcms.org,
in your sub-theme, override the layout template for the home page by adding
 `--home` to the end of the file name.

For example: `layout--boxton.tpl.php` becomes `layout--boxton--home.tpl.php`.
Once copied, change the `l-top` region to be `l-hero` as follows:

```
<?php if (!empty($content['top']) || $messages): ?>
  <div class="l-hero">
    <div class="l-hero-image">
      <div class="l-hero-vgradients"><div class="l-hero-hgradients">
        <?php if ($messages): ?>
          <div class="l-messages" role="status" aria-label="<?php print t('Status messages'); ?>">
            <?php print $messages; ?>
          </div>
        <?php endif; ?>

        <?php print $content['top']; ?>
      </div></div>
    </div>
  </div>
<?php endif; ?>
```

If your sub-theme has overridden `page-front.css` ensure the two sections on
`Hero Region` and `Blocks in hero region` have been copied, and are up to date.

### Utility classes

There are a few utility classes provided with this theme:
- menu-solid will render a sidebar menu on a light gray background.
- more as they come!


Issues
------

Bugs and Feature requests should be reported in the Issue Queue:
https://github.com/backdrop-contrib/borg/issues

Current Maintainers
-------------------

- Jen Lampton (https://github.com/jenlampton)
- Seeking additional maintainers

Credits
-------

- Designed for Backdrop CMS by [Darius Garza](http://dariusgarza.com/).
- Updated Designs by [Nica Lorber](https://www.nicalorber.com).
- Coded for Backdrop CMS by [Jen Lampton](https://github.com/jenlampton).
- Code often fixed by [Wes Ruvacabla](https://github.com/wedruv).

License
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.
