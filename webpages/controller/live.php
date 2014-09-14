<?php
    exec('sudo killall -37 antifurto 2>&1', $out, $retvar);
    // write json result
    $log = sizeof($out) > 0 ? $out[0] : "";
    echo json_encode(array('result' => $retvar, 'log' => $log));
?>
