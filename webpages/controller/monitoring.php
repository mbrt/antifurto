<?php
    if (isset($_GET['start'])) {
        exec('sudo killall -USR1 antifurto 2>&1', $out, $retvar);
    }
    elseif (isset($_GET['stop'])) {
        exec('sudo killall -USR2 antifurto 2>&1', $out, $retvar);
    }
    else {
        $retvar = -1;
    }

    // sleep(2); // debug purpose
    $log = sizeof($out) > 0 ? $out[0] : "";
    echo json_encode(array('result' => $retvar, 'log' => $log));
?>
