<?php
  /**
   * weekplanning_perproject node class file
   *
   * @package achievo_modules
   * @subpackage weekplanning
   *
   * @author ivo <ivo@ibuildings.nl>
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2005 Ibuildings.nl BV
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision: 3686 $
   * $Id: class.weekplanning_perproject.inc 3686 2006-09-13 13:17:21Z harrie $
   */

  // Load the parent class node file
  require_once("class.weekplanning.inc");

  /**
   * The node class for weekplanning_perproject.
   *
   * @author ivo <ivo@ibuildings.nl>
   * @author guido <guido@ibuildings.nl>
   * @package achievo_modules
   * @subpackage weekplanning
   */
  class weekplanning_perproject extends weekplanning
  {
    /**
     * weekplanning_perproject constructor
     */
    function weekplanning_perproject()
    {
      $this->weekplanning("weekplanning_perproject");
      $this->setSecurityAlias("weekplanning.weekplanning");
    }

    function getFilterRecord()
    {
      static $s_record = NULL;
      global $g_sessionManager;

      if ($s_record==NULL)
      {
        $s_record = array();
        $s_record["phaseid"] = $g_sessionManager->pageVar("phaseid");
        $s_record["coordinator"] = $g_sessionManager->pageVar("coordinator");
        $s_record["from"] = $g_sessionManager->pageVar("from");
        $s_record["to"] = $g_sessionManager->pageVar("to");
        $s_record["estimate"] = $g_sessionManager->pageVar("estimate");

        // convert attribute html values to internal values
        $attrs = &$this->getFilterAttrs();
        foreach (array_keys($attrs) as $attribname)
        {
          $p_attrib = &$attrs[$attribname];
          $s_record[$attribname] = &$p_attrib->fetchValue($s_record);
        }


        $go = $g_sessionManager->pageVar("go");
        if ($go!=1)
        {
          // initial loading. We'll put the from/to fields to reasonable defaults.

          $s_record["from"] = dateUtil::str2arr(dateUtil::startOfWeek(date("Ymd")));

          // next month
          $next_month = mktime(12,0,0,date("m")+1, date("d"), date("Y"));
          $s_record["to"] = dateUtil::str2arr(dateUtil::endOfWeek(date("Ymd", $next_month)));
        }
      }
      return $s_record;
    }

    function &getFilterAttrs()
    {
      $attrs = array();

      useattrib("employee.myemployeesattribute");
      useattrib("project.projectphaseattrib");

      $attrs["phaseid"] = &new atkProjectPhaseAttribute("phaseid");
      $attrs["phaseid"]->m_nested = false;
      $attrs["coordinator"] = &new myEmployeesAttribute("coordinator", "weekplanning.weekplanning.any_project");

      $attrs["from"] = &new atkDateAttribute("from");
      $attrs["to"] = &new atkDateAttribute("to");
      
      $attrs["estimate"] = &new atkBoolAttribute("estimate");

      return $attrs;
    }

    function getFilterBar()
    {
      $record = $this->getFilterRecord();

      $attrs = &$this->getFilterAttrs();

      $output = '<form action="dispatch.php" method="get">';
      $output.= session_form();
      $output.= '<input type="hidden" name="go" value="1">'; //trigger to prevent loading the first time
      $output.= atktext("phase").": ".$attrs["phaseid"]->edit($record)." ";

      if ($this->hasAnyProjectPrivilege())
      {
        $output.= atktext("coordinator").": ".$attrs["coordinator"]->edit($record)." ";
      }

      $output.= "<br/>";

      $output.= atktext("period_from", "weekplanning")." ".$attrs["from"]->edit($record)." ";
      $output.= atktext("to", "weekplanning")." ".$attrs["to"]->edit($record);
      
      $output.= "<br/>";
      $output.= $this->text("estimate")." ".$attrs["estimate"]->edit($record);

      $output.= '<br/> <input type="submit" value="'.atktext("refresh").'">';

      $output.= '</form>';
      return $output;
    }

    function getPlanning()
    {
      $result = "";

      $phases = $this->getPlannedPhases();

      if (!count($phases)) return atktext("noprojectsmatchingcriteria", "weekplanning");

      $weeks = $this->getWeeksBetween();

      if (!count($weeks)) return atktext("invaliddateperiod", "weekplanning");

      $result = '<form name="entryform" action="dispatch.php" method="post">';
      $result.= session_form();
      $result.= '<input type="hidden" name="atkaction" value="update">';

      foreach ($phases as $phase)
      {
        $result.= $this->getGrid($phase, $weeks);
        $result.= "<br/>";
      }

      if (!$this->isPrinter())
      {
        $result.= '<input type="submit" value="'.atktext("save").'"> ';

        $params = $this->getFilterParams();
        $url = dispatch_url("weekplanning.weekplanning_peremployee", "admin", $params);
        $result.= atkButton(atktext("per_employee", "weekplanning"), $url, SESSION_REPLACE, true);

        $result.= '<br/><br/>'.$this->getConflicts().'</br>';
      }

      $result.= '</form>';

      return $result;
    }

    function getGrid($phase, $weeks)
    {
      $table = &atknew("atk.utils.atktablerenderer");
      $phasenode = &getNode("project.phase");
      $result = "<b>".$phase["projectid"]["name"]." / ".$phasenode->descriptor($phase)."</b><br/><br/>";

      $filterrecord = $this->getFilterRecord();

      // Load the estimated, realised and planned amount of hours for this phase
      $result .= $this->getPhaseHourAmounts($phase);

      $data[0] = array(atktext("employee_week", "weekplanning"));
      foreach($weeks as $week)
      {
        $date = dateUtil::str2arr($week["startdate"]);
        $data[0][] = $this->weekFormat($week["week"])."<br/>(".atktext(dateUtil::short_monthname($date["month"]))." ".$date["day"].")";
      }

      $filterrecord = $this->getFilterRecord();

      $planning = $this->getPlanningData($phase["id"], dateUtil::arr2str($filterrecord["from"]), dateUtil::arr2str($filterrecord["to"]));

      // Get this phases employees..
      $employees = $this->getEmployees($phase["id"]);

      $empnode = &getNode("employee.employee");
      $holidaynode = &atkGetNode("calendar.holidays");

      $totaldata = array();
      $totalestimate = array();
      foreach($weeks as $week)
      {
        $totaldata[$week["startdate"]] = 0;
      }

      foreach($employees as $employee)
      {
        $rownumber = count($data);
        $row = array();
        $row[0] = $empnode->descriptor($employee);

        $contracts = $this->getContracts($employee["id"], dateUtil::arr2str($filterrecord["from"]), dateUtil::arr2str($filterrecord["to"]));

        $employeeplanning = atkArrayNvl($planning, $employee['id'], array());

        $valueperweek = array();
        
        foreach($weeks as $week)
        {
          $colnumber = count($row);
          $inputname = "data[".$employee["id"]."_".$week["startdate"]."_".$phase["id"]."]";
          $oldinputname = "old".$inputname;

          $value = atkArrayNvl($employeeplanning, $week["startdate"], "");

          $totaldata[$week["startdate"]]+=$value;
          $valueperweek[$week["startdate"]]=$value;

          if ($this->isPrinter())
          {
            $row[] = time_format($value*60);
          }
          else
          {
            $row[] = '<input type="hidden" name="'.$oldinputname.'" value="'.$value.'"><input type="text" name="'.$inputname.'" size="3" style="width: 90%;" value="'.$value.'">';

            // Get the number of holidays (feestdagen) in this week
            $holidaycount = $holidaynode->CountDb("holiday_date>='{$week["startdate"]}' AND holiday_date<='{$week["enddate"]}'");

            // get holiday requests
            if (moduleExists("holidayreport"))
            {
              $holidayrequestnode = &atkGetNode("holidayreport.holidayrequest");
              $holidayrequests = $holidayrequestnode->SelectDB("holidayreport_request.status<>'denied' AND holidayreport_request.userid='{$employee["id"]}'", "", "", "", array("fromdate","todate"));
              $weekstart =$week["startdate"];
              $weekend = $week["enddate"];
              foreach ($holidayrequests as $holidayrequest)
              {
                $holidayrequeststart = dateutil::arr2str($holidayrequest["fromdate"]);
                $holidayrequestend = dateutil::arr2str($holidayrequest["todate"]);
                $daysbeginholiday = dateutil::date_diff($holidayrequeststart, $weekend)+1;
                $daysendholiday = dateutil::date_diff($weekstart, $holidayrequestend)+1;
                $holidayduration = dateutil::date_diff($holidayrequeststart, $holidayrequestend)+1;
                if (($daysbeginholiday > 0) && ($daysendholiday > 0))
                {
                  $holidaycount += min($holidayduration, $daysbeginholiday, $daysendholiday, 7);
                }
              }
            }
            $holidayhours = min($contracts[$week["week"]]/60, $holidaycount*8);

            if ($holidayhours > 0)
              $table->setClass($rownumber, $colnumber, ($holidayhours < ($contracts[$week["week"]]/60)) ? "orange" : "red");
          }
        }

        if ($filterrecord['estimate'] == 1)
          $row[] = $this->getEstimate($phase,$weeks,$valueperweek,$filterrecord,$totalestimate,$employee['id']);
        
        $data[] = $row;
      }

      // Totals
      $totals = array("<b>" . atktext("total") . "</b>");
      foreach($weeks as $week)
      {
        $total = time_format($totaldata[$week["startdate"]]*60, true);

        $totals[] = $total;
      }
      $data[] = $totals;
      
      // Get the estimate rows
      if ($filterrecord['estimate'] == 1)
      {
        $estimate_total_row = array("<b>".atktext('estimate','weekplanning')."</b>");
        foreach($weeks as $week)
          $estimate_total_row[] = ($totalestimate[$week['startdate']] ? $totalestimate[$week['startdate']] : 0);
          
        $estimate_total_row[] = array_sum($totalestimate);
          
        $data[] = $estimate_total_row;
      }

      $result.= $table->render($data, TBL_DATA|TBL_FOOTER, "recordlist");

      $securitymanager = &atkGetSecurityManager();
      if ($securitymanager->allowed("project.project", "edit"))
      {
        $result.= "<br/>";
        $result.= href(dispatch_url("project.project", "edit", array("atkselector"=>"project.id={$phase["projectid"]["id"]}", "atktab"=>"planning")), atktext("edit", "atk")." ".atktext("project_members"));
      }
      $result.= "<br/><br/>";

      return $result;

    }

    function getPhaseHourAmounts($phase)
    {
      // Get a reference to a phase node instance
      $phasenode = &getNode("project.phase");

      // Start an empty array which will be used to store the phase hour amounts
      $amounts = array();

      // Loop through the initial_planning, current_planning and spend_hours attributes
      foreach(array("initial_planning", "current_planning", "spend_hours") as $item)
      {

        // Translate the attribute name
        $name = atktext($item, "project");

        // Get the attribute and use it to render the value of the current phase
        $attr = &$phasenode->getAttribute($item);
        $value = $attr->display($phase, "view");

        // Store the name and value in "name: value" format
        $amounts[] = sprintf("%s: %s", $name, $value);
      }

      // Concatenate all phase hour amounts with commas as glue
      $result = implode(", ", $amounts) . "<br />";

      // Calculate the total amount of planned hours:
      $db = &atkGetDb();
      $totalplannedresult = $db->getRows("SELECT SUM(minutes) as totalplanned FROM weekplanning_planning WHERE phaseid='{$phase["id"]}'");

      // Store the planned amounts in an array
      $plannedamounts = array();

      // Get a duration attribute so we can display the durations
      $timeattr = &new atkDurationAttribute("time");

      // Add the total planned hours to the plannedamounts array
      $totalplanned = $totalplannedresult[0]["totalplanned"];
      $plannedamounts[] = sprintf('%s: %s', atktext("total_planned", "weekplanning"), $timeattr->display(array("time"=>$totalplanned)));

      // Add the difference between the total planned hours and current planning to the plannedamounts array
      $planningdifference = abs($phase["current_planning"] - $totalplanned);
      if ($phase["current_planning"] >= $totalplanned)
      {
        $plannedamounts[] = sprintf('%s: <span style="color: green">%s</span>', atktext("yet_to_plan", "weekplanning"), $timeattr->display(array("time"=>$planningdifference)));
      }
      else
      {
        $plannedamounts[] = sprintf('%s: <span style="color: red">%s</span>', atktext("planned_too_much", "weekplanning"), $timeattr->display(array("time"=>$planningdifference)));
      }

      // Concatenate all planned hour amounts with commas as glue
      $result .= implode(", ", $plannedamounts) . "<br />";

      // Add a html line break at the end
      $result .= "<br />";

      // Return the result
      return $result;
    }

    function getPlanningData($phase_id, $strfrom, $strto)
    {
      $recs = $this->selectDb('phaseid='.$phase_id." AND monday BETWEEN '$strfrom' AND '$strto'", "", "", "", array("employeeid", "monday", "minutes"));

      // create lookup array
      $result = array();

      //atk_var_dump($recs, 'recs');
      foreach($recs as $rec)
      {
        $result[$rec["employeeid"]["id"]][dateUtil::arr2str($rec["monday"])] = $rec["minutes"]/60;
      }
      return $result;
    }

    function getEmployees($phase_id)
    {
      // Start with an empty filter
      $empfilter = "";

      // Get the project id of the given project phase
      $phasenode = &getNode("project.phase");
      $projectids = $phasenode->selectDb("phase.id='$phase_id'", "", "", "", array("projectid"));
      $projectid = $projectids[0]["projectid"]["id"];

      // Get a list of team member personids
      $project_personemployeenode = &getNode("project.project_personemployee");
      $personids = $project_personemployeenode->selectDb("project_person.projectid='$projectid'", "", "", "", array("personid"));
      $personidarray = array();
      foreach ($personids as $personid)
        $personidarray[] = $personid["personid"]["id"];

      // If any team members found for the given project, then add a filter
      // limiting the result to be (a subset of) the project team
      if (count($personidarray) > 0)
      {
        $empfilter .= " AND person.id IN (".implode(",", $personidarray).")";
      }

      // If no anyuser privilege available, then do not show employees the
      // current user isn't supervisor of
      if (!$this->hasAnyUserPrivilege())
      {
        $user = getUser();
        $empfilter .= " AND supervisor=".$user["id"];
      }

      $empnode = &getNode("employee.employee");
      return $empnode->selectDb("status='active' $empfilter", "person.lastname ASC, person.firstname ASC", "", "", array("id", "lastname", "firstname"));
    }

    function getPlannedPhases()
    {
      $phasenode = &getNode("project.phase");
      $phasenode->addFilter("phase.status='active'");
      $filterrecord = &$this->getFilterRecord();

      $user = getUser();

      if ($filterrecord["coordinator"]!=NULL)
      {
        $phasenode->addFilter("projectid.coordinator=".$filterrecord["coordinator"]["id"]);
      }
      else if (!$this->hasAnyProjectPrivilege()) // only show your own projects
      {
        $phasenode->addFilter("projectid.coordinator=".$user["id"]);
      }
      if (!empty($filterrecord["phaseid"]["phaseid"]))
      {
        $phasenode->addFilter("phase.id='".$filterrecord["phaseid"]["phaseid"]."'");
      }

      $strstart = dateUtil::startOfWeek(dateUtil::arr2str($filterrecord["from"]));
      $strend = dateUtil::endOfWeek(dateUtil::arr2str($filterrecord["to"]));

      return $phasenode->selectDb("(phase.startdate IS NULL OR phase.startdate<'".$strend."')
                                      AND
                                     (phase.enddate IS NULL OR phase.enddate>'".$strstart."')
                                     AND
                                      (projectid.status='active')", "projectid.name, phase.name", "", "", array("id", "name", "projectid", "initial_planning", "current_planning", "spend_hours"));
    }

  }
?>
