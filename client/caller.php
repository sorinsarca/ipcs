<?php
$client = './ipcs_client'; //path to client executable
system($client . ' ' . trim(urldecode($_SERVER['QUERY_STRING'])));
// QUERY_STRING format: cache_key daemon_prefix function params_serialized_base64