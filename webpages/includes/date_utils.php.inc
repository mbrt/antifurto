<?php
  // today globals
  $today_dd = date("d", time());
  $today_mm = date("m", time());
  $today_yyyy = date("Y", time());
  $today = "$today_yyyy$today_mm$today_dd";
  $today_dir = "$today_yyyy-$today_mm-$today_dd";
  $today_dt = new DateTime();
?>
