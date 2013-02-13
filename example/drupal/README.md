How to use Drupal ajax example
===

This example assumes that you already added ipcs.so extension to your php.ini files.

1. Create folder `drupal7` in the root of localhost
2. Download Drupal 7 to drupal7 folder
3. Install Drupal
4. Copy daemon.php file in drupal7 folder, near index.php
5. Copy `ipcs_client` executable in drupal7 folder and edit `ipcs.php` file from `ipcs_test` folder.
6. Add module `ipcs_test` to sites/all/modules and install it
7. Go to admin/structure/blocks and add to sidebar second the block `Display nodes using ipcs`
8. Add some articles in this site. You can use devel module to generate some dummy content.
9. Start a daemon named `drp7` which points to `daemon.php` script file from drupal7 folder
10. Refresh your page and click 'Older' button from ipcs block to load new content
11. Modify the `ipcs_test.module` file and set USING_DAEMON constant to 0 to compare response times between normal drupal ajax and ipcs ajax


By skipping bootstrap with ipcs, the response time is `~6ms` while the normal one is `~100ms`.

An example of using Drupal with IPCS and Nginx SSI to boost performance, will be added soon!
