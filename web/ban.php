<?php
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $steamid = $_POST['steamid'];
    $devicename = $_POST['devicename'];
    $file = 'banned/usersbanned.txt';
    file_put_contents($file, "$steamid\n", FILE_APPEND);
}
?>
