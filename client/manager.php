<!DOCTYPE html>
<html>
<head>
    <title>IPCS manager</title>
</head>
<body>
<form method="get" action="">
    Daemon prefix: <input type="text" name="p" value="" />
    Number of daemons: <input type="text" name="n" value="1" />
    Daemon script: <input type="text" name="s" value="" />
    <input type="submit" name="submit" value="create" />
</form>
<hr />
<?php

define('IPCS_MANAGER_CURRENT', 'IPCS_MANAGER_CURRENT');

$current = ipcs_get(IPCS_MANAGER_CURRENT);
if ($current) {
    $current = unserialize($current);
}
else {
    $current = array();
}

$redir = true;
switch (@$_GET['submit']) {
    case 'create':
        $prefix = @$_GET['p'];
        if (!$prefix || strlen($prefix) > 10) die('Invalid prefix');
        $number = (int) @$_GET['n'];
        if ($number < 1) die('Invalid number!');
        if (isset($current[$prefix])) die('Daemon exists!');
        $script = @$_GET['s'];
        if (!$script || !file_exists($script)) die('Invalid daemon script!');
        ipcs_seti($prefix . '_t', $number);
        ipcs_seti($prefix . '_c', 0);
        $path = explode('/', $script);
        array_pop($path);
        $path = implode('/', $path);
        for ($i = 0; $i < $number; $i++) {
            $current[$prefix][] = shell_exec('cd ' . $path . ' && php  ' . $script . ' ' . $prefix . ' ' . $i . ' > /dev/null 2>&1 & echo $!');
        }
        ipcs_set(IPCS_MANAGER_CURRENT, serialize($current));
        break;
    case 'kill':
        $prefix = @$_GET['p'];
        if (!$prefix || !isset($current[$prefix])) die('Daemon not exists!');
        $total = count($current[$prefix]);
        for ($i = 0; $i < $total; $i++) {
            shell_exec('kill ' . $current[$prefix][$i]);
            ipcs_del($prefix . '_d_' . $i);
            ipcs_semrm($prefix . '_a_' . $i);
            ipcs_semrm($prefix . '_d_' . $i);
            ipcs_semrm($prefix . '_c_' . $i);
        }
        ipcs_del($prefix . '_t');
        ipcs_del($prefix . '_c');
        unset($current[$prefix]);
        ipcs_set(IPCS_MANAGER_CURRENT, serialize($current));
        break;
    default:
        $redir = false;
        break;
}

if ($redir) {
    header('Location: manager.php');
    exit;
}

if ($current) {
    echo '<ul>';    
    foreach ($current as $prefix => $pids) {
        echo '<li><b>', $prefix, '</b> - <a href="?submit=kill&p=', $prefix, '">Kill daemon</a>', '<ul>';
        echo '<li>', implode('</li><li>', $pids), '</li>';
        echo '</ul></li>';
    }
    echo '</ul>';
}

?>
</body>
</html>