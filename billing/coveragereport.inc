<?php
 /**
   * Module Resource planning
   * Ganttchart resource planning
   *
   * This file generates the ganttchart. .
   *
   * @author Ludo M. Beumer  <ludo@ibuildings.nl>
   * @version $Revision: 2809 $
   *
   * $Id: coveragereport.inc 2809 2006-08-07 18:02:59Z guido $
   */

  include_once(moduleDir("graph")."jpgraph/jpgraph.php");
  include_once(moduleDir("graph")."jpgraph/jpgraph_line.php");
  include_once(atkconfig("atkroot")."atk/utils/adodb-time.inc.php");
  atkimport("module.utils.dateutil");
  include_once("achievotools.inc");

  atksession();
  atksecure();

  global $g_securityManager;

  /*if ($g_securityManager->allowed("project.project", "any_project"))
  {
    $coordinator = $_REQUEST["coordinator"];
  }
  else
  {
    $user = getUser();
    $coordinator = $user["id"];
  }*/

  $from = dateUtil::startOfWeek($_REQUEST["from"]);
  $to = dateUtil::endOfWeek($_REQUEST["to"]);
  $includeholiday = $_REQUEST["includeholiday"];
  if ($includeholiday!="exclude"&&$includeholiday!="include") $includeholiday="exclude"; // safeguard

  $graphwidth   = atkconfig("graph_width",650);
  $graphheight  = round($graphwidth / 650 * 250);

  $graph        = new  Graph($graphwidth,$graphheight,"auto");

  global $g_theme;
  //$graph->SetMarginColor($g_theme["BgColor"]);


  // Use an integer X-scale
  $graph->SetScale("textlin");

  // Use built in font
  $graph->title->SetFont(FF_FONT1,FS_BOLD);


  $weekdata = dateUtil::weeksBetween($from, $to);

  function getContractHours($weekdata)
  {
    $result=array();
    $db = &atkGetDb();
    foreach($weekdata as $week)
    {
      $recs = $db->getrows("SELECT SUM(uc.uc_hours) as hours, e.billing_declarability
                            FROM
                              person e, usercontract uc
                            WHERE
                              e.id = uc.userid
                              AND e.status='active'
                              AND uc.startdate<'".$week["enddate"]."'
                              AND (uc.enddate IS NULL OR uc.enddate>'".$week["startdate"]."')
                            GROUP BY
                              e.id");

      if (count($recs) == 0)
        $result[] = 0;
      else if (is_null($recs[0]["billing_declarability"]))
        $result[] = $recs[0]["hours"];
      else
        $result[] = round($recs[0]["hours"] * $recs[0]["billing_declarability"] / 100, 2);
    }
    return $result;
  }

  function getCoverage($weekdata)
  {
    $result=array("covered"=>array(), "uncovered"=>array(), "partiallycovered"=>array());
    $db = &atkGetDb();
    foreach($weekdata as $week)
    {
      $recs = $db->getrows("SELECT ph.projectid, (SUM(wp.minutes)/60) as planned
                            FROM
                               weekplanning_planning wp,
                               phase ph,
                               project prj
                             WHERE
                               wp.phaseid = ph.id
                               AND prj.id = ph.projectid
                               AND ph.status='active'
                               AND prj.status='active'
                               AND wp.monday='".$week["startdate"]."'
                             GROUP BY
                               ph.projectid");

      $covered = 0;
      $uncovered = 0;
      $partiallycovered = 0;

      foreach($recs as $rec)
      {
        $status = getCoverageStatus($rec["projectid"]);
        $$status += $rec["planned"];
      }
      $result["covered"][] = $covered;
      $result["uncovered"][] = $uncovered;
      $result["partiallycovered"][] = $partiallycovered;
    }
    return $result;
  }

  function getCoverageStatus($projectid)
  {
    static $s_projects = array();
    if (!isset($s_projects[$projectid]))
    {
      // determine 'covered', 'uncovered' or 'partiallycovered'.
      $db = &atkGetDb();

      $rows = $db->getrows("SELECT
                              prj.id,
                              prj.fixed_price,
                              SUM(i.amount) as income
                            FROM
                              project prj LEFT JOIN billing_income i
                                ON (i.project_id = prj.id)
                            WHERE
                              prj.id = ".$projectid."
                            GROUP BY
                              prj.id,
                              prj.fixed_price");
      if ($rows[0]["income"]>=$rows[0]["fixed_price"])
      {
        $status = "covered";
      }
      else if ($rows[0]["income"]>0)
      {
        $status = "partiallycovered";
      }
      else
      {
        $status = "uncovered";
      }
      $s_projects[$projectid] = $status;
    }
    return $s_projects[$projectid];
  }

  function getHolidayHours($weekdata)
  {
    $result=array();
    $db = &atkGetDb();
    foreach($weekdata as $week)
    {
      $recs = $db->getrows("SELECT fromdate, todate
                            FROM
                               person e, holidayreport_request hr
                             WHERE
                               e.id = hr.userid
                               AND e.status='active'
                               AND hr.status<>'denied'
                               AND hr.fromdate<='".$week["enddate"]."'
                               AND hr.todate>='".$week["startdate"]."'");
      $overlapping_hours = 0;
      foreach($recs as $rec)
      {
        $overlapping_hours += (calculate_overlap($week["startdate"], $week["enddate"], undash($rec["fromdate"]), undash($rec["todate"]))*8);
      }
      $result[] = $overlapping_hours;
    }
    return $result;
  }

  function undash($dasheddate)
  {
    return str_replace("-", "", $dasheddate);
  }

  function calculate_overlap($strstart1, $strend1, $strstart2, $strend2)
  {
    $earliest_end = min($strend2, $strend1);
    $latest_start = max($strstart2, $strstart1);

    $result = dateutil::date_diff($latest_start, $earliest_end)+1;

    return $result;
  }

  $contracthours = getContractHours($weekdata);

  $holidayhours = getHolidayHours($weekdata);

  $coverage = getCoverage($weekdata);

  // 100% = all contracthours (incl/excl holidays, depending on setting)
  // so all other arrays must be calculated as a percentage of the
  // contracthours.
  $baseline = $contracthours;
  if ($includeholiday=="exclude")
  {
    $baseline = deduct($contracthours, $holidayhours);
  }

  function deduct($set1, $set2)
  {
    $result = array();
    for ($i=0, $_i=count($set1); $i<$_i; $i++)
    {
      $result[] = $set1[$i]-$set2[$i];
    }
    return $result;
  }

  function cumulate($set1, $set2)
  {
    $result = array();
    for ($i=0, $_i=count($set1); $i<$_i; $i++)
    {
      $result[] = $set1[$i]+$set2[$i];
    }
    return $result;
  }

  function percentage($set1, $base)
  {
    $result = array();
    for ($i=0, $_i=count($set1); $i<$_i; $i++)
    {
      if ($base[$i]==0) $result[] = 0;
      else $result[] = sprintf("%2d", ($set1[$i]/$base[$i])*100);
    }
    return $result;
  }

  //var_dump($coverage);

  $coveredline = percentage($coverage["covered"], $baseline);
  $partiallycoveredline = cumulate($coveredline, percentage($coverage["partiallycovered"], $baseline));
  $uncoveredline = cumulate($partiallycoveredline, percentage($coverage["uncovered"], $baseline));

  $xlabels = array_keys($weekdata);

  $graph->xaxis->SetTickLabels($xlabels);
  $graph->xaxis->SetTextTickInterval(1);

  // We can place about 5 labels on the x-axis, so we
  // calculate how big the interval should be.
  // To prevent weird unreadable intervals, we round
  // to the nearest multiple of 5 bigger than what we
  // calculated (to prevent label overlaps).
  if (count($xlabels)>5)
  {
    $interv = floor(count($xlabels)/7);
    $graph->xaxis->SetTextLabelInterval(max(1,$interv+(5-($interv%5))));
  }

  $p = &new LinePlot($uncoveredline);
  $p->SetLegend("Ongedekt");
  $p->SetFillColor(COLOR_ERROR);
  $p->mark->SetWidth(14);
  $graph->Add($p);

  $p = &new LinePlot($partiallycoveredline);
  $p->SetLegend("Gedeeltelijk gedekt");
  $p->SetFillColor(COLOR_WARNING);
  $p->mark->SetWidth(4);
  $graph->Add($p);

  $p = &new LinePlot($coveredline);
  $p->SetLegend("Gedekt");
  $p->SetFillColor(COLOR_OK);
  $p->mark->SetWidth(4);
  $graph->Add($p);

  // Dummy base
  $values = array("0");
  $p = &new LinePlot($values);
  $graph->Add($p);

  // Dummy top
  $values = array("100");
  $p = &new LinePlot($values);
  $graph->Add($p);

  $graph->Stroke();
?>
