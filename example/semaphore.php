<?php 
	$sem_key = 'test_sem';
	
	// create semaphore if not exists
	if(!ipcs_sem($sem_key, 0, TRUE)) die('Could not create semaphore ' . $sem_key); 

	switch(@$_GET['action']) {
		case 'wait':
			ipcs_wait($sem_key);
			echo 'Wait done! Sem value: ' . ipcs_semval($sem_key);
			break;
		case 'signal':
			ipcs_signal($sem_key);
			echo 'Signal done! Sem value: ' . ipcs_semval($sem_key);
			break;
		case 'remove':
			ipcs_semrm($sem_key);
			echo 'Semaphore removed!';
			break;
		default:
			echo 'These links will open in new tabs. Waiting pages will wait until you hit signal for each one!<br/>';
			echo '<a href="?action=wait" target="_blank">Wait</a> | <a href="?action=signal" target="_blank">Signal</a> | <a href="?action=remove" target="_blank">Remove</a>';
			break;
	}
?>