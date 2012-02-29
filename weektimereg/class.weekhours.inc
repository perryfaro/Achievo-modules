<?php

include_once(moduleDir("timereg")."class.locking.inc");
include_once(moduleDir("timereg")."class.timeregutils.inc");

useattrib("project.projectphaseattrib");
useattrib("project.projectattrib");
useattrib("atkdateattribute");
useattrib("atklistattribute");
useattrib("atktextattribute");
useattrib("atkdurationattribute");
useattrib("atkdummyattribute");
useattrib("weektimereg.weekdateattribute");
userelation("atkmanytoonerelation");

class weekhours extends atkNode
{
  var $m_lock = "";
  var $m_lockmode = "";
  var $m_viewdate = "";
  var $m_user = "";
  var $m_hide_fields_with_remarkrequired = false;

  var $m_groupTotals = array();

  function processGlobalVar($name, $value="")
  {
    global $g_sessionManager;
    $session = &$g_sessionManager->getSession();
    $sessionvarid = "weektimereg_weekhours_$name";
    if (empty($value) && isset($_REQUEST[$name])) $value = $_REQUEST[$name];
    if (!empty($value)) $session[$sessionvarid] = $value;
    return $session[$sessionvarid];
  }

  function weekhours($name="weekhours")
  {
    global $g_sessionManager, $HTTP_POST_VARS;
    $this->m_user = $this->processGlobalVar("viewuser");
    if($this->m_user=="") $this->m_user = atkGetUserId();


    // viewdate is a variable that we need to remember..
    $this->atkNode($name, NF_NO_ADD|NF_NO_EDIT|NF_NO_DELETE);
    if (is_array(atkArrayNvl($_REQUEST, "viewweek"))) // if set by the date jumper, viewdate is an array.
    {
      $date = $this->datefromweek($_REQUEST["viewweek"]["year"], $_REQUEST["viewweek"]["week"]);

      $this->m_viewdate = sprintf("%04d-%02d-%02d", $date["year"], $date["month"],$date["day"]);

      $this->m_viewdate = $this->processGlobalVar("viewdate", $this->m_viewdate);
    }
    elseif (is_array(atkArrayNvl($_REQUEST, "viewdate", false)))
    {
      $date = $_REQUEST["viewdate"];

      $this->m_viewdate = sprintf("%04d-%02d-%02d", $date["year"], $date["month"],$date["day"]);

      $this->m_viewdate = $this->processGlobalVar("viewdate", $this->m_viewdate);
    }
    else
    {
      $this->m_viewdate = $this->processGlobalVar("viewdate");
      if ($this->m_viewdate=="") $this->m_viewdate = strftime("%Y-%m-%d");
    }

    $weekdates = array_slice(TimeregUtils::getWeekDates($this->m_viewdate), 1, 7);

    $weekdatesViewdateIndex = "";

    // Check if the current viewdate is locked, and keep corresponding array
    // index of $weekdatesf it is.
    foreach ($weekdates as $key => $date)
    {
      if (!empty($date['lock']) && $date['date'] == $this->m_viewdate)
      {
        $weekdatesViewDateIndex = $key;
        break;
      }
    }

    // If there are any non-locked days for this perod, we need to set this as
    // m_viewdate, to allow user to add hours at all.
    if ($weekdatesViewDateIndex) {
      $this->m_viewdate = Locking::getFirstNonLockedDayOfPeriod(
                                   $weekdates, $weekdatesViewDateIndex);
    }

    $this->m_lockmode = atkConfig::get("timereg", "lockmode", "week");

    $this->m_hide_fields_with_remarkrequired = atkconfig("weektimereg_hide_remarkrequired", false);

    $flags=NF_NO_VIEW;

    $this->add(new atkAttribute("id",AF_AUTOKEY));

    $this->add(new atkDateAttribute("activitydate","","", 0, date("Ymd"), AF_OBLIGATORY|AF_FORCE_LOAD));

    // force_load flag is set for userid, since several features rely on the
    // userid, even if it's not displayed on screen.
    $this->add(new atkManyToOneRelation("userid","employee.employee", AF_HIDE|AF_FORCE_LOAD|AF_OBLIGATORY));

    $this->add(new atkProjectPhaseAttribute("phaseid",AF_OBLIGATORY|AF_JS_UPDATEACTIVITY));
    $this->add(new atkListAttribute("activityid",array("dummy","dummy","dummy","dummy","dummy"),array(0,0,0,0,0),AF_OBLIGATORY, 10));
    $this->add(new atkTextAttribute("remark", atkconfig("timereg_remark_lines", 1), 0));
    $this->add(new atkDurationAttribute("time",'15m', '10h',AF_OBLIGATORY|AF_TOTAL));
    $this->add(new atkDateAttribute("entrydate",AF_HIDE|AF_OBLIGATORY));

    $this->setOrder("hours.id"); // Sort by order of input..
    $this->addFilter("hours.userid",$this->m_user);

    $this->setTable("hours","hours");
    $this->setSecurityAlias("timereg.hours");
    $this->addSecurityMap("weekregistration", "admin");
    $this->addSecurityMap("add_allowed_timereg","admin");
  }

  function initial_values()
  {
    global $g_user;

    return array("userid"=>array("id"=>$g_user["id"]),
    "entrydate"=>Array("year"=>date("Y"),
    "month"=>date("m"),
    "day"=>date("d")),
    "activitydate"=>$this->getInitialDate()
    );
  }

  function setLock()
  {
    $userid = atkArrayNvl(getUser(), "id");
    $this->m_lock = Locking::getLockType($userid, $this->m_viewdate);
  }

  function action_weekregistration()
  {
    $this->setLock();
    $this->addFilter("hours.userid",atkGetUserId());
    $this->weekview();
  }

  /**
     * Determine the number of available activities
     */
  function getActivityCount()
  {
    $db = &atkGetDb();
    $result = $db->getrows("SELECT count(*) as activitycount FROM activity");
    return $result[0]['activitycount'];
  }

  /**
     * Generates the weekplanning html to be included in the weekregistration screen if the weekplanning module is installed
     *
     * @param Integer $userid User for which the planning must be listed
     * @return string HTML code containing weekplanning
     */
  function getWeekPlanning($userid, $monday)
  {
    // Don't attempt to display the weekplanning if the weekplanning module is not installed
    if (!moduleExists("weekplanning"))
    return "";

    // Open the weekplanning output in a righ-aligned div element with the text "Planning" in the header
    $output = "<div style=\"float:right;width:50%\">";
    $output.= "<b>" . atktext("weekplanning","weekplanning") . ":</b>";

    // Get the weekplanning_peremployee node (used to get a list of projects and planningdata)
    $weekplanning_peremployee_node = &getNode("weekplanning.weekplanning_peremployee");

    // Determine the startdate and enddate in the format which is accepted by the weekplanning modules
    $startdate = date("Ymd", $monday);
    $enddate = date("Ymd", $monday + (7 * 24 * 60 * 60));

    // Read the projects and planningdata from the weekplanning node
    $planning = $weekplanning_peremployee_node->getPlanningData($userid, $startdate, $enddate);

    // Get the project node (used to get a descriptor for displayed projects)
    $phasenode = &getNode("project.phase");

    // Open a table for the planning data
    $output.= "<table border=\"0\" cellspacing\=\"0\" cellpadding=\"0\">";

    // Loop through all projects
    foreach($planning as $phaseid => $plandata)
    {
      $phaserecs = $phasenode->selectDb("phase.id=".$phaseid, "", "", "", array("name", "projectid"));
      $time = $plandata[date("Ymd", $monday)]*60;
      if ($time>0)
      {
        $output.= "<tr><td>" . $phaserecs[0]["projectid"]["name"]." / ".$phasenode->descriptor($phaserecs[0]) . "</td><td>:&nbsp;</td><td>";
        $output.= time_format($time) . "h </td></tr>";
      }
    }

    // Close the planning data table
    $output.= "</table>";

    // Close the weekplanning div block
    $output.= "<br><br></div>";

    // Return the generated output:
    return $output;
  }

  function weekview()
  {
    $viewtime = TimeregUtils::getViewTime($this->m_viewdate);
    $ui = &$this->getUi();
    $title = atktext('title_weekhouradmin', "weektimereg").' ('.atktext("week").' '.strftime('%V, %Y',$viewtime).')';
    $content = $this->getWeekView($viewtime);
    $boxedcontent = $this->genericPage($title, $content);

    $page = &$this->getPage();
    $page->register_script(atkconfig("atkroot")."atk/javascript/formsubmit.js");
    $page->register_script(moduleDir("weektimereg")."javascript/validateinput.js");
    $page->addContent($boxedcontent);
  }

  function _getWeekViewNavigation($weekdates, $viewtime, $locks, $userid)
  {
    // First a line with previous and next week links..
    $prevnextlinks='<form name="jumper" method="post" action="dispatch.php">';
    $prevnextlinks.=session_form();
    $prevnextlinks.='<table border=0 cellpadding=0 cellspacing=0 width="100%" class="table">';
    $prevnextlinks.='<tr><td valign="top" align="left">';
    $prevnextlinks.= ''.href('dispatch.php?viewdate='.$weekdates[0]['date'], atktext('previousweek'));
    $prevnextlinks.= '&nbsp;&nbsp;'.href('dispatch.php?viewdate='.date("Y-m-d"), atktext('thisweek'));

    if ($weekdates[8]['date'] <= date("Y-m-d"))
    {
      $prevnextlinks.='&nbsp;&nbsp;'.href('dispatch.php?viewdate='.$weekdates[8]['date'],atktext('nextweek'));
    }


    // Locking / Unlocking links
    if ($this->m_lockmode == "week")
    {
      if ($this->m_lock=="individual" && $this->allowed("unlock"))
      {
        $prevnextlinks.='&nbsp;&nbsp;'.href('dispatch.php?atkaction=unlock&userid='.$userid.'&viewdate='.$this->m_viewdate,'<b>'.atktext("unlock").'</b>');
      }
      // users may only lock weeks that are not already locked. they must have the proper permission
      // and, if configured, they may only lock a week once they have booked all their hours.
      else if ($this->m_lock=="" && $this->allowed("lock") && (atkconfig("timereg_incompleteweeklock") || $this->weekComplete($userid, $this->m_viewdate)))
      {
        $prevnextlinks.='&nbsp;&nbsp;'.href('dispatch.php?atkaction=lock&userid='.$userid.'&viewdate='.$this->m_viewdate,'<b>'.atktext("lock").'</b>');
      }
    }

    $prevnextlinks.='</td><td valign="top" align="right">';
    $prevnextlinks.=$this->getUserDropdown();
    $prevnextlinks.=$this->getDateJumper();
    $prevnextlinks.='&nbsp;<input type="submit" value="'.atktext("goto").'">';

    $prevnextlinks.='</td></tr></table></form>';

    $weekday = strftime("%w",$viewtime);
    if ($weekday==0) $weekday=7;

    $monday = $viewtime-(86400*($weekday - 1));
    $prevnextlinks.= $this->getWeekPlanning($userid, $monday);

    $prevnextlinks.='<form name="entryform" method="post" action="dispatch.php">';
    $prevnextlinks.=session_form(SESSION_NESTED);

    $partialLockString = ($locks > 0 && $locks < 7) ? ' ('.atktext("locked_partially").')' : null;
    $prevnextlinks.='<b>'.atktext("week").' '.strftime('%V, %Y',$viewtime).'</b>'.Locking::getLockIcon($locks).$partialLockString.'<br><b>'.atktext("monday").' '.$weekdates[1]["dateday"].' '.$weekdates[1]["month"].' - '.$weekdates[7]["dateday"].' '.$weekdates[7]["month"].' '.strftime('%Y',$viewtime).'</b><br><br><br>';
    $prevnextlinks.='<br style="clear:both;">';

    return $prevnextlinks;
  }

  function getweekview($viewtime, $shownavigation = true, $readonly = false)
  {
    global $g_user, $ATK_VARS;

    $result = "";
    $locks = 0;

    $db = &atkGetDb();
    $sessionmanager = &atkGetSessionManager();

    $weekdates = TimeregUtils::getWeekDates($this->m_viewdate);

    // Get number of locked days
    foreach(array_slice($weekdates, 1, 7) as $date)
    {
      $locks += ($date['lock'] != "") ? 1 : 0;
    }

    $week = $this->getBookableEntries($weekdates[1]['date'], $weekdates[7]['date']);

    $userid = $g_user["id"];

    if ($shownavigation)
    {
      $prevnextlinks = $this->_getWeekViewNavigation($weekdates, $viewtime, $locks, $userid);
      $result .= $prevnextlinks;
    }

    $result .='<input type="hidden" name="atkaction" value="save">
                 <input type="hidden" name="atknodetype" value="weektimereg.weekhours">';


    $headers = array();

    if (atkConfig::get('weektimereg', 'seperatedProjectPhase', false))
    {
      $headers[] = atktext('project');
      $headers[] = atktext('phase');
    }
    else
      $headers[] = atktext('project').'/'.atktext('phase');

    $activityCountIsOne = $this->getActivityCount() == 1;
    if (!$activityCountIsOne) {
      $headers[] = atktext('activity');
    }

    $lockicon = "";
    for ($i=1;$i<=7;$i++)
    {
      // Individual lock icons for each day
      if ($this->m_lockmode != "week")
      {
        $lockicon = Locking::getLockIcon($weekdates[$i]['lock'], "11");
      }
      $dateattribute = new atkDateAttribute("dummy", "j M", "j M");
      $date = array("dummy" => array("year"=>substr($weekdates[$i]["date"],0,4), "month"=>substr($weekdates[$i]["date"],5,2), "day"=>substr($weekdates[$i]["date"],8,2)));
      $datefield = $dateattribute->display($date, "plain");
      $headers[] = $weekdates[$i]['day'].$lockicon.'<br>'.str_replace(" ", "&nbsp;", $datefield);
    }

    $headers[] = atktext('total');
    $headers[] = atktext('remark');
    $this->add_billing_headers($headers);

    $row = 0;

    // Initialize the daytotals array with 0 for each weekdate
    $daytotals = array();
    for ($i=1;$i<=7;$i++)
    {
      $daytotals[$weekdates[$i]['date']] = 0;
    }
    $total = 0;
    $j = 0;

    // Load the selected phases and activities
    $selectedphaseactivities = $sessionmanager->pageVar("selectedphaseactivities");

    // If a phase/activity was selected, add it.
    if ((!$activityCountIsOne && (atkArrayNvl($ATK_VARS, "selectedactivityid") != "")) || ($activityCountIsOne && (atkArrayNvl($ATK_VARS, "selectedphaseid") != "")))
    {
      $selection = array();
      $selection["phase"] = atkArrayNvl($ATK_VARS, "selectedphaseid");
      if (!$activityCountIsOne)
      {
        $selection["activity"] = atkArrayNvl($ATK_VARS, "selectedactivityid");
      }

      $selectedphaseactivities[] = $selection;
    }

    // Delete any phase/activites that already exist
    if (is_array($selectedphaseactivities) && (count($selectedphaseactivities) > 0))
    {
      // Delete already visible selectedphaseactivities
      for($i=0,$_i=count($selectedphaseactivities); $i<$_i; $i++)
      {
        foreach($week as $weekitem)
        {
          if (($weekitem["phaseid"] == $selectedphaseactivities[$i]["phase"]) && (($activityCountIsOne) || ($weekitem["activityid"] == $selectedphaseactivities[$i]["activity"])))
          {
            unset($selectedphaseactivities[$i]);
          }
        }
      }
    }

    // Save the phase/activities array
    $sessionmanager->pageVar("selectedphaseactivities", $selectedphaseactivities);

    if (is_array($selectedphaseactivities) && (count($selectedphaseactivities) > 0))
    {
      $sql = "SELECT project.id as projectid, phase.id as phaseid, activity.id as activityid, activity.remarkrequired, phase.name as phasename, project.name as projectname, project.abbreviation as projectabbreviation, activity.name as activityname\n";
      $selectorelements = array();

      if (!$activityCountIsOne) {

        foreach($selectedphaseactivities as $selectedphaseactivity)
        {
          $selectorelements[] = "(phase.id = ".$selectedphaseactivity["phase"]." AND activity.id = ".$selectedphaseactivity["activity"].")";
        }

        $sql .= "FROM project, phase, activity
                 WHERE project.id = phase.projectid ";

        if (count($selectorelements) > 0)
        {
          $sql .= "AND (".implode(" OR ", $selectorelements).")";
        }
      }
      else
      {
        foreach($selectedphaseactivities as $selectedphaseactivity)
        {
          $selectorelements[] = "(phase.id = ".$selectedphaseactivity["phase"].")";
        }

        $sql .= "FROM project, phase, phase_activity, activity
                  WHERE project.id = phase.projectid
                  AND phase_activity.phaseid = phase.id
                  AND phase_activity.activityid = activity.id ";

        if (count($selectorelements) > 0)
        {
          $sql .= "AND (".implode(" OR ", $selectorelements).")";
        }
      }

      $selectdata = $db->getRows($sql);

      for($i=0,$_i=count($selectdata); $i<$_i; $i++)
      {
        $j = count($week);
        $week[$j]["projectname"] = $selectdata[$i]["projectname"];
        $week[$j]["projectabbreviation"] = $selectdata[$i]["projectabbreviation"];
        $week[$j]["phasename"] = $selectdata[$i]["phasename"];
        $week[$j]["activityname"] = $selectdata[$i]["activityname"];
        $week[$j]["projectid"] = $selectdata[$i]["projectid"];
        $week[$j]["phaseid"] = $selectdata[$i]["phaseid"];
        $week[$j]["activityid"] = $selectdata[$i]["activityid"];
        $week[$j]["remark"] = "";
        if ($this->m_hide_fields_with_remarkrequired)
        {
          $week[$j]["remarkrequired"] = $selectdata[$i]["remarkrequired"];
        }
        for ($k = 0; $k < 7; $k++)
        {
          $date = date("Y-m-d", mktime(0, 0, 0, substr($weekdates[1]['date'], 5, 2), substr($weekdates[1]['date'], 8, 2) + $k,substr($weekdates[1]['date'], 0, 4)));
          $week[$j]["activitydate"][$date]["hoursid"] = "";
          $week[$j]["activitydate"][$date]["date"] = $date;
          $week[$j]["activitydate"][$date]["time"] = 0;
        }
      }
    }

    $j = 0;
    $groupDayTotals = array();
    $lastprojectName = null;
    // iterate over all project-week items
    for ($i=0;$i<count($week);$i++)
    {
      $datarow = array();
      if (atkConfig::get('weektimereg', 'seperatedProjectPhase', false))
      {
        $projectname = $week[$i]["projectabbreviation"].' '.$week[$i]["projectname"];
        $datarow[] = $projectname;
        $datarow[] = $week[$i]["phasename"];
      }
      else
      {
        $projectname = $week[$i]["projectabbreviation"].' '.$week[$i]["projectname"].'/'.$week[$i]["phasename"];
        $datarow[] = $projectname;
      }

      if (!$activityCountIsOne) {
        $datarow[] = $week[$i]["activityname"];
      }

      $projtotal = 0;

      // iterate over the dates in the current week
      foreach ($week[$i]["activitydate"] as $key => $value)
      {
        // Only show input boxes for days which are not locked
        if ($key <= date("Y-m-d"))
        {
          $dayLock =  Locking::getLockType(atkArrayNvl(getUser(), "id"),$key);
          $input = "";
          $originalTime = $week[$i]["activitydate"][$key]["time"];
          $originalTimeString = trim(str_replace("&nbsp;", "", time_format($originalTime)));

          // grouping totals
          if (!isset($groupDayTotals[$key])) {
            $groupDayTotals[$key] = 0 ;
          }

          // init for the first group
          if ($lastprojectName === null)
          {
            $lastprojectName = $projectname;
          }

          // new group start?
          if ($lastprojectName !== $projectname)
          {
              $this->_processLastTotalsGroup($lastprojectName, $groupDayTotals);

              $groupDayTotals = array();
          }

          $groupDayTotals[$key] += $originalTime;
          $lastprojectName = $projectname;

          // formatting
          if ($originalTimeString != "")
          {
            if (($dayLock == "") && (!$readonly) && $value['readonly'] !== true)
            {
              $input .= '<input type="text" name="act_input[' . $j . '][input]" size="4" value="'.$originalTimeString.'" onBlur="validateInput(this, \''.atktext("nocorrectminute", "weektimereg").'\');">';
            }
            else
            {
              $input .= $originalTimeString;
            }
            $input .= '<input type="hidden" name="act_input[' . $j . '][orginput]" value="'.$originalTimeString.'">';
          }
          else
          {
            $recordReadonly = array_key_exists('readonly', $value) && $value['readonly'] === true;
            if (($dayLock == "") && (!$readonly) && (!$recordReadonly))
            {
              $input .= '<input type="text" name="act_input[' . $j . '][input]" size="4" value="'.$originalTimeString.'" onBlur="validateInput(this, \''.atktext("nocorrectminute", "weektimereg").'\');">';
            }
            else
            {
              $input = "";
            }
            $input .= '<input type="hidden" name="act_input['.$j.'][orginput]" value="">';
          }

          $input .= '<input type="hidden" name="act_input[' . $j . '][projectid]" value="'.$week[$i]["projectid"].'">';
          $input .= '<input type="hidden" name="act_input[' . $j . '][phaseid]" value="'.$week[$i]["phaseid"].'">';
          $input .= '<input type="hidden" name="act_input[' . $j . '][activityid]" value="'.$week[$i]["activityid"].'">';
          $input .= '<input type="hidden" name="act_input[' . $j . '][hoursid]" value="'.$week[$i]["activitydate"][$key]["hoursid"].'">';
          $input .= '<input type="hidden" name="act_input[' . $j . '][date]" value="'.$week[$i]["activitydate"][$key]["date"].'">';
          $input .= '<input type="hidden" name="act_input[' . $j . '][week]" value="'.$i.'">';
          $datarow[] = $input;
          $projtotal += $originalTime;
          $daytotals[$key] += $originalTime;
          $total += $originalTime;
          $j++;
        }
        else
        {
          $datarow[] = "&nbsp;";
        }
      }

      // add the last group too!
      $this->_processLastTotalsGroup($lastprojectName, $groupDayTotals);


      $datarow[] = time_format($projtotal);

      if (($this->m_lock == "") && (!$readonly))
      $remark = '<input type="text" name="week_input[' . $i . '][remark]" value="'.$week[$i]["remark"].'">';
      else
      $remark = $week[$i]["remark"].'<input type="hidden" name="week_input[' . $i . '][remark]" value="'.$week[$i]["remark"].'">';
      $remark.= '<input type="hidden" name="week_input[' . $i . '][orgremark]" value="'.$week[$i]["remark"].'">';
      $remark.= '<input type="hidden" name="week_input[' . $i . '][projectid]" value="'.$week[$i]["projectid"].'">';
      $remark.= '<input type="hidden" name="week_input[' . $i . '][phaseid]" value="'.$week[$i]["phaseid"].'">';
      $remark.= '<input type="hidden" name="week_input[' . $i . '][activityid]" value="'.$week[$i]["activityid"].'">';
      $remark.= '<input type="hidden" name="week_input[' . $i . '][date]" value="'.$week[$i]["activitydate"][$key]["date"].'">';
      $datarow[] = $remark;

      $this->add_billing_fields($datarow,$week[$i]);

      $data[] = $datarow;
    }

    // create groups
    if (atkConfig::get('weektimereg', 'collapseProjects', true))
    {
      $data = $this->_createGroups($data, 0, $activityCountIsOne);
    }

    // add headers to top after creating groups
    array_unshift($data, $headers);

    $datarow = array();

    $datarow[] = atktext('total');
    if (!$activityCountIsOne) {
      $datarow[] = "&nbsp;";
    }

    if (atkConfig::get('weektimereg', 'seperatedProjectPhase', false))
      $datarow[] = "&nbsp;";

    for ($i=1;$i<=7;$i++)
    {
      $datacell = time_format($daytotals[$weekdates[$i]['date']]);
      if ($daytotals[$weekdates[$i]['date']]>atkconfig("overtimethreshold"))
      $datacell = '<font color="#FF0000">' . $datacell . '</font>';
      $datarow[] = $datacell;
    }
    $datarow[] = time_format($total);
    $datarow[] = "&nbsp;";

    $this->add_billing_footers($datarow);

    $data[] = $datarow;
    $tablerenderer = &atknew("atk.utils.atktablerenderergrouped");

    for($i=$activityCountIsOne?1:2, $_i=count($datarow); $i<$_i; $i++)
    {
      $tablerenderer->setColAlignment($i, TBL_RIGHT);
    }
    $result .= $tablerenderer->render($data, TBL_DATA|TBL_FOOTER, "recordlist");

    if (($this->m_lock == "") && (!$readonly))
    {
      if (atkConfig::get('weektimereg','hide_add_project_link') !== true)
      {
        $result .= "<br>" . $this->getProjectSelectorAddLink($activityCountIsOne).'<br><br>';
      }

      $result.='<table border=0 cellpadding=0 cellspacing=0 width="100%" class="table">';

      $result.='<tr><td valign="top" align="center">';
      if ($this->m_lock=="")
      {
        $result.='<input type="submit" value="'.atktext("save").'">';
      }
      $result.='</td></tr></table>';
    }
    $result.='</form><br>';

    if (atkConfig::get('weektimereg','project_selection_byteam_activityfilter') > 0 && $this->m_lock == "")
    {
      $extra_selector = $this->getAdditionalActivitySelector($weekdates[1]['date'], $weekdates[7]['date'],$userid);

      if ($extra_selector != "")
      $result .= "<b>" . sprintf(atktext("add_new", "weektimereg"), atktext("project")."/".atktext("phase")."/".atktext("activity")) . ":</b><br/>" . $extra_selector;
    }

    return $result;
  }

  function _processLastTotalsGroup($lastprojectName, $groupDayTotals)
  {
    ksort($groupDayTotals);
    $readableTotals = array();
    $grandTotal = 0;
    foreach ($groupDayTotals as $groupday)
    {
      $grandTotal += $groupday;
      $readableTotals[] = trim(str_replace("&nbsp;", "", time_format($groupday)));
    }

    $fill = 7 - count($groupDayTotals);
    if ($fill > 0)
    {
      for($i=0;$i<$fill;$i++)
      {
        $readableTotals[] = '&nbsp;';
      }
    }

    $readableTotals[] = trim(str_replace("&nbsp;", "", time_format($grandTotal)));;

    $this->m_groupTotals[$lastprojectName] = $readableTotals;
  }

  // creates groups on uniqueness of a column
  // @todo maybe move this to tablerenderergrouped as a utility method?
  function _createGroups($data, $col = 0, $activitycountisone=false)
  {
    $rows = array();
    $newgroup = null;

    $lastid = null;
    $count = count($data);
    for ($i=0;$i<=$count;$i++)
    {
      $row = $data[$i];
      // wrap up
    	if ($i == $count || ($newgroup !== null && $row[$col] !== $lastid))
    	{
    	  if (count($newgroup['rows']) > 1)
    	  {
    	    $rows[] = $newgroup;
    	  }
    	  else
    	  {
    	    $rows[] = $newgroup['rows'][0];
    	  }

    	  $newgroup = null;
    	}

    	// start a new group
  	  if ($newgroup === null)
  	  {
  	    $header = array($row[0]);

  	     // add another empty col if project and phase are not in one col.
        if (atkConfig::get('weektimereg', 'seperatedProjectPhase', false))
        {
          $header[] = '';
        }

        if (!$activitycountisone)
        {
          $header[] = '';
        }

  	    if (array_key_exists($row[0], $this->m_groupTotals)) {
  	     $header = array_merge($header, $this->m_groupTotals[$row[0]]);
  	    }

        $newgroup = array(
          'type' => 'group',
          'id' => 'weekhoursgroup_'.$i,
          'rows' => array(),
          'header' => $header,
        );
  	  }

  	  $newgroup['rows'][] = $row;
    	$lastid = $row[$col];
    }

    return $rows;
  }

  function getUserDropdown()
  {
    $reportutils = &atknew("modules.reports.utils.reportutils");
    $ret = '<select name="viewuser">';
    $ret.= $reportutils->getLevelEmployees($this->m_user);
    $ret.= '</select>';
    return $ret;
  }

  function getProjectSelectorAddLink($activityCountIsOne,$node="weektimereg.weekhours")
  {
    if ($activityCountIsOne)
    {
      $targeturl_weekhour_admin = dispatch_url($node, "weekregistration", array("selectedphaseid" => "[id]","atklevel"=>atkLevel()));;
      $targeturl_phase = dispatch_url("project.phase", "select", array("atkfilter" => "projectid=[id]","atktarget" => atkurlencode($targeturl_weekhour_admin)));
      $linktext = sprintf(atktext("add_new", "weektimereg"), atktext("project")."/".atktext("phase"));
    }
    else
    {
      $targeturl_weekhour_admin = dispatch_url($node, "weekregistration", array("selectedactivityid" => "[activityid.id]", "selectedphaseid" => "[phaseid.id]","atklevel"=>atkLevel()));;
      if ($this->m_hide_fields_with_remarkrequired)
      {
        // Hide the activities with a second fiter (for activity additions) (sleblanc)
        $targeturl_activity = dispatch_url("project.phase_activity", "select", array("atkfilter" => "phaseid=[id] and remarkrequired=0","atktarget" => atkurlencode($targeturl_weekhour_admin)));
      }
      else
      {
        $targeturl_activity = dispatch_url("project.phase_activity", "select", array("atkfilter" => "phase_activity.phaseid=[id]","atktarget" => atkurlencode($targeturl_weekhour_admin)));
      }
      $targeturl_phase = dispatch_url("project.phase", "select", array("atkfilter" => "projectid=[id]","atktarget" => atkurlencode($targeturl_activity)));
      $linktext = sprintf(atktext("add_new", "weektimereg"), atktext("project")."/".atktext("phase")."/".atktext("activity"));
    }

    return href(dispatch_url("project.projectselector", "select", array("atktarget" => atkurlencode($targeturl_phase),"viewuser"=>$this->m_user)), $linktext, SESSION_NESTED, true);
  }

  function action_lock()
  {
    $period = Locking::getPeriod($this->m_postvars["viewdate"]);
    Locking::lockPeriod($period);

    $this->redirect();
  }

  function action_unlock()
  {
    $viewdate = $this->m_postvars["viewdate"];
    Locking::unlockPeriod(Locking::getPeriod($viewdate));
    $this->redirect();
  }

  function getDateJumper()
  {
    if (atkConfig::get('weektimereg', 'useCalendar', false))
    {
      $dummy_rec = array("viewdate"=>$this->getInitialDate());
      $datebox = new atkDateAttribute("viewdate","","",0, 0,AF_OBLIGATORY);
    }
    else
    {
      $dummy_rec = array("viewweek"=>$this->getInitialDate());
      $datebox = new weekDateAttribute("viewweek",AF_OBLIGATORY);
    }

    return $datebox->edit($dummy_rec);
  }

  function getInitialDate()
  {
    if ($this->m_viewdate!="")
    {
      $initial_date = Array("year"=>substr($this->m_viewdate,0,4),
      "month"=>substr($this->m_viewdate,5,2),
      "day"=>substr($this->m_viewdate,8,2));
    }
    else
    {
      $initial_date = Array("year"=>date("Y"),
      "month"=>date("m"),
      "day"=>date("d"));
    }
    return $initial_date;
  }


  function datefromweek ($y, $w, $o=0)
  {
    $year = $y;
    $week = $w;

    $startDay = Mon;
    //      $endDay = Sun;

    $weeknumber = date("W", mktime(0,0,0,1,1,$year));

    // According to ISO 8601 the first Thursday of January is week 1.
    // So if 1 January is Friday, Saturday or Sunday 1 January belongs to the last week of previous year.
    // So for our calculation, if 1 January is ISO weeknumber 1 we have to decrease the $w by 1 to get the
    // requested week.
    if ($weeknumber == 1)
    {
      $week -= 1;
    }

    $startdate =  strtotime('+' . $week . ' week',mktime(0,0,0,1,1,$year));
    //      $enddate = $startdate;

    while(date("D",$startdate) != $startDay)
    {
      $startdate = mktime(0,0,0,date("m",$startdate),date("d",$startdate)-1, date("Y",$startdate));
    }

    //      while(date("D",$enddate) != $endDay)
    //      {
    //        $enddate = mktime(0,0,0,date("m",$enddate),date("d",$enddate)+1, date("Y",$enddate));
    //      }

    $tempDate = getdate($startdate);

    $date["year"] = $tempDate["year"];
    $date["month"] = $tempDate["mon"];
    $date["day"] = $tempDate["mday"];

    return ($date);
  }

  function weekComplete($userid, $date)
  {
    $start = startOfWeek($date);
    $end = endOfWeek($date);

    // first get the total of contract hours
    $sql = "SELECT sum(uc_hours*60) as contract FROM usercontract WHERE startdate <= '$start'
                AND enddate > '$start' AND userid = '$userid'";
    $db = &atkGetDb();
    $result = $db->getRows($sql);

    // get the total of time already registered this week
    $query = "SELECT sum(time) as totaal
              FROM
                hours
              WHERE
                hours.userid = '$userid'
                AND activitydate between '$start' and '$end'";
    $res = $db->getrows($query);
    if (count($res)==0 || count($result)==0)
    {
      // either no contract in this period, or no hours booked at all..
      return false;
    }
    else
    {
      return ($result[0]["contract"] - $res[0]["totaal"]<=0);
    }
  }

  function getWeekPlanningPhaseList($userid, $monday)
  {
    if (moduleExists("weekplanning"))
    {
      // Query the database for phases in weekplanning
      $db = &atkGetDb();
      $monday_weekplanningformat = date("Ymd", mktime(0,0,0,substr($monday, 5, 2),substr($monday, 8, 2),substr($monday, 0, 4)));
      $query = "SELECT phase.id AS phaseid FROM weekplanning_planning
                  LEFT JOIN phase ON weekplanning_planning.phaseid = phase.id
                  LEFT JOIN project ON phase.projectid = project.id
                  WHERE weekplanning_planning.monday='$monday_weekplanningformat'
                  AND weekplanning_planning.employeeid = '$userid';";
      $projectids = $db->getrows($query);

      // Build a comma seperated list of phase ids
      $phaselist = array();
      for($i=0,$_i=count($projectids); $i<$_i; $i++)
      if (atkArrayNvl($projectids[$i], "phaseid") != "")
      $phaselist[] = $projectids[$i]["phaseid"];
      $phaseliststring = implode(",", $phaselist);

      // Return the list
      return $phaseliststring;
    }
    return "";
  }

  function getEntriesByTeamArray($startdate, $enddate, $userid, $reverseActivityFilter = false)
  {
    $db = &atkGetDb();
    $activity_id = atkConfig::get('weektimereg','project_selection_byteam_activityfilter');

    if ($activity_id > 0)
    {
      // get allowed activities
      $weeknumber = date("YW",strtotime($startdate));
      $extra_allowed = $db->getRows("SELECT projectid,phaseid,activityid
                                       FROM weektimereg_byteam_extra_allowed
                                       WHERE week = '$weeknumber' AND userid = '$userid'");

      if (is_array($extra_allowed) && sizeof($extra_allowed) > 0)
      {
        $eaa = array();
        foreach($extra_allowed as $ea)
        {
          $eaa[] = "(project.id = '{$ea['projectid']}' AND phase.id = '{$ea['phaseid']}' AND activity.id = '{$ea['activityid']}')";
        }
        $extra_allowed_activities = implode(" OR ",$eaa);
      }
    }

    // first get all the data of the current week
    $query = "SELECT
                  project.id as projectid,
                  phase.id as phaseid,
                  activity.id as activityid,
                  activity.description as activitydescription,
                  activity.remarkrequired,
                  phase.name as phasename,
                  project.abbreviation as projectabbreviation,
                  project.name as projectname,
                  activity.name as activityname,
                  hours.time,
                  hours.activitydate, hours.id as hoursid,
                  hours.remark as remark,
                  GREATEST(phase.startdate,project.startdate) AS project_startdate,
                  IF(phase.enddate = '0000-00-00' OR phase.enddate IS NULL OR project.enddate < phase.enddate,project.enddate,phase.enddate) AS project_enddate
                FROM
                  (project,phase,phase_activity,activity)
                LEFT JOIN project_person ON (project_person.projectid = project.id AND project_person.personid = '$userid')
                LEFT JOIN
                  hours
                    ON (phase.id = hours.phaseid AND hours.activityid = activity.id
                       AND hours.activitydate >= '$startdate' AND hours.activitydate <= '$enddate'
                       AND hours.userid = '".$userid."')
                WHERE
                   project.id = phase.projectid
                  AND
                   phase.id = phase_activity.phaseid
                  AND
                   phase_activity.activityid = activity.id
                  AND
                   (project.alwaysvisibleintimereg = 1 OR
                   (project.startdate <= '$enddate' AND project.enddate >= '$startdate'))
                  AND
                   (project.timereg_limit <> 1 OR project_person.personid IS NOT NULL)
                   ";

    if ($activity_id > 0)
    {
      $query .= " AND
                    (
                      activity.id ".($reverseActivityFilter?"<>":"=")." '$activity_id'
                      ";

      if ($extra_allowed_activities != "")
      $query .= " " . ($reverseActivityFilter ? " AND NOT " : " OR ") . "
                      ($extra_allowed_activities)";

      $query .= "
                    )
                     ";
    }

    $query .= "
                ORDER BY
                  project.name, phase.name, activity.name, activitydate";

    $rows = $db->getRows($query);
    return $rows;

  }

  function getAdditionalActivitySelector($startdate,$enddate,$userid)
  {
    if (atkConfig::get('weektimereg','project_selection_byteam_activityfilter') < 1) return "";

    $results = $this->getEntriesByTeamArray($startdate,$enddate,$userid,true);
    if (!is_array($results) || sizeof($results) == 0) return "";

    // rebuild resultset
    $projects = array();

    foreach($results as $row)
    {
      if (!array_key_exists($row['projectid'],$projects))
      $projects[$row['projectid']] = array("phases"=>array(),"projectname"=>$row['projectname']);

      if (!array_key_exists($row['phaseid'],$projects[$row['projectid']]["phases"]))
      $projects[$row['projectid']]["phases"][$row['phaseid']] = array("activities"=>array(),"phasename"=>$row['phasename']);

      $projects[$row['projectid']]["phases"][$row['phaseid']]["activities"][$row['activityid']] = $row['activityname'].' ('.$row['activitydescription'].')';
    }

    $page = &atkPage::getInstance();
    $page->register_script(moduleDir('weektimereg') . 'javascript/toggle_phase_activity_boxes.js');

    $project_selector     = "";
    $phase_selectors      = array();
    $activity_selectors   = array();

    $project_selector = "<select name='weekhours_add_project_selector' id='weekhours_add_project_selector' onchange='togglePhaseActivities(this)'>";
    $project_selector .= "<option value='0'>" . atktext('please_select_project','weektimereg') . "</option>";
    foreach($projects as $project_id => $project)
    {
      $project_selector .= "<option value='$project_id'>" . $project['projectname'] . "</option>";

      $myphaseselector = "";
      $myphaseselector .= "<select name='weekhours_add_phase_selector_$project_id' id='weekhours_add_phase_selector_$project_id' style='display: none;' onChange='togglePhaseActivities(this)'>";
      $myphaseselector .= "<option value='0'>" . atktext('please_select_phase','weektimereg') . "</option>";
      foreach($project['phases'] as $phase_id => $phase)
      {
        $myphaseselector .= "<option value='$phase_id'>" . $phase['phasename'] . "</option>";

        $myactselector  = "";
        $myactselector .= "<select name='weekhours_add_activity_selector_{$phase_id}_{$project_id}' id='weekhours_add_activity_selector_{$phase_id}_{$project_id}' style='display: none;' onChange='togglePhaseActivities(this)'>";
        $myactselector .= "<option value='0'>" . atktext('please_select_activity','weektimereg') . "</option>";
        foreach($phase["activities"] as $activity_id => $activity)
        {
          $myactselector .= "<option value='$activity_id'>$activity</option>";
        }
        $myactselector .= "</select>";

        $activity_selectors[] = $myactselector;
      }

      $myphaseselector .= "</select>";

      $phase_selectors[] = $myphaseselector;
    }
    $project_selector .= "</select>";

    $ret = "";
    $ret .= '<form name="entryform" method="post" action="dispatch.php">';
    $ret .= session_form(SESSION_NESTED);
    $ret .= '<input type="hidden" name="atknodetype" value="weektimereg.weekhours"/>';
    $ret .= '<input type="hidden" name="atkaction" value="add_allowed_timereg"/>';
    $ret .= '<input type="hidden" name="startdate" value="'.$startdate.'"/>';
    $ret .= '<input type="hidden" name="enddate" value="'.$enddate.'"/>';
    $ret .= $project_selector . implode("\n",$phase_selectors) . implode("\n",$activity_selectors);
    $ret .= '<div id="weektimereg_by_team_submitbutton" style="display: none;"><input type="submit" class="btn_save" value="'.atktext('add').'"/></div>';
    $ret .= '</form>';

    return $ret;
  }

  function action_add_allowed_timereg()
  {
    // Get project/phase/activity id's
    $project_id = $this->m_postvars['weekhours_add_project_selector'];
    $phase_id = $this->m_postvars['weekhours_add_phase_selector_' . $project_id];
    $activity_id = $this->m_postvars['weekhours_add_activity_selector_' . $phase_id . "_" . $project_id];

    // Get weeknumber
    $weeknumber = date('YW',strtotime($this->m_postvars['startdate']));

    // Get userid
    $user = &getUser();
    $userid = $user['id'];

    // Add to databasetable
    $db = &atkGetDb();

    $new_id = $db->nextId('weektimereg_byteam_extra_allowed');
    $db->query("
        INSERT INTO weektimereg_byteam_extra_allowed
          (`id`,`userid`,`projectid`,`phaseid`,`activityid`,`week`)
        VALUES
          ('$new_id','$userid','$project_id','$phase_id','$activity_id','$weeknumber')
      ");

    $redir_to = session_url("",SESSION_BACK);
    header("Location: $redir_to");
    return "redirecting to $redir_to";
  }

  function getEntriesRecentArray($startdate, $enddate, $userid)
  {
    // first get all the data of the current week
    $query = "SELECT project.id as projectid, phase.id as phaseid, activity.id as activityid, activity.remarkrequired, phase.name as phasename, project.name as projectname, project.abbreviation as projectabbreviation, activity.name as activityname, hours.time, hours.activitydate, hours.id as hoursid, hours.remark as remark
                FROM project, phase, hours, phase_activity, activity
                WHERE project.id = phase.projectid
                AND phase.id = hours.phaseid
                AND phase.id = phase_activity.phaseid
                AND phase_activity.activityid = activity.id
                AND hours.activityid = activity.id
                AND hours.activitydate >= '$startdate' AND hours.activitydate <= '$enddate'
                AND hours.userid = '".$userid."'
                ORDER BY project.name, phase.name, activity.name, activitydate";

    $db = &atkGetDb();
    $rows = $db->getRows($query);
    return $rows;
  }

  function getBookableEntries($startdate, $enddate)
  {
    global $g_user;

    $project_selection = strtolower(atkconfig::get('weektimereg','project_selection'));

    $function_name = "getEntries".$project_selection."Array";

    $arr = array();
    if (method_exists($this, $function_name))
    {
      $arr = $this->$function_name($startdate, $enddate, $this->m_user);
    }
    else
    {
      atkdebug("weekhours::getBookableEntries call function not found: '$function_name'");
    }

    $db = &atkGetDb();
    $proj = array();
    $alreadyhad = Array();
    $j=0;

    if (!empty($arr))
    {
      for ($i=0;$i<count($arr);$i++)
      {
        if (!in_array($arr[$i]["projectname"].$arr[$i]["phasename"].$arr[$i]["activityname"], array_keys($alreadyhad)))
        {
          $alreadyhad[$arr[$i]["projectname"].$arr[$i]["phasename"].$arr[$i]["activityname"]] = $j;
          $proj[$j]["projectname"] = $arr[$i]["projectname"];
          $proj[$j]["projectabbreviation"] = $arr[$i]["projectabbreviation"];
          $proj[$j]["phasename"] = $arr[$i]["phasename"];
          $proj[$j]["activityname"] = $arr[$i]["activityname"];
          $proj[$j]["projectid"] = $arr[$i]["projectid"];
          $proj[$j]["phaseid"] = $arr[$i]["phaseid"];
          $proj[$j]["activityid"] = $arr[$i]["activityid"];
          $proj[$j]["remarkrequired"] = $arr[$i]["remarkrequired"];
          $proj[$j]["remark"] = $arr[$i]["remark"];

          for ($k=0;$k<7;$k++)
          {
            $date = date("Y-m-d", mktime(0,0,0,substr($startdate, 5, 2),substr($startdate, 8, 2)+$k,substr($startdate, 0, 4)));

            if (atkconfig::get('weektimereg','no_booking_outside_projectdates') == true && $project_selection == "byteam" && (strtotime($date) < strtotime($arr[$i]['project_startdate']) || strtotime($date) > strtotime($arr[$i]['project_enddate'])))
            {
              // skip (date exceeds projectdates)
              $proj[$j]["activitydate"][$date]["date"] = $date;
              $proj[$j]["activitydate"][$date]["time"] = 0;
              $proj[$j]["activitydate"][$date]["hoursid"] = "";
              $proj[$j]["activitydate"][$date]["readonly"] = true;
              continue;
            }

            $proj[$j]["activitydate"][$date]["date"] = $date;
            $proj[$j]["activitydate"][$date]["time"] = 0;
            $proj[$j]["activitydate"][$date]["hoursid"] = "";
          }
          if ($arr[$i]["activitydate"]!="")
          {
            $proj[$j]["activitydate"][$arr[$i]["activitydate"]]["time"] = $arr[$i]["time"];
            $proj[$j]["activitydate"][$arr[$i]["activitydate"]]["hoursid"] = $arr[$i]["hoursid"];
          }
          $j++;
        }
        else
        {
          $key = $alreadyhad[$arr[$i]["projectname"].$arr[$i]["phasename"].$arr[$i]["activityname"]];
          $proj[$key]["activitydate"][$arr[$i]["activitydate"]]["time"] += $arr[$i]["time"];
          if ($proj[$key]["activitydate"][$arr[$i]["activitydate"]]["hoursid"] != "")
          $proj[$key]["activitydate"][$arr[$i]["activitydate"]]["hoursid"] .= ",";
          $proj[$key]["activitydate"][$arr[$i]["activitydate"]]["hoursid"] .= $arr[$i]["hoursid"];
          if ($proj[$key]["remark"] == "")
          $proj[$key]["remark"] = $arr[$i]["remark"];
        }
      }
    }

    $query = "SELECT projectid
                FROM project_person
                WHERE personid='".$this->m_user."'";

    $arr = $db->getrows($query);

    $mem_only = "(";

    foreach ($arr as $key => $value)
    {
      $mem_only .= $value['projectid'].",";
    }

    $mem_only = substr($mem_only,0,(strlen($mem_only)-1)).")";

    $mem_only_clausule = (count($arr) > 0) ? ("OR project.id IN ". $mem_only) : "";

    $weekplanningphaselist = $this->getWeekPlanningPhaseList($this->m_user, $startdate);
    $weekplanningqueryclausule = ($weekplanningphaselist != "") ? ("OR phase.id IN (" . $weekplanningphaselist . ")") : "";
    $autoshowactiveprojects = (atkConfig::get("weektimereg", "autoshowactiveprojects", false)) ? "OR project.status='active'" : "";
    $query = "SELECT project.id as projectid, phase.id as phaseid, activity.id as activityid, activity.remarkrequired, phase.name as phasename, project.name as projectname, project.abbreviation as projectabbreviation, activity.name as activityname
                FROM project, phase, phase_activity, activity
                WHERE project.id = phase.projectid
                AND phase_activity.phaseid = phase.id
                AND phase_activity.activityid = activity.id
                AND
                (
                  (
                    (
                      project.alwaysvisibleintimereg = 1
                      $autoshowactiveprojects
                    )
                    AND (project.startdate IS NULL OR project.startdate < '$enddate')
                    AND (project.enddate IS NULL OR project.enddate >= '$startdate')
                    AND (phase.startdate IS NULL OR phase.startdate < '$enddate')
                    AND (phase.enddate IS NULL OR phase.enddate >= '$startdate')
                  )
                  $weekplanningqueryclausule
                )
                AND
                (
                  timereg_limit = 0
                  $mem_only_clausule
                )";

    // Do not limit the amount of always shown/active projects to 100 when project collapsing is enabled
    $limit = (atkConfig::get('weektimereg', 'collapseProjects', true)) ? 1000 : 100;
    $arr = $db->getrows($query, 0, $limit);

    //
    $recproj = array();
    for ($i=0;$i<count($arr);$i++)
    {
      if (!in_array($arr[$i]["projectname"].$arr[$i]["phasename"].$arr[$i]["activityname"], array_keys($alreadyhad)))
      {
        $alreadyhad[$arr[$i]["projectname"].$arr[$i]["phasename"].$arr[$i]["activityname"]] = $j;
        $recproj[$j]["projectname"] = $arr[$i]["projectname"];
        $recproj[$j]["projectabbreviation"] = $arr[$i]["projectabbreviation"];
        $recproj[$j]["phasename"] = $arr[$i]["phasename"];
        $recproj[$j]["activityname"] = $arr[$i]["activityname"];
        $recproj[$j]["projectid"] = $arr[$i]["projectid"];
        $recproj[$j]["phaseid"] = $arr[$i]["phaseid"];
        $recproj[$j]["activityid"] = $arr[$i]["activityid"];
        $recproj[$j]["remark"] = "";
        if ($this->m_hide_fields_with_remarkrequired)
        {
          $recproj[$j]["remarkrequired"] = $arr[$i]["remarkrequired"];
        }
        for ($k=0;$k<7;$k++)
        {
          $date = date("Y-m-d", mktime(0,0,0,substr($startdate, 5, 2),substr($startdate, 8, 2)+$k,substr($startdate, 0, 4)));
          $recproj[$j]["activitydate"][$date]["hoursid"] = "";
          $recproj[$j]["activitydate"][$date]["date"] = $date;
          $recproj[$j]["activitydate"][$date]["time"] = 0;
        }
        $j++;
      }
    }
    $proj = array_merge($proj, $recproj);

    $defaultsort = array(array('name' => 'projectabbreviation', 'dir' => 'ASC'), array('name' => 'projectname', 'dir' => 'ASC'), array('name' => 'phasename', 'dir' => 'ASC'), array('name' => 'activityname', 'dir' => 'ASC'));
    $sort = atkConfig::get('weektimereg','sort',$defaultsort);
    $this->sortRows($proj, $sort);

    return $proj;
  }

  function action_save()
  {
    global $g_user;

    $this->setLock();
    if ($this->m_lock!="")
    {
      $this->redirect();
      return;
    }

    for ($i=0;$i<count($this->m_postvars["act_input"]);$i++)
    {
      $rec = $this->m_postvars["act_input"][$i];

      $rec["input"] = trim($rec["input"]);
      $rec["orginput"] = trim($rec["orginput"]);

      // check if the current input differs from the original input
      if (($rec["orginput"] != $rec["input"]) || ($this->m_postvars["week_input"][$rec["week"]]["orgremark"] != $this->m_postvars["week_input"][$rec["week"]]["remark"]))
      {
        $time = split(':', $rec["input"]);
        $record["activitydate"]["day"] = substr($rec["date"], 8, 2);
        $record["activitydate"]["month"] = substr($rec["date"], 5, 2);
        $record["activitydate"]["year"] = substr($rec["date"], 0, 4);
        $record["userid"]["id"] = $g_user["id"];
        $record["phaseid"]["phaseid"] = $rec["phaseid"];
        $record["activityid"] = $rec["activityid"];
        $record["remark"] = $this->m_postvars["week_input"][$rec["week"]]["remark"];
        $record["time"] = $this->m_attribList["time"]->_string2minutes($time[0].":".$time[1]);
        $record["entrydate"]["day"] = date("d");
        $record["entrydate"]["month"] = date("m");
        $record["entrydate"]["year"] = date("Y");
        atkdebug("input differs");

        if ($rec["input"] == "" || $rec["input"] == "0")
        {
          if ($rec["hoursid"]!="")
          {
            atkdebug("remove old records");
            // remove old records
            $this->deleteDb("hours.id IN (".$rec["hoursid"].")");
          }
        }
        else
        {
          atkdebug("input is not empty");
          if ($rec["orginput"] == "")
          {
            atkdebug("original input is empty, add new record");
            $time = split(':', $rec["input"]);
            $record["id"] = NULL;
            // add new record to the database
            $this->addDb($record);
            $node = &getNode("timereg.hours");
            $node->notify("save", $record);
          }
          else
          {
            if (!stristr($rec["hoursid"], ","))
            {
              // update current record
              atkdebug("update current record");
              $time = split(':', $rec["input"]);
              $record["id"] = $rec["hoursid"];
              $record["atkprimkey"] = "hours.id='".$rec["hoursid"]."'";
              // add new record to the database
              $this->updateDb($record);
              $node = &getNode("timereg.hours");
              $node->notify("update", $record);
            }
            else
            {
              // remove old records and insert new record
              atkdebug("remove old records and insert new record");
              $this->deleteDb("hours.id IN (".$rec["hoursid"].")");
              $record["id"] = NULL;
              // add new record to the database
              $this->addDb($record);
              $node = &getNode("timereg.hours");
              $node->notify("save", $record);
            }
          }
        }
      }
    }

    if ($this->m_postvars['redirect_after_save'] != "")
    $location = $this->m_postvars['redirect_after_save'];
    else
    $location = $this->feedbackUrl("save", ACTION_SUCCESS);

    $this->redirect($location);
  }

  function isBillingViewable()
  {
    $securitymanager = &atkGetSecurityManager();
    return (moduleExists("billing") && $securitymanager->allowed("billing.expense", "admin"));
  }

  function add_billing_fields(&$datarow,$data)
  {
    if (!$this->isBillingViewable()) return;
    $datarow[] = $this->add_expenses_booked_boolean($data);
    $datarow[] = $this->add_billing_link($data);
  }

  function add_expenses_booked_boolean($data)
  {
    $projectid = $data['projectid'];
    $from_date = array_shift(array_keys($data['activitydate']));
    $to_date =  array_pop(array_keys($data['activitydate']));

    $billingNode = &getNode("billing.expense");
    $user = getUser();
    $userid = $user["id"];
    $checked = (0 < $billingNode->countDb("expensedate >= '$from_date' AND expensedate <= '$to_date' AND project_id.id = '$projectid' AND billing_expense.user_id='$userid'"));

    return "<div align='center'>" . ($checked ? atktext('yes') : atktext('no')) . "</div>";
  }

  function add_billing_link($data)
  {
    $page = &atkPage::getInstance();
    $page->register_script(moduleDir('weektimereg') . 'javascript/save_hours_on_billinglink.js');

    $projectid = $data['projectid'];
    $dates = array_keys($data['activitydate']);
    $link = dispatch_url("billing.expense","admin",array( "atkfilter"=>"project_id=$projectid",
    "viewdate"=>$dates[0],
    "weekview"=>1));
    $link = session_url($link,SESSION_NESTED);
    return "<a href=\"javascript:save_hours_on_billinglink('$link','" . atktext('confirm_savehoursfirst_billinglink','weektimereg') . "')\">" . atktext('add_billing_link_txt','weektimereg') . "</a>";
  }

  function add_billing_headers(&$datarow)
  {
    if (!$this->isBillingViewable()) return;
    $datarow[] = atktext('header_costs_booked','weektimereg');
    $datarow[] = atktext('header_to_billing_link','weektimereg');
  }

  function add_billing_footers(&$datarow)
  {
    if (!$this->isBillingViewable()) return;
    $datarow[] = '';
    $datarow[] = '';
  }

  /**
   * Custom weektimereg record comparison function
   *
   * @static This function will be called statically (so $this cannot be used inside this function)
   * @param array $a_aRow1 First record
   * @param array $a_aRow2 Second record
   * @param fieldnumer $a_lField Fieldnumber for which the records should be compared
   * @return int Number indicating which record should be listed first
   */
  function compare($a_aRow1, $a_aRow2, $a_lField = 0)
  {
    global $g_aaSortArray;

    $lCompareVal    = 0;

    if ($a_lField < count($g_aaSortArray))
    {
      $sSortFieldName    = $g_aaSortArray[$a_lField]['name'];
      $sSortFieldDir    = $g_aaSortArray[$a_lField]['dir'];

      $vValue1    = eval('return $a_aRow1["' . $sSortFieldName . '"];');
      $vValue1 = is_string($vValue1) ? strtolower($vValue1) : $vValue1;
      $vValue2    = eval('return $a_aRow2["' . $sSortFieldName . '"];');
      $vValue2 = is_string($vValue2) ? strtolower($vValue2) : $vValue2;

      if ($vValue1 == $vValue2)
      {
        $lCompareVal = weekhours::compare($a_aRow1, $a_aRow2, $a_lField + 1);
      }
      else
      {
        $lCompareVal = $vValue1 > $vValue2 ? 1 : -1;
        if (strtolower(substr($sSortFieldDir, 0, 4)) == 'desc')
        $lCompareVal = -$lCompareVal;
      }
    }

    return $lCompareVal;
  }

  /**
   * Custom weektimereg sorting function
   *
   * @param array $a_aaRows Array of weektimereg records
   * @param array $a_aaSortCriteria Array in format array(array('name' => 'field1', 'dir' => 'ASC'), array('name' => 'field2', 'dir' => 'DESC'), ...)
   */
  function sortRows(&$a_aaRows, $a_aaSortCriteria)
  {
    global $g_aaSortArray;

    $g_aaSortArray    = $a_aaSortCriteria;
    if (is_array($a_aaRows) && count($a_aaRows)>1)
    {
      usort($a_aaRows, array("weekhours", "compare"));
    }
  }
}

?>