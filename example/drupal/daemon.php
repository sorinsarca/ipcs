<?php
if (!defined('STDOUT')) exit;

define('IPCS', TRUE);

$_SERVER['REMOTE_ADDR'] = '127.0.0.1';

// Drupal bootstrap.
define('DRUPAL_ROOT', getcwd());
require_once DRUPAL_ROOT . '/includes/bootstrap.inc';
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);
// End bootstrap.

$prefix = @$argv[1];
$id = (int) @$argv[2];
if (!$prefix || $id < 0) die('Invalid params');

// Set base url.
$base_url = 'http://localhost/drupal7';
// Set base path.
$base_path = '/';

// Start daemon.
ipcs_daemon($prefix, $id);