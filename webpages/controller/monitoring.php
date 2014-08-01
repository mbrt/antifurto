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

    // write json result
    echo '{ "result": ' . $retvar . ', "log": "' . $out[0] . '" }';
    //echo "Result $retvar.<br>";
    //echo var_dump($out);
?>
