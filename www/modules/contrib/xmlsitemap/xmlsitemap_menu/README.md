# XML Sitemap Menu

The XML sitemap menu module, part of the [XML sitemap](https://backdropcms.org/project/xmlsitemap)
package, enables menu links to be on the site map. The XML sitemap module
creates a sitemap that conforms to the sitemaps.org specification. This helps
search engines to more intelligently crawl a website and keep their results up
to date.

## Dependencies

 - [XML sitemap](https://backdropcms.org/project/xmlsitemap)

## Installation and Usage

1. Install XML Sitemap (and then this module) using the [official Backdrop CMS instructions](https://backdropcms.org/guide/modules)
2. Enable the XML sitemap module and the XML sitemap menu submodule.
4. Navigate to **Administration > Configuration > Search > XML Sitemap**.
5. Select the Settings tab and there will be a Menu link fieldset. Open.
6. Choose the menu link to be edited. There will now be a XML sitemap horizontal
   tab. Under "Inclusion" change "Excluded" to become "Included". Select Save.
7. Once that is all complete, go to Configuration > Search and Metadata > XML
   Sitemap.
8. Select the Rebuild Links tab in the upper right.
9. Select "Rebuild sitemap" even if the message says that you do not need to.
10. Now you're taken back to the configuration page which shows you the link to
    your XML sitemap which you can select and confirm that pages have been
    added.


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
