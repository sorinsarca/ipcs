<?php 
	$key = 'cache1';
	$cache = ipcs_cache_get($key);
	if ($cache) {
		echo 'Cache hit!<br />';
		$cache = unserialize($cache);
		var_dump($cache);
	}
	else {
		echo 'Cache miss! Data is stored. Retry!';
		// save in memory some data
		$data = array(
			'date' => date('h:i:s'),
			'other' => 'other data',
		);
		ipcs_cache_set($key, serialize($data), 5); // 5 seconds
	}
	//Note: you can delete cache using ipcs_del($key);
?>