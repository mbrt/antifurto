<?php
  include 'includes/date_utils.php.inc';

  // settings
  $pics_path = 'archive/pics';   

  function find_max_in_dir($dir) {
    if (!file_exists($dir)) return null;
    $handle = opendir($dir);
    $max_val = null;
    while (false !== ($entry = readdir($handle)))
      if ($entry != '.' && $entry != '..')
        $max_val = max($entry, $max_val);
    return $max_val;
  }

  // input: YYYY-MM-DD_hh:mm:ss.xxxx.jpg
  // output: a DateTime representing input
  function from_pic_file_to_datetime($file) {
    return new DateTime(substr($file, 0, 10) .' '. substr($file, 11, 8));
  }


  // main code
  // find last day in pics
  $last_day = find_max_in_dir($pics_path);
  if ($last_day === null)
    // no pics found
    $last_alarm = 'none';
  else {
    if ($last_day != $today_dir) {
      // was not today: print how many days ago
      $last_day_dt = new DateTime($last_day);
      $interval = $last_day_dt->diff($today_dt);
      $last_alarm = $interval->days .' days ago';
    }
    else {
      // was today: print hours, minutes or just now
      $last_pic = find_max_in_dir("$pics_path/$last_day");
      if ($last_pic === null)
        // no pictures, show today only
        $last_alarm = 'today';
      else {
        // compute diff from now
        $last_pic_dt = from_pic_file_to_datetime($last_pic);
        $interval = $last_pic_dt->diff($today_dt);
        if ($interval->h > 0)
          // hours ago
          $last_alarm = $interval->h .' hours ago';
        else if ($interval->i > 4)
          // minutes ago
          $last_alarm = $interval->i .' minutes ago';
        else
          // less than 5 minutes ago
          $last_alarm = 'just now';
      }
    }
  }
?>

<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <meta name="description" content="Main page for antifurto controller">
    <meta name="author" content="Michele Bertasi">
    <!-- link rel="icon" href="../../favicon.ico"-->

    <title>Antifurto</title>

    <!-- Bootstrap -->
    <link href="libs/bootstrap/css/bootstrap.min.css" rel="stylesheet">
    <!-- Custom styles taken from template -->
    <link href="assets/css/theme.css" rel="stylesheet">

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
            <li class="active"><a href="..">Home</a></li>
            <li><a href="../live/">Live view</a></li>
            <li><a href="../archive/">Archive</a></li>
          </ul>
        </div><!--/.nav-collapse -->
      </div>
    </div>

    <div class="container theme-showcase" role="main">

      <!-- Main jumbotron for a primary marketing message or call to action -->
      <div class="jumbotron">
        <h1>Antifurto home</h1>
          <p>From here you can enable and disable the monitoring.</p>
          <p>
            <div>Monitoring status: <b>active?</b></div>
            <div>Last alarm: <b><?php echo $last_alarm ?></b></div>
          </p>
      </div>

      <!-- contents -->
      <div class="row on-off">
        <div class="col-sm-4 col-sm-offset-2">
          <button type="button" data-loading-text="Starting..." class="btn btn-block btn-lg btn-primary start">
            Start monitoring
          </button>
        </div>
        <div class="col-sm-4">
          <button type="button" data-loading-text="Stopping..." class="btn btn-block btn-lg btn-primary stop">
            Stop monitoring
          </button>
        </div>
        <div id="message" class="col-sm-12 alert-msg"></div>
      </div>

    </div> <!-- /container -->



    <!-- Bootstrap core JavaScript
    ================================================== -->
    <!-- Placed at the end of the document so the pages load faster -->
    <script src="libs/jquery/jquery.min.js"></script>
    <!-- Include all compiled plugins (below), or include individual files as needed -->
    <script src="libs/bootstrap/js/bootstrap.min.js"></script>
    <script src="assets/js/docs.min.js"></script>
    <!-- IE10 viewport hack for Surface/desktop Windows 8 bug -->
    <script src="assets/js/ie10-viewport-bug-workaround.js"></script>
    <!-- displayMessage utility -->
    <script src="assets/js/message.js"></script>

    <!-- Page custom JavaScript -->
    <script type="text/javascript">
        function startStopPressed(target, other, msg, params) {
            target.button('loading');
            other.attr('disabled', 'disabled');
            $.ajax({
                    url: 'controller/monitoring.php',
                    data: params,
                    dataType: 'json',
                    cache: false
                })
                .done(function(data) {
                    // data is { result: int, log: string }
                    if (data.result == 0)
                        displayMessage('#message',
                            '<h4>Success</h4><p>' + msg + '</p>',
                            'alert-success');
                    else
                        displayMessage('#message',
                            '<h4>Error</h4><p>' + data.log + '.</p>',
                            'alert-danger');
                })
                .fail(function(jqxhr, textStatus, errorThrown) {
                    displayMessage('#message',
                        '<h4>Error</h4>' +
                        '<p>' + errorThrown + '.</p>',
                        'alert-danger');
                })
                .always(function() {
                    target.button('reset');
                    other.removeAttr('disabled');
                });
        }
        
        $('.on-off .start').click(function() {
            startStopPressed($('.on-off .start'), $('.on-off .stop'),
                'Monitoring started.', { start: 1 });
        });
        $('.on-off .stop').click(function() {
            startStopPressed($('.on-off .stop'), $('.on-off .start'),
                'Monitoring stopped.', { stop: 1 });
        });
    </script>

  </body>
</html>
