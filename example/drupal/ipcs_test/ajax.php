<?php

include 'ipcs.php';

switch (@$_GET['action']) {
    case 'get_nodes':
        $start = (int) @$_GET['start'];
        if ($start < 0) $start = 0;
        $count = (int) @$_GET['count'];
        if ($count < 1) $count = 1;
        
        system(ipcs_format(IPCS_DAEMON, 'ipcs_test_get_nodes', array($count, $start, TRUE), NULL));
        
        break;
}

function ipcs_format($prefix, $func = NULL, $params = NULL, $cache = 0) {
	if ($params !== NULL) {
		$params = base64_encode(serialize($params));
	}
	return IPCS_CLIENT . " \"{$cache}\" \"{$prefix}\" \"{$func}\" \"{$params}\"";
}

exit;

