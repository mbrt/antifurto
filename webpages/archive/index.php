<?php
  // settings
  $pics_path = 'pics';
  $pics_per_page = 16;
  $pics_per_row = 4;
  $pages_in_nav = 5;

  // helper functions
  function get_or_default(&$var, $default = null) {
    return isset($var) ? $var : $default;
  }

  // input: yyyymmdd
  // output: (yyyy, mm, dd)
  function split_compact_date($from) {
    $yyyy = substr($from, 0, 4);
    $mm = substr($from, 4, 2);
    $dd = substr($from, 6, 2);
    return array($yyyy, $mm, $dd);
  }

  // input: yyyy?mm?dd
  // output: yyyymmdd
  function compact_date($date) {
    $yyyy = substr($date, 0, 4);
    $mm = substr($date, 5, 2);
    $dd = substr($date, 8, 2);
    return "$yyyy$mm$dd";
  }

  // input: yyyy?mm?dd
  // output: dd/mm/yyyy
  function format_day_dir($dir) {
    $a = split_compact_date(compact_date($dir));
    return $a[2] .'/'. $a[1] .'/'. $a[0];
  }

  // input: yyyymmdd
  // output: yyyy-mm-dd
  function from_compact_to_dir($compd) {
    $a = split_compact_date($compd);
    return $a[0] .'-'. $a[1] .'-'. $a[2];
  }

  function from_pic_file_to_time($file) {
    $hh = substr($file, 11, 2);
    $mm = substr($file, 14, 2);
    $ss = substr($file, 17, 2);
    $xxxx = substr($file, 20, 4);
    return "$hh:$mm:$ss.$xxxx";
  }

  // count files in directory
  function count_files($dir) {
    $count = 0;
    if (file_exists($dir))  {
      $handle = opendir($dir);
      while (false !== ($entry = readdir($handle)))
        if ($entry != '.' && $entry != '..')
          $count++;
    }
    return $count;
  }

  // globals
	$today_dd = date("d", time());
	$today_mm = date("m", time());
	$today_yyyy = date("Y", time());
	$today = "$today_yyyy$today_mm$today_dd";
	$today_dir = "$today_yyyy-$today_mm-$today_dd";

  // parameters
  $page = get_or_default($_GET['page'], 1);
  $sel_date = get_or_default($_GET['day'], $today);
  if (strlen($sel_date) < 8)
    $sel_date = $today;

  $sel_year = substr($sel_date, 0, 4);
  $sel_month = substr($sel_date, 4, 2);
  $sel_day = substr($sel_date, 6, 2);

  // collect days from newer to older
  $days = array();
  if (file_exists($pics_path)) {
    $pics_handle = opendir($pics_path);
    while (false !== ($entry = readdir($pics_handle)))
      if ($entry != '.' && $entry != '..')
        array_push($days, $entry);
    rsort($days);
  }

  // collect pictures of selected day from older to newer
  $curr_day_dir = "$pics_path/". from_compact_to_dir($sel_date);
  $all_pics = array();
  if (file_exists($curr_day_dir)) {
    $pics_handle = opendir($curr_day_dir);
    while (false !== ($entry = readdir($pics_handle)))
      if ($entry != '.' && $entry != '..')
        array_push($all_pics, $entry);
    sort($all_pics);
  }

  // compute pages to display
  $num_pages = ceil(sizeof($all_pics) / $pics_per_page);
  if ($page > $num_pages || $page < 1)
    $page = 1;
  $first_page = intval(($page - 1) / $pages_in_nav) * $pages_in_nav + 1;
  $last_page = min($num_pages + 1, $first_page + $pages_in_nav);

  // compute pictures to display
  $first_pic_index = ($page - 1) * $pics_per_page;
  $last_pic_index = min($first_pic_index + $pics_per_page, sizeof($all_pics));
  $num_pics_in_page = $last_pic_index - $first_pic_index;

  // DEBUG
  // echo "DAYS\n";
  // var_dump($days);
  // echo "ALL PICS\n";
  // var_dump($all_pics);
?>

<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <meta name="description" content="Picture archive for antifurto">
    <meta name="author" content="Michele Bertasi">
    <!-- link rel="icon" href="../../favicon.ico"-->

    <title>Antifurto archive</title>

    <!-- Bootstrap -->
    <link href="../libs/bootstrap/css/bootstrap.min.css" rel="stylesheet">
    <!-- Custom styles taken from template -->
    <link href="../assets/css/theme.css" rel="stylesheet">

    <!-- HTML5 Shim and Respond.js IE8 support of HTML5 elements and media queries -->
    <!-- WARNING: Respond.js doesn't work if you view the page via file:// -->
    <!--[if lt IE 9]>
      <script src="https://oss.maxcdn.com/html5shiv/3.7.2/html5shiv.min.js"></script>
      <script src="https://oss.maxcdn.com/respond/1.4.2/respond.min.js"></script>
    <![endif]-->
  </head>

  <body role="document">
    
    <!-- Fixed navbar -->
    <div class="navbar navbar-default navbar-fixed-top" role="navigation">
      <div class="container">
        <div class="navbar-header">
          <button type="button" class="navbar-toggle collapsed" data-toggle="collapse" data-target=".navbar-collapse">
            <span class="sr-only">Toggle navigation</span>
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
          </button>
          <a class="navbar-brand" href="..">Antifurto</a>
        </div>
        <div class="navbar-collapse collapse">
          <ul class="nav navbar-nav">
            <li><a href="..">Home</a></li>
            <li><a href="../live/">Live view</a></li>
            <li class="active"><a href="../archive/">Archive</a></li>
          </ul>
        </div><!--/.nav-collapse -->
      </div>
    </div>

    <div class="container theme-showcase" role="main">

      <!-- Main jumbotron for a primary marketing message or call to action -->
      <div class="jumbotron">
        <h1>Antifurto archive</h1>
        <p>View the recording archive of triggered alarms.</p>
      </div>

      <!-- days and pages navigation -->
      <div class="row">
        <!-- days -->
        <div class="col-sm-4 text-center">
          <div class="btn-group days">
            <button type="button" class="btn btn-default dropdown-toggle" data-toggle="dropdown">
              <?php
                if ($sel_date == $today)
                  echo 'Today';
                else
                  echo "$sel_day/$sel_month/$sel_year";
                echo ' <span class="caret"></span>';
              ?>
            </button>
            <ul class="dropdown-menu" role="menu">
              <?php
                function gen_day_li_link($day_dir, $day_display, $link) {
                  global $pics_path;
                  $num_pics = count_files("$pics_path/$day_dir");
                  echo '<li><a href="'. $link .'">';
                  if ($num_pics > 0)
                    echo '<span class="badge pull-right">'. $num_pics .'</span>';
                  echo "$day_display</a></li>";
                }
                function gen_day_li($day_dir, $day_display) {
                  gen_day_li_link($day_dir, $day_display, '?day='. compact_date($day_dir));
                }

                gen_day_li_link($today_dir, 'Today', '?');
                if (sizeof($days) > 1 || (sizeof($days) == 1 && $days[0] != $today_dir))
                  echo '<li class="divider"></li>';
                if (sizeof($days) > 0 && $days[0] != $today_dir)
                  gen_day_li($days[0], format_day_dir($days[0]));
                for ($i = 1; $i < sizeof($days); $i++)
                  gen_day_li($days[$i], format_day_dir($days[$i]));
                // example
                // <button type="button" class="btn btn-default dropdown-toggle" data-toggle="dropdown">
                //   Today <span class="caret"></span>      // display the selected day
                // </button>
                // <ul class="dropdown-menu" role="menu">
                //   <li><a href="#">Today</a></li>         // always displayed
                //   <li class="divider"></li>              // only if more than one day
                //   <li><a href="#">09/09/2014</a></li>
                //   <li><a href="#">15/08/2014</a></li>
                //   <li><a href="#">14/08/2014</a></li>
                // </ul>
              ?>
            </ul>
          </div>
        </div>

        <!-- pages -->
        <div class="col-sm-8 text-center">
          <ul class="pagination archive-pages">
            <?php
              echo '<li';
              if ($page == 1)
                echo ' class="disabled"><a>';
              else
                echo '><a href="?day='. $sel_date .'&page='. ($page - 1) .'">';
              echo '<span class="glyphicon glyphicon-chevron-left"></span><span class="hidden-xs"> Older</span></a></li>';
              for ($i = $first_page; $i < $last_page; $i++) {
                echo '<li';
                if ($i == $page)
                  echo ' class="active"';
                echo '><a href=?day='. $sel_date .'&page='. $i .'>'. $i .'</a></li>';
              }
              echo '<li';
              if ($page == $num_pages)
                echo ' class="disabled"><a>';
              else
                echo '><a href="?day='. $sel_date .'&page='. ($page + 1) .'">';
              echo '<span class="hidden-xs">Newer </span><span class="glyphicon glyphicon-chevron-right"></span></a></li>';
              // example:
              // <li class="disabled"><a href="#"><span class="glyphicon glyphicon-chevron-left"></span><span class="hidden-xs"> Older</span></a></li>
              // <li class="active"><a href="#">1</a></li>
              // <li><a href="#">2</a></li>
              // <li><a href="#">3</a></li>
              // <li><a href="#">4</a></li>
              // <li><a href="#">5</a></li>
              // <li><a href="#"><span class="hidden-xs">Newer </span><span class="glyphicon glyphicon-chevron-right"></span></a></li>
            ?>
          </ul>
        </div>
      </div>

      <!-- Nav tabs -->
      <ul id="nav-tab" class="nav nav-tabs" role="tablist">
        <li class="active"><a href="#thumbnails-tab" role="tab" data-toggle="tab">Thumbnails</a></li>
        <li><a href="#carousel-tab" role="tab" data-toggle="tab">Carousel</a></li>
      </ul>
      
      <!-- Tab panes -->
      <div class="tab-content">
        <div class="tab-pane active" id="thumbnails-tab">
        
          <!-- thumbnails panel -->
          <?php
            for ($i = $first_pic_index, $data_index = 0; $i < $last_pic_index; $i++, $data_index++) {
              if ($i % $pics_per_row == 0)
                echo '<div class="row archive-thumb-grid">';
              $caption = from_pic_file_to_time($all_pics[$i]);
              echo '<div class="col-xs-6 col-sm-3"><div class="thumbnail">'
                  .'<img class="img-responsive" data-index="'. $data_index
                  .'" src="'. "$curr_day_dir/". $all_pics[$i] .'" alt="'. $caption .'"/>'
                  .'<div class="caption">'. $caption .'</div></div></div>';
              if ($i % $pics_per_row == $pics_per_row - 1 || $i == $last_pic_index - 1)
                echo '</div>';
            }
            // example
            // <div class="row archive-thumb-grid">
            //   <div class="col-xs-6 col-sm-3">
            //     <div class="thumbnail">
            //       <img class="img-responsive" data-index="0" src="pics/2014-09-09/2014-09-09_07:04:30.0536.jpg" alt="07:04:30.0536"/>
            //       <div class="caption">07:04:30.0536</div>
            //     </div>
            //   </div>
            //   ...
            // </div>
          ?>
        </div>

        <div class="tab-pane" id="carousel-tab">
          <!-- carousel panel -->

          <div id="carousel-archive" class="carousel slide" data-ride="carousel">
            <!-- Indicators -->
            <ol class="carousel-indicators">
              <?php
                for ($i = 0; $i < $num_pics_in_page; $i++) {
                  echo '<li data-target="#carousel-archive" data-slide-to="'. $i .'"';
                  if ($i == 0)
                    echo ' class="active"';
                  echo '></li>';
                }
                // example
                // <li data-target="#carousel-archive" data-slide-to="0" class="active"></li>
                // <li data-target="#carousel-archive" data-slide-to="1"></li>
              ?>
            </ol>
          
            <!-- Wrapper for slides -->
            <div class="carousel-inner">
              <?php
                for ($i = $first_pic_index, $data_index = 0; $i < $last_pic_index; $i++, $data_index++) {
                  if ($data_index == 0)
                    echo '<div class="item active">';
                  else
                    echo '<div class="item">';
                  echo '<img src="'. "$curr_day_dir/". $all_pics[$i]
                      .'" alt="'. from_pic_file_to_time($all_pics[$i])
                      .'"/></div>';
                }
                // example
                // <div class="item active">
                //   <img src="pics/2014-09-09/2014-09-09_07:04:30.0536.jpg" alt="07:04:30.0536"/>
                // </div>
                // <div class="item">
                //   <img src="pics/2014-09-09/2014-09-09_07:04:30.0536.jpg" alt="07:04:30.0536"/>
                // </div>
              ?>
            </div>
          
            <!-- Controls -->
            <a class="left carousel-control" href="#carousel-archive" role="button" data-slide="prev">
              <span class="glyphicon glyphicon-chevron-left"></span>
            </a>
            <a class="right carousel-control" href="#carousel-archive" role="button" data-slide="next">
              <span class="glyphicon glyphicon-chevron-right"></span>
            </a>
          </div>

        </div>
      </div>

    </div> <!-- /container -->


    <!-- Bootstrap core JavaScript
    ================================================== -->
    <!-- Placed at the end of the document so the pages load faster -->
    <script src="../libs/jquery/jquery.min.js"></script>
    <!-- Include all compiled plugins (below), or include individual files as needed -->
    <script src="../libs/bootstrap/js/bootstrap.min.js"></script>
    <script src="../assets/js/docs.min.js"></script>
    <!-- IE10 viewport hack for Surface/desktop Windows 8 bug -->
    <script src="../assets/js/ie10-viewport-bug-workaround.js"></script>
    <!-- for goTo -->
    <script src="../assets/js/goto.js"></script>

    <!-- Page custom JavaScript -->
    <script type="text/javascript">
        $(".thumbnail img").click(function() {
            $('#nav-tab a[href="#carousel-tab"]').tab('show');
            $('#carousel-archive').carousel(parseInt($(this).attr('data-index')))
                                  .goTo();
        });
    </script>

  </body>
</html>
