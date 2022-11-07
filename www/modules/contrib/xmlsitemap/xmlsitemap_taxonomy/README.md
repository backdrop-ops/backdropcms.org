# XML Sitemap Taxonomy

The XML sitemap taxonomy module, part of the [XML sitemap](https://backdropcms.org/project/xmlsitemap)
package, adds taxonomy term links to the sitemap.

## Dependencies

 - [XML sitemap](https://backdropcms.org/project/xmlsitemap)

## Installation and Usage

1. Install XML Sitemap (and then this module) using the [official Backdrop CMS instructions](https://backdropcms.org/guide/modules)
2. Enable the **XML sitemap** module and the **XML sitemap taxonomy** submodule.
4. Navigate to **Administration > Structure > Taxonomy**.
5. To include a whole vocabulary in the sitemap, click "edit vocabulary".
   Select the XML sitemap field set. Under "Inclusion" change "Excluded" to
   become "Included". Save.
6. To include a single vocabulary term in the sitemap, select edit vocabulary.
   Select the vocabulary term to be included. Select the XML sitemap field set.
   Under "Inclusion" change "Excluded" to become "Included". Save.
7. Once that is all complete, go to Configurations > Search and Metadata > XML
   Sitemap.
8. Select the **Rebuild Links** tab in the upper right.
9. Select "Rebuild sitemap" even if the message says that you do not need to.
10. Now you're taken back to the configuration page which shows you the link to
   your XML sitemap which you can click and confirm that pages have been added.

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
  [Renato Gonçalves](https://www.drupal.org/u/RenatoG),
  [Juampy NR](https://www.drupal.org/u/juampynr), and
  [Tasya Rukmana](https://www.drupal.org/u/tadityar)

## License

This project is GPL v2 software. See the LICENSE.txt file in the root directory
for complete text.
