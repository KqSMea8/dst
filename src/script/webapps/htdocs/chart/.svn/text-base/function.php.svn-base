<?php
// Include the library
require_once('./src/highcharts.php');
// prepare data
require_once("../conf/config.php");

function gen_chart($container, $xAxis, $yAxis)
{
    $oHighcharts = new Highcharts(new HighchartsChart($container, HighchartsChart::SERIES_TYPE_LINE));
    $oHighcharts->title = new HighchartsTitle($container);
    $oHighcharts->xAxis->categories = $xAxis;

    $oHighcharts->tooltip->formatter = "function() {
        return this.series.name +': '+ this.y +'';
    }";

    foreach ($yAxis as $peid => $qps) {
        $oHighcharts->series->addSerie(new HighchartsSerie($peid, $qps));
    }
    echo $oHighcharts->render();
    echo "<div id='$container' style='width: 1280px; height: 768px;'> </div>";
}
