<?php
useattrib("graph.dataGraphattribute");
class extendedDataGraphAttribute extends dataGraphAttribute
{
  /**
   * Constructor.
   *
   * @param $name - The name of the graph (must be unique within the node).
   *
   * @param $callback - The name of the method to call on the node to
   *                    collect the data.
   *                    Example: if you specify "totals" as $callback, the
   *                    system invokes the method "graph_totals()" in the
   *                    node.
   *                    Callback prototype:
   *                    function graph_<callbackname>($params)
   *                    {
   *                    }
   *                    The callback must return an array of data in the
   *                    following format:
   *                    array('plotname1'=>array('x-axis/legend title'=>
   *                                                     'y-axis/pie value',
   *                                             ...),
   *                          'plotname2'=>...);
   *                    In other words, it must return one or more plots.
   *
   * @param $params - Specify an array of parameters to be passed to
   *                  the callback method. It is possible to specify
   *                  'templates' as values.
   *                  For example, specifying array("selected"=>"[id]" will
   *                  result in a callback invokation with a parameter
   *                  'selected' which has the id of the current record
   *                  as parameter. A special template is [pk], which
   *                  passes the entire primary key of the current record.
   *
   * @param $graphtype - The type of graph to plot. Currently supported are:
   *                     "pie" - Create a pie chart
   *                     "bar" - Create a bar chart
   * 					"accBar"- Create a accumulated bar chart
   *                     "line" - Create a line chart
   *                     "auto" - Creates a pie chart if only one plot is
   *                              passed and a bar chart if more than one
   *                              are passed.
   *
   * @param $flags - Any standard attribute (AF_*) flag can be passed.
   *                 As of yet, there are no specific flags for this
   *                 attribute.
   */
  function extendedDataGraphAttribute($name, $callback, $params=array(), $graphtype="auto", $flags=0)
  {
    $this->datagraphAttribute($name, $callback, $params, $graphtype, $flags);
  }

  function getSRC()
  {
    return  "./" .$this->_graphUrl($record);
  }

  function showGraph($record=array(), $addForm=true)
  {
    $session = atkSessionManager::getSession();

    $imgname = $this->fieldName();
    $res = '<img name="'.$imgname.'" src= "./' . $this->_graphUrl($record).'">'. $session['testing'];

    $params = $this->_parseParams($record);

    // $graph = new Graph()
    if (count($this->m_dynparams))
    {
      $res.="<script language=\"javascript\">\n";

      $decodedparams = $this->_parseParams($record);

      $paramelems = "";

      foreach ($decodedparams as $paramname=>$value)
      {
        $res.= $imgname."_".$paramname." = '".$value."';\n";
        $paramelems[]= $paramname."='+".$imgname."_".$paramname;
      }

      $res.=" function ".$imgname."_refresh()
                {
                  document.".$imgname.".src = '".$this->_baseUrl().implode("+'&", $paramelems).";
                }\n";

      $res.='</script>';
      if ($addForm) $res.='<form name="dummyform">';
      foreach ($this->m_dynparams as $paramname=>$possiblevalues)
      {
        $res.= '<br />'.atktext($paramname).": ";

        for ($i=0, $_i=count($possiblevalues); $i<$_i; $i++)
        {
          $checked = ($possiblevalues[$i]==$this->m_params[$paramname]?"checked":"");
          $res.= '<input type="radio" name="resolution" onClick="';
          $res.= $imgname."_".$paramname."='".$possiblevalues[$i]."'; ".$imgname."_refresh();";
          $res.= '" value="'.$possiblevalues[$i].'" '.$checked.'>'.
          atktext($possiblevalues[$i]);
        }
      }
      if ($addForm) $res.='</form>';
    }

    return $res;
  }

  function _baseUrl()
  {
    return session_url("graph.php?atknodetype=".$this->getSource().
                         "&plotter=competency.".get_class($this).
                         "&callback=".$this->m_callback, SESSION_NEW);
  }



}
class extendedDataGraphAttributePlotter extends dataGraphAttributePlotter
{
  /**
   * Enter description here...
   *
   * @param barLineItem $data
   */

  function barLinePlot($data)
  {

    usegraph("line");
    usegraph("bar");
    include_once(moduleDir("graph")."/class.barLineItem.inc");


    $barline = reset($data);

    $title = $barline->getTitle();
    $dataSet = $barline->getData();
    $lines = array();
    $datax = array();
    $plots = array();

    //We split the data we got so we can generate the needed
    //graphs
    foreach($dataSet as $setKey=>$set)
    {
      $name = $setKey;
      $innerPlots = array();
      foreach($set as $innersetKey=>$innerSet){

        $plot = new BarPlot($innerSet->getData());
        $plot->SetFillColor($innerSet->getColor());
        $plot->SetLegend($name);

        //we check if the bar is an link to a differnt page;
        if($innerSet->getLinks())
        {
          $alts=array("val=%d","val=%d","val=%d","val=%d","val=%d","val=%d");
          $plot->SetCSIMTargets($innerSet->getLinks(),$alts);
        }

        $innerPlots[] = $plot;
      }
      $cBar = new AccBarPlot($innerPlots);
      $plots[] = $cBar;
    }

    $groupPlot = new GroupBarPlot($plots);

    //we setup the graph
    $graphwidth   = atkconfig("graph_width",550);
    $graphheight  = round($graphwidth / 650 * 250);
    // Setup title
    $graph = new Graph($graphwidth,$graphheight+(count($plots)*30),'auto');

    $graph->title->Set($title);
    $graph->title->SetFont(FF_FREESANS, FS_BOLD,14);

    // Use an integer X-scale
    $graph->SetScale("textlin");
    $graph->Set90AndMargin(50,20,50,30);

    // Setup X-axis
    $graph->xaxis->SetTickLabels($datax);
    $graph->xaxis->SetFont(FF_FREESANS, FS_NORMAL,8);

    $graph->add($groupPlot);
    foreach($lines as $line)
    {
      $graph->add($line);
    }

    $graph->Stroke();
    $session = &atkGetSessionManager();
  }

  /**
   * Creates a accumulated bargraph so we can set differnt values in one bar and
   * give those diffent colors.
   * This function uses the same file as the normal bargraph
   * To use give a dataset in this form Array("GRAPHTITLE"=>Array(PLOTTITLE,Array(COLOR=>"color",Value=>(Array(Label,value))))
   * COLOR is optional if it isnt filled the graph will use gray as standard
   * EXAMPLE
   * $grayValues= array(values=>array(1=>1,2=>2,3=>2));//no color given so uses gray
   * $redValues= array("color"=>"red",values=>array(1=>3,2=>5,3=>3));
   * $greenValues= array("color"=>"green",values=>array(1=>4,2=>10,3=>0));
   * $dataset =array($dataset,$dataset2,$dataset4);
   * return array("Example"=>$dataset
   * );
   * @param Array $data
   * @return boolean
   */
  function accBarPlot($data,$raw = false)
  {
    usegraph("bar");
    $dataset = $data;
    $title = key($data);

    // Setup the basic parameters for the graph
    $graphwidth   = atkconfig("graph_width",550);
    $graphheight  = round($graphwidth / 650 * 250);
    $datax= array();
    $plots = array();

    $top = 100;
    $bottom = 30;
    $left = 70;
    $right = 30;
    $graph->Set90AndMargin($left,$right,$top,$bottom);


    //we walk trough all the data sets and plot these
    foreach($dataset as $dkey=>$dvalue){
      foreach($dvalue as $dataKey=>$data){
        $datay=array_values($data["values"]);
        $datax=array_keys($data["values"]);

        $bplot = new BarPlot($datay);
        // We want to display the value of each bar at the top
        $bplot->value->Show();
        $bplot->value->SetFont(FF_FREESANS, FS_BOLD,8);
        $bplot->value->SetAlign('left','center');
        $bplot->value->SetColor("firebrick");
        $bplot->SetFillColor(isset($data["color"])?$test2["color"]:"gray");
        $plots[] = $bplot;
      }
    }

    // Now create a bar pot
    $bplot = new AccBarPlot($plots);

    $bplot->value->Show();
    $bplot->value->SetFont(FF_FREESANS, FS_BOLD,10);

    //if we ant the raw data we dont need the graph so in that case we wont run this code
    if($raw = true){
      // Set the basic parameters of the graph
      $graph = new Graph($graphwidth,$graphheight,'auto');
      $graph->SetScale("textlin");
      $graph->xaxis->SetTickLabels($datax);
      // Setup X-axis
      $graph->xaxis->SetFont(FF_FREESANS, FS_NORMAL,12);

      // Some extra margin looks nicer
      $graph->xaxis->SetLabelMargin(5);

      // Label align for X-axis
      $graph->xaxis->SetLabelAlign('right','center');

      // Add some grace to y-axis so the bars doesn't go
      // all the way to the end of the plot area
      $graph->yaxis->SetLabelAlign('center','bottom');
      $graph->yaxis->SetLabelAngle(60);
      $graph->yaxis->SetLabelFormat('%d');
      $graph->yaxis->SetFont(FF_FREESANS, FS_NORMAL,12);

      // Add the bar to the graph
      $graph->Add($bplot);
      $graph->Stroke();
      return true;
    }
    return $bplot;

  }

  function radarPlot($data)
  {

    usegraph("radar");
    // We only support one dataset, so we only use the first piece of data.
    // $dataset = reset($data);
    $title =$data["name"];

    $dataset=$data["data"];

    //in case we have 2 or less plot points
    if(count($data["data"]) == 1)
    {
      $dataset[" "] = 0;
      $dataset["  "] = 0;
    }
    elseif (count($data["data"]) ==2)
    {
      $dataset[" "] = 0;
    }
    $datay=array_values($dataset);
    $datax=array_keys($dataset);

    // Setup the basic parameters for the graph
    $graphwidth   = atkconfig("graph_width",550);
    $graphheight  = round($graphwidth / 650 * 250);
    $graph = new RadarGraph($graphwidth,$graphheight,"auto");
    $graph->SetTitles($title);
    //$graph->img->SetAntiAliasing();
    $graph->grid->SetLineStyle("solid");
    $graph->grid->SetColor("navy");
    $graph->grid->Show();
    $graph->HideTickMarks();

    $graph->SetShadow();
    $graph->SetTitles($datax);
    $plot = new RadarPlot($datay);
    $plot->mark->SetType(MARK_IMG_SBALL,'red');

    $plot->SetCSIMTargets($data["CSIM"],$data["CSIM"]);

    $plot->SetFillColor("#c2d8ed");

    $graph->Add($plot);
    $graph->StrokeCSIM();
    return true;
  }

  function piePlot($data)
  {
    usegraph("pie");
    // Determine lookup
    $title =$data["name"];

    $dataset=$data["data"];

    // We only support one dataset, so we only use the first piece of data.
    $graphwidth = atkconfig("graph_width",550);
    $graphheight = round($graphwidth / 550 * 250);

    $graph = new PieGraph($graphwidth,$graphheight,"auto");

    $theme = &atkTheme::getInstance();
    $bgColor = $theme->getAttribute("graphBgColor");
    $graph->SetColor($bgColor);

    // Set title
    $graph->title->Set(atktext($title));

    // Use built in font
    $graph->title->SetFont(FF_FREESANS,FS_BOLD);

    // Create a plot
    $p1 = new PiePlot(array_values($dataset));

    $p1->setCenter(0.26,0.5);
    $p1->SetLegends(array_keys($dataset));
    $p1->SetCSIMTargets($data["CSIM"],$data["CSIM"]);
    $graph->Add($p1);

    // Finally output the  image
    $graph->StrokeCSIM();
    return true;
  }

  function combBarPlot($data)
  {
    usegraph("bar");
    usegraph("mgraph");

    $mgraph = new MGraph();
    $mgraph->SetMargin(2,2,2,2);
    $mgraph->SetFrame(true,'darkgray',2);

    $i=0;
    //$dataset = reset($data);
    foreach($data as $objectKey=>$object)
    {
      $title =$objectKey;
      $lbls=array();
      $plotdata = array();
      foreach($object as $itemKey=>$item)
      {
        $lbls[]=$itemKey;

        $dataset= $object;
        // Setup the basic parameters for the graph
        $graphwidth   = atkconfig("graph_width",350);
        $graphheight  = round($graphwidth / 550 * 250);
        $graphwidth   = atkconfig("graph_width",420);

        // Set the basic parameters of the graph
        $graph = new Graph($graphwidth,$graphheight,'auto');
        $graph->SetScale("textlin");

        $top = 60;
        $bottom = 30;
        $left = 100;
        $right = 100;
        $graph->Set90AndMargin($left,$right,$top,$bottom);

        // Setup title
        $graph->title->Set($title);
        $graph->title->SetFont(FF_FREESANS, FS_BOLD,12);

        // Setup X-axis
        //  $graph->xaxis->SetTickLabels(array(1,2,3,4));
        $graph->xaxis->SetFont(FF_FREESANS, FS_NORMAL,10);

        // Add some grace to y-axis so the bars doesn't go
        // all the way to the end of the plot area
        $graph->yaxis->scale->SetGrace(20);
        $graph->yaxis->SetLabelAlign('center','bottom');
        $graph->yaxis->SetLabelAngle(60);
        // $graph->yaxis->SetLabelFormat('%d');
        $graph->yaxis->SetFont(FF_FREESANS,FS_NORMAL,8);

        // We don't want to display Y-axis

        foreach($item as $barDataKey=>$bardata)
        {
          $dat = $bardata==""?0:$bardata;
          $datay=array($dat);
          $datax=$barDataKey;

          $plotdata[$barDataKey][] = $dat;
        }
        $graph->xaxis->SetTickLabels($lbls);

      }

      //now we make plots of our data
      $acc= array();
      $targetlvl = 0;
      foreach($plotdata  as $plotKey=>$plot)
      {
        $bplot = new BarPlot($plot);
        $sort = key($plot);
        if($plotKey=="normal")
        {
          $targetlvl+=$plot[0];
          $bplot->SetFillColor("#c2d8ed;");
        }else if($plotKey=="hasmore"){
          $bplot->SetFillColor("#c2e158");
        }
        else
        {
          $bplot->SetFillColor("red");
          $targetlvl+=$plot[0];
        }


        // We want to display the value of each bar at the top
        $bplot->value->Show();
        $bplot->value->SetFont(FF_FREESANS,FS_BOLD,8);
        $bplot->value->SetAlign('left','center');
        $bplot->value->SetColor("black","darkred");

        // Create and add a new text
        $acc[]=$bplot;
      }
      $gbplot = new AccBarPlot($acc);

      // Add the bar to the graph
      $graph->Add($gbplot);

      $txt=new Text(atkText("target_level","competency")." ". number_format($targetlvl,1));
      $txt->SetPos(320,70);

      $txt->SetFont(FF_FREESANS, FS_BOLD,8);
      $txt->SetBox('lightblue','navy');
      $txt->SetColor("black");
      $graph->AddText($txt);

      $mgraph->Add($graph,0,($graphheight*($i++)));
    }
    $mgraph->Stroke();
    return true;
  }
}
?>
