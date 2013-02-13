<?php 
	$key = @$_GET['key'];
	$data = @$_GET['data'];
?>
<form method="get" action="">
	Key: <input type="text" name="key" value="<?php echo $key;?>" />
	Data: <input type="text" name="data" value="<?php echo $data;?>" />
	Action:
	<select name="action">
		<option value="set">Set data to key</option>
		<option value="get">Get data from key</option>
		<option value="del">Remove data from key</option>
	</select>
	<input type="submit" value="Perform" />
</form>
<hr />
<pre>
<?php 
	if (!$key) die('Please use a key!');
	switch (@$_GET['action']) {
		case 'set':
			if(ipcs_set($key, $data)) {
				echo $data, ' saved to ', $key;
			}
			else {
				echo 'Could not save to ', $key;
			}
			break;
		case 'get':
			if (($val = ipcs_get($key)) !== FALSE) {
				echo 'Value of ', $key, ":\n", print_r($val, TRUE);
			}
			else {
				echo 'Key ', $key, ' has no stored data!';
			}
			break;
		case 'del':
			if (ipcs_del($key)) {
				echo $key, ' deleted!';
			}
			else {
				echo 'Could not delete ', $key, '!';
			}
			break;
	}
?>
</pre>