Documentation
===============================================================================

This theme was intended for backdropcms.org, and then adapted for use on
forum.backdropcms.org as well. As such, common elements have been separated
out into separate files after-the-fact, and it may not be done perfectly ;)

This file intends to document the following:
* Stylesheets: Which to override in your base theme, and why.
* Block styles: classes that can be added to blocks, and what they do.
* Colors used: where and why


Stylesheets
===============================================================================

We do not recommend that these files be overridden in a sub-theme:

* base.css - contains general element styles. (include in editor)
* layout.css - contains general overrides to core layouts.
* components.css - contains general styles for blocks and page elements.
* components-menus.css - contains genreal menu styles.
* components-styles.css - contains classes that can be applied to blocks.
* menu-dropdown.theme.css - contans color and behavior for drop-down menu.
* menu-toggle.theme.css - contains color and placement for hamburger menu.

We do recommend that these files are overridden in a sub-theme:

* layout-pages.css - contains page-specific overrides to general styles for
    pages that only exist on the backdropcms.org site.
* components-blocks.css - contains styles for blocks that only exist on the
    backdropcms.org site.
* components-views.css - contains styles for views that only exist on the
    backdropcms.org site.
* ckeditor-iframe.css - contains patterns for styles available in the editor
    only on the backdropcms.org site.


Block Styles
===============================================================================

full-width - Add the class "full-width" to have a block break out of the content
             area and span the full width of the page. This class also adds a
             light gray background and medium gray borders.

dark - Add the class "dark" to any block to make the block's blackground black,
       and the text white.


Color List
===============================================================================

The Borg theme is mostly black and white but there are several accent gray
colors. We use a very specific shade of blue that is accessable on both white
and gray backgrounds.

True Black: #000;

True White: #fff;

Blue: #007CBA;
  This color blue should be accessable on both a white background, and gray.

Backgroupd gray: #f7f7f7;
  This medium gray is used mainly as a background color. It's used on the
  full-width banners and behind views exposed filters.

Border gray: #E8E6E5;
  This darker gray is used as a border color for the regions that have a
  background, and on form elements. It should replace the use of the more
  standard #ccc;

Darker border gray: #d7d7d7;
  This even darker border color shoud be used where a 1px border is needed
  instead of 2px, for example, for inline node and comment links.

Disabled gray: #7D7D7D;
  This darkish gray should be used when an element on the page is disabled. It
  should replace the more standard #bbb; or #999;

Soft Black: #444b53;
  This softer color should be used for form input fields and places where the
  text needs to be slightly de-emphasized. It should be used to replace the more
  standard #555.

