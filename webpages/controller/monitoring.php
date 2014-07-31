<?php
    if (isset($_GET['start'])) {
        exec('sudo killall -USR1 antifurto 2>$1', $out, $retvar);
    }
    elseif (isset($_GET['stop'])) {
        exec('sudo killall -USR2 antifurto 2>&1', $out, $retvar);
    }
    else {
        $retvar = -1;
    }

    echo "Result $retvar.";
    $dump = var_dump($out);
    echo "$dump";
?>
