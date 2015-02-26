Redirect
======================
This is the new module home for a unified redirection API (also replaces
path_redirect and globalredirect).

* Initial Backdrop Port

there are some flags in the code in areas where I believe some action is required,
but I did not know the correct action.

those areas are flagged: 
* TODO
* BACKDROP_INITIAL_PORT

Dependencies
* locale
* field_ui

i.  Redirect module allows you to create 301 redirects inside the Backdrop GUI.
  * for example you can tell user request to the path /old-path to return /new-path to the user.
  
ii.  To install the redirect module:
  * download the source code from: <todo:URL> to your backdrop_root/modules/contrib
  * visit /admin/modules
  * search for 'redirect'
  * click enable and save
  ** When adding content to your site you will see a Redirect Vertical tab
    *** you can click this tab to add a redirect to the content.
  * You can also add redirects at: /admin/config/search/redirect
  
iii. Licensed under GPL v2
    * See the LICENSE.txt file

iv.  No additional assets or licensing at this time.
     
v.  Maintainers:
  *  Geoff St. Pierre (serundeputy)
  
vi.  Past Maintainers:
  * Dave Reid
  
   
  
