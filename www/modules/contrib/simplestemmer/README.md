# Simple Stemmer for Core Search

Provides partial word matching for entities in the core Search module, by
further processing of keywords and the text added to the search index.

Partial word matching is implemented by using ngrams. Each word is split into 3
(default) letter lengths, so 'apple' gets indexed with 3 smaller strings 'app',
'ppl', 'ple'. The effect of this is that as long as your search matches X
percentage of the word the entity will be pulled up in the results.

This will increase the size of the `search_index` table, which is a tradeoff for
being able to find more partial words.

This can be used in conjunction with other modules that preprocess keywords and
the text added to the search index. For example, it can be used with the Porter
Stemmer module, which may help find content with a wider range of search terms.
However, thorough testing is recommended with a variety of search terms to make
sure it functions as desired.

## Installation

- Install this module using the official Backdrop CMS instructions at
  <https://backdropcms.org/guide/modules>.
- Requires core Search module to be enabled.

## Configuration

- Go to the core Search settings page `admin/config/search/settings`.
- The "Minimum word length to index" is used for the N-gram size. Minimum word
  length is set to 3 by default. This can be overridden by setting
  `simplestemmer_ngram_size` in `settings.php` to a whole number. E.g.:
  `$settings['simplestemmer_ngram_size'] = '5';`
- Then click the button for "Rebuild Search Index" and re-index the entire site.

## Current Maintainers

- [Herb v/d Dool](https://github.com/herbdool)
- Seeking additional maintainers

## Credits

Created for Backdrop CMS by Herb v/d Dool. Inspired by Scott Reynolds' work
on https://www.drupal.org/node/103548 and the Fuzzy Search module for Search API.

## License

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.
