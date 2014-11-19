        <!-- pages -->
        <div class="col-sm-8 text-center">
          <h3>Pages</h3>
          <p>Select a page among the available for the selected day, ordered from older to newer.</p>
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
