# XML Sitemap Node

The XML sitemap node module, part of the [XML sitemap](https://backdropcms.org/project/xmlsitemap)
package, enables content nodes to be in the sitemap.

## Dependencies

 - [XML sitemap](https://backdropcms.org/project/xmlsitemap)

## Installation and Usage

1. Install XML Sitemap (and then this module) using the [official Backdrop CMS instructions](https://backdropcms.org/guide/modules)
2. Enable the **XML sitemap** module and the **XML sitemap node** submodule.
4. To add nodes to the sitemap, visit the Edit page of the Content Type which
   you want to appear on the sitemap.
5. Select the XML sitemap horizontal tab.
6. Under "Inclusion" change "Excluded" to become "Included". Save.
7. If enabled, all content of the specific node type will be included.
   Individual nodes can be excluded on their specific node edit page.
8. Once that is all complete, go to Configurations --> Search and Metadata -->
   XML sitemap.
9. Select the Rebuild Links tab in the upper right.
10. Select on "Rebuild sitemap" even if the message says that you do not need
   to.
11. Now you're taken back to the config page which shows you the link to your
    XML sitemap which you can select and confirm that pages have been added.

## Issues

 - Bugs and Feature requests should be reported in the
   [Issue Queue](https://github.com/backdrop-contrib/xmlsitemap/issues).

## Current Maintainers

 - [Laryn Kragt Bakker](https://github.com/laryn) - [CEDC.org](https://cedc.org)
 - Collaboration and co-maintainers welcome!

## Credits

- Ported to Backdrop CMS by [Alex Finnarn](https://github.com/alexfinnarn)
- Maintainers on drupal.org include
  [Andrei Mateescu](https://www.drupal.org/u/amateescu),
  [Dave Reid](https://www.drupal.org/u/dave-reid),
  [Juampy NR](https://www.drupal.org/u/juampynr), and
  [Tasya Rukmana](https://www.drupal.org/u/tadityar)

## License

This project is GPL v2 software. See the LICENSE.txt file in the root directory
for complete text.
