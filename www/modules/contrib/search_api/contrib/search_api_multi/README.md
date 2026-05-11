# Search API: Multi-index searches

This module allows you to create search queries on multiple indexes that lie on
the same server. The only thing you'll need is a search service class that
supports the "search_api_multi" feature. Currently, only the "Solr search"
supports this.

Information for users
---------------------

Enable the Search views (search_api_views) module along with this one to make
instant use of the multi-index searching facilities. You'll get a new base table
in Views for each server supporting the "search_api_multi" feature.
You can then add filters, arguments, fields and sorts (although the last one
might work rather poorly, depending on the sorted field and the implementation)
from all enabled indexes on this server.

- Issues

If you find any bugs or shortcomings while using this module, please file an
issue in the project's issue queue

Information for developers
--------------------------

If you are the developer of a SearchApiServiceInterface implementation and want
to support searches on multiple indexes with your service class, too, you'll
have to support the "search_api_multi" feature by implementing the
SearchApiMultiServiceInterface interface documented in
search_api_multi.service.inc.
