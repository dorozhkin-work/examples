Description
-----------
This module provides a commerce payment integration with Expay payment service.

Installation
------------
1. Extract the tar ball that you downloaded from Drupal.org.

2. Upload the entire directory and all its contents to your modules directory.
   For example: sites/all/modules or sites/all/modules/contrib.

Configuration
-------------
To enable and configure this module do the following:

1. Login in Drupal as with administrator role.

2. Go to Admin -> Modules, find Commerce Expay module and enable it.

3. Go to Admin -> Configuration -> Payment -> Expay settings, and fill
   all necessary field and save.
   You can use "Callback URL", "URL as succeess_url", "URL as waiting_url" and
   "URL as reject_url" in you merchant account. Just copy them and save in
   Expay account settings.

3. Go to Store -> Configuration -> Payment methods, and  configure commerce
   payment methods dispay. Note: list of Expay payment methods will be cached.
   To update method list you need to clear cache.

Note: by default all Expay payment methods will be enabled after installing
enabling this module.

