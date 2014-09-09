<?php
    exec('sudo killall -37 antifurto 2>&1', $out, $retvar);
    // write json result
    echo json_encode(array('result' => $retvar, 'log' => $out[0]));
?>
