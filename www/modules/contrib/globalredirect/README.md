Global Redirect
===============

GlobalRedirect is a simple module which redirects visitors from one page to another.

URL aliases provide nice clean URLs for the pages of a site. However Backdrop does not remove the old system path (eg node/1234). The problem is that you now have two URLs representing the same content. This is considdered "duplicate content" by search engines, and can decrease your pagerank.

Redirections:
- Checks the current URL for an alias and does a 301 redirect to it if it is not being used.
- Checks the current URL for a trailing slash, removes it if present and repeats check 1 with the new request.
- Checks if the current URL is the same as the site_frontpage and redirects to the frontpage if there is a match.
- Checks if the Clean URLs feature is enabled and then checks the current URL is being accessed using the clean method rather than the 'unclean' method.
- Checks access to the URL. If the user does not have access to the path, then no redirects are done. This helps avoid exposing private aliased node's.
- Make sure the case of the URL being accessed is the same as the one set by the author/administrator. For example, if you set the alias "articles/cake-making" to node/123, then the user can access the alias with any combination of case.
- Most of the above options are configurable in the settings page.

Current Maintainers
-------------------

- docwilmot (https://github.com/docwilmot)
- Nate Lampton (https://github.com/quicksketch)

Credits
-------

- Ported to Backdrop by [docwilmot](https://github.com/docwilmot)
- Maintained for Drupal by [Nicholas Thompson](https://www.drupal.org/u/nicholasthompson)

License
-------

This project is GPL v2 software. See the LICENSE.txt file in this directory for
complete text.
