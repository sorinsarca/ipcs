IPCS
===

What is IPCS?
---

IPCS is PHP extension that allows you to create daemons (daemon = background process that is  processing client requests).
Also IPCS has functions for caching, shared memory or semaphores.

The goal of this project is to call any Drupal (or other framework) function without bootstrap.

Using IPCS
---

1. Create ipcs extension.

    Extension is located in `php_ipcs` folder, so cd to `php_ipcs` and run:

		phpize
		./configure --enable-ipcs
		make

	Your extension will be in php_ipcs/modules, with name ipcs.so. You must now add it to your php.ini files (for cli and server):
	>extension=/path/to/ipcs.so
    
	Test if extension is loaded:
    
    ```php
	var_dump(function_exists('ipcs_daemon'));
    ```

2. Create the client

	Client is located in client/ipcs_client.c, cd to client and run:
	
    `gcc ipcs_client.c -o ipcs_client -lrt`
    
	The executable should be ipcs_client.

3. Test ipcs functions

	You can test using the examples from example folder: simple.php, cache.php and semaphore.php.

4. Test the daemon

	To test the daemon use the example located in ajax folder. Steps:
	1. Create a new folder named ipcs in localhost root (/var/www for apache, /srv/http for nginx, /?/? yours). For this example we are assuming that you copied in folder /var/www/ipcs.

	2. Open a new tab and navigate to http://localhost/ipcs/client/manager.php which will show up a page where you can start or kill daemons.

	3. Start a new daemon named ajaxtest. The path to daemon script is /var/www/ipcs/example/ajax/daemon.php.

	4. Open a new tab and navigate to http://localhost/ipcs/example/ajax/index.html.

	5. Test the daemon.

	6. Modify the daemon, but remember to kill and restart the daemon after any change to daemon.php file.

Soon will be added examples for boosting Drupal performance with Nginx SSI and IPCS.
