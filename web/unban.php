<?php
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $steamid = $_POST['steamid'];
    $devicename = $_POST['devicename'];
    $file = 'banned/usersbanned.txt';
    $contents = file($file, FILE_IGNORE_NEW_LINES);
    $contents = array_filter($contents, function($line) use ($steamid) {
        return trim($line) !== $steamid;
    });
    file_put_contents($file, implode("\n", $contents) . "\n");
}
?>
