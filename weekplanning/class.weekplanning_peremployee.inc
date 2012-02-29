<?php
  /**
   * weekplanning_peremployee node class file
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
   * @version $Revision: 3687 $
   * $Id: class.weekplanning_peremployee.inc 3687 2006-09-22 13:22:58Z harrie $
   */

  // Load the parent class node file
  atkimport("module.weekplanning.weekplanning");

  /**
   * The node class for weekplanning_peremployee.
   *
   * @author ivo <ivo@ibuildings.nl>
   * @author guido <guido@ibuildings.nl>
   * @package achievo_modules
   * @subpackage weekplanning
   */
  class weekplanning_peremployee extends weekplanning
  {
    /**
     * weekplanning_peremployee constructor
     */
    function weekplanning_peremployee()
    {
      $this->weekplanning("weekplanning_peremployee");
      $this->setSecurityAlias("weekplanning.weekplanning");
    }

    function getFilterRecord()
    {
      static $s_record = NULL;
      global $g_sessionManager;

      if ($s_record==NULL)
      {
        $s_record = array();
        $s_record["functionlevel"] = $g_sessionManager->pageVar("functionlevel");
        $s_record["employee"] = $g_sessionManager->pageVar("employee");
        $s_record["department"] = $g_sessionManager->pageVar("department");
        $s_record["supervisor"] = $g_sessionManager->pageVar("supervisor");
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
      $attrs["functionlevel"] = &new atkManyToOneRelation("functionlevel", "employee.functionlevel");

      useattrib("employee.myemployeesattribute");
      $attrs["employee"] = &new myEmployeesAttribute("employee", "weekplanning.weekplanning.any_user");

      $attrs["department"] = &new atkManyToOneRelation("department", "employee.department");

      $attrs["supervisor"] = &new myEmployeesAttribute("supervisor", "weekplanning.weekplanning.any_user");
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

      $output.= atktext("functionlevel").": ".$attrs["functionlevel"]->edit($record)." ";
      $output.= atktext("employee").": ".$attrs["employee"]->edit($record)." ";
      $output.= atktext("department").": ".$attrs["department"]->edit($record)." ";
      if ($this->hasAnyUserPrivilege())
      {
        $output.= atktext("supervisor").": ".$attrs["supervisor"]->edit($record)." ";
      }
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

      $emps = $this->getPlannedEmployees();

      if (!count($emps)) return atktext("noemployeesmatchingcriteria", "weekplanning");

      $weeks = $this->getWeeksBetween();

      if (!count($weeks)) return atktext("invaliddateperiod", "weekplanning");

      $result = '<form name="entryform" action="dispatch.php" method="post">';
      $result.= session_form();
      $result.= '<input type="hidden" name="atkaction" value="update">';

      foreach ($emps as $emp)
      {
        $result.= $this->getGrid($emp, $weeks);
        $result.= "<br/>";
      }

      if (!$this->isPrinter())
      {
        $result.= '<input type="submit" value="'.atktext("save").'"> ';

        $params = $this->getFilterParams();
        $url = dispatch_url("weekplanning.weekplanning_perproject", "admin", $params);
        $result.= atkButton(atktext("per_project", "weekplanning"), $url, SESSION_REPLACE, true);

        $result.= '<br/><br/>'.$this->getConflicts().'<br/>';
      }

      $result.= '</form>';

      return $result;
    }


    function getGrid($employee, $weeks)
    {
      $empnode = &getNode("employee.employee");
      $result = "<b>".$empnode->descriptor($employee)."</b><br/><br/>";

      $data[0] = array(atktext("project"), atktext("phase"));
      foreach($weeks as $week)
      {
        $date = dateUtil::str2arr($week["startdate"]);
        $data[0][] = $this->weekFormat($week["week"])."<br/>(".atktext(dateUtil::short_monthname($date["month"]))." ".$date["day"].")";
      }

      $contract = array("<b>" . atktext("usercontract") . "</b>", "");

      $filterrecord = $this->getFilterRecord();
     
      if ($filterrecord['estimate'] == 1)
        $data[0][] = "<b>" . atktext('estimate','weekplanning') . "</b>"; 
      
      $contracts = $this->getContracts($employee["id"], dateUtil::arr2str($filterrecord["from"]), dateUtil::arr2str($filterrecord["to"]));

      $planning = $this->getPlanningData($employee["id"], dateUtil::arr2str($filterrecord["from"]), dateUtil::arr2str($filterrecord["to"]));

      foreach($weeks as $week)
      {
        $contract[] = time_format($contracts[$week["week"]]);
      }
      
      $data[] = $contract;

      // Get this employees phases..
      $strstart = dateUtil::startOfWeek(dateUtil::arr2str($filterrecord["from"]));
      $strend = dateUtil::endOfWeek(dateUtil::arr2str($filterrecord["to"]));
      $phases = $this->getPhases($employee["id"], $strstart, $strend);

      $phasenode = &getNode("project.phase");

      $totaldata = array();
      $totalestimate = array();
      foreach($weeks as $week)
      {
        $totaldata[$week["startdate"]] = 0;
      }

      foreach($phases as $phase)
      {
        $row = array();
        $row[0] = $phase["projectid"]["name"];
        $row[1] = $phasenode->descriptor($phase);

        $phaseplanning = atkArrayNvl($planning, $phase['id'], array());

        $valueperweek = array();
        
        foreach($weeks as $week)
        {
          $inputname = "data[".$employee["id"]."_".$week["startdate"]."_".$phase["id"]."]";
          $oldinputname = "old".$inputname;

          $value = atkArrayNvl($phaseplanning, $week["startdate"], "");

          $totaldata[$week["startdate"]]+=$value;
          $valueperweek[$week["startdate"]]=$value;

          if ($this->isPrinter())
          {
            $row[] = time_format($value*60);
          }
          else
          {
            $row[] = '<input type="hidden" name="'.$oldinputname.'" value="'.$value.'"><input type="text" name="'.$inputname.'" size="3" style="width: 90%;" value="'.$value.'">';
          }
        }
        
        if ($filterrecord['estimate'] == 1)
          $row[] = $this->getEstimate($phase,$weeks,$valueperweek,$filterrecord,$totalestimate,$employee['id']);

        $data[] = $row;
      }

      // Totals
      $totals = array("<b>" . atktext("total") . "</b>", "");
      foreach($weeks as $week)
      {
        $total = time_format($totaldata[$week["startdate"]]*60, true);

        if ($totaldata[$week["startdate"]]*60 > $contracts[$week["week"]] && (!atkconfig::get('weekplanning','no_contract_no_conflict') || $contracts[$week["week"]] > 0))
        {
          $total = '<span style="color: red;">'.$total.'</span>';
        }
        $totals[] = $total;
      }
      $data[] = $totals;

      // Get the holidays row
      $holidaynode = &atkGetNode("calendar.holidays");
      $holidays = array("<b>" . atktext("holiday", "weekplanning") . "</b>", "");
      $holidaydata = array();
      foreach($weeks as $week)
      {
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
        $holidaydata[$week["startdate"]] = $holidayhours;
        $holidays[] = time_format($holidayhours*60, true);
      }
      $data[] = $holidays;
      
      // Get the estimate rows
      if ($filterrecord['estimate'] == 1)
      {
        $estimate_total_row = array("<b>".atktext('estimate','weekplanning')."</b>","");
        foreach($weeks as $week)
          $estimate_total_row[] = ($totalestimate[$week['startdate']] ? $totalestimate[$week['startdate']] : 0);
          
        $estimate_total_row[] = array_sum($totalestimate);
          
        $data[] = $estimate_total_row;
      }

      // Get the timeleft row
      $left = array(atktext("left", "weekplanning"), "");
      $table = &atknew("atk.utils.atktablerenderer");
      $i=2;
      foreach($weeks as $week)
      {
        $timeleft = $contracts[$week["week"]]-($totaldata[$week["startdate"]]*60)-($holidaydata[$week["startdate"]]*60);
        $fmttimeleft = time_format($timeleft, true);
        if ($holidaydata[$week["startdate"]]>0)
          $table->setColClass($i, "orange");
        if ($timeleft<0 && (!atkconfig::get('weekplanning','no_contract_no_conflict') || $contracts[$week["week"]] > 0))
        {
          $fmttimeleft = '<span style="color: red;">'.$fmttimeleft.'</span>';
        }

        $left[] = $fmttimeleft;
        $i++;
      }
      $data[] = $left;

      $result.= $table->render($data, TBL_DATA|TBL_FOOTER, "recordlist");

      return $result;
    }

    function getPlanningData($employee_id, $strfrom, $strto)
    {
      $recs = $this->selectDb("employeeid='".$employee_id."' AND monday BETWEEN '$strfrom' AND '$strto'", "", "", "", array("phaseid", "monday", "minutes"));

      // create lookup array
      $result = array();

      //atk_var_dump($recs, 'recs');
      foreach($recs as $rec)
      {
        $result[$rec["phaseid"]][dateUtil::arr2str($rec["monday"])] = $rec["minutes"]/60;
      }
      return $result;
    }

    function getPhases($employee_id, $strstart, $strend)
    {

      $filterrecord = $this->getFilterRecord();

      $phasenode = &getNode("project.phase");

      $prjfilter = "";

      // Get a list of project ids this employee is member of
      $project_personemployeenode = &getNode("project.project_personemployee");
      $projectids = $project_personemployeenode->selectDb("project_person.personid='$employee_id'", "", "", "", array("projectid", "personid"));
      $projectidarray = array();
      foreach ($projectids as $projectid)
        if (!empty($projectid["projectid"]["id"]))
          $projectidarray[] = $projectid["projectid"]["id"];

      // If any projects found for the given employee, then add a filter
      // limiting the result to be (a subset of) these projects
      if (count($projectidarray) > 0)
      {
        $prjfilter .= " AND projectid.id IN (".implode(",", $projectidarray).")";
      }

      if (!$this->hasAnyProjectPrivilege())
      {
        $user = getUser();
        $prjfilter .= " AND projectid.coordinator=".$user["id"];
      }
      else if ($filterrecord["coordinator"]!=NULL)
      {
        $prjfilter .= " AND projectid.coordinator=".$filterrecord["coordinator"]["id"];
      }

      return $phasenode->selectDb("(phase.startdate IS NULL OR phase.startdate<'".$strend."')
                                      AND
                                     (phase.enddate IS NULL OR phase.enddate>'".$strstart."')
                                     AND
                                     (phase.status='active' AND projectid.status='active')
                                     $prjfilter", "projectid.name ASC, phase.startdate ASC", "", "", array("id", "name", "projectid"));
    }

    function getPlannedEmployees()
    {
      $empnode = &getNode("employee.employee");
      $empnode->addFilter("status='active'");
      $filterrecord = &$this->getFilterRecord();

      $user = getUser();

      if ($filterrecord["functionlevel"]!=NULL)
      {
        $empnode->addFilter("functionlevel=".$filterrecord["functionlevel"]["id"]);
      }

      if ($filterrecord["employee"]!=NULL)
      {
        $empnode->addFilter("id=".$filterrecord["employee"]["id"]);
      }
      else if (!$this->hasAnyUserPrivilege()) // only show your own employees
      {
        $empnode->addFilter("supervisor=".$user["id"]);
      }

      if ($filterrecord["department"]!=NULL)
      {
        $empnode->addFilter("department=".$filterrecord["department"]["id"]);
      }

      if ($filterrecord["supervisor"]!=NULL)
      {
        $empnode->addFilter("supervisor=".$filterrecord["supervisor"]["id"]);
      }

      return $empnode->selectDb("", "", "", "", array("id", "lastname", "firstname"));
    }
  }
?>
