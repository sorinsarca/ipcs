<?php 
	$exec = array(
		'client' => '../../client/ipcs_client', //path to client
		'cache' => @$_GET['cache'],
		'daemon' => 'ajaxtest',
		'function' => @$_GET['func'] 
	);
	if (@$_GET['params'] && is_array($_GET['params'])) {
		$exec['params'] = base64_encode(serialize($_GET['params']));
	}
	$exec = '"' . implode('" "', $exec) . '"';
	system($exec);
?>