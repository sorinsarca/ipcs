<?php 
if (!defined('STDOUT')) exit;
$prefix = (string) @$argv[1];
$id =  (int) @$argv[2];
if (!$prefix || $id < 0) die('Invalid params');

define('PRE', $prefix . '[' . $id . ']: ');

function ajax_request_sum() {
	return PRE . array_sum(func_get_args());
}

function ajax_request_static() {
	static $i = 0;
	return PRE . (++$i);
}

function ajax_request_class_date() {
	return PRE . AjaxTest::date();
}

function ajax_request_repeat($str, $times) {
	return PRE . str_repeat($str, $times);
}

function ajax_request_cache($key, $str, $times = 1) {
	$str = date('h:i:s: ') . str_repeat($str, $times);
	ipcs_cache_set($key, $str, 5);
	return PRE . $str; 
}

class AjaxTest {
	public static function date() {
		return date('h:i:s');
	} 
}

// create daemon
ipcs_daemon($prefix, $id);

?>
