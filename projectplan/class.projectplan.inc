<?php

  include_once(moduleDir("utils")."class.dateutil.inc");
  useattrib("atkdurationattribute");

  class projectplan extends atkNode
  {
    var $m_project = array();
    var $m_project_totals = array();
    var $m_phase_totals = array();
    
    function projectplan()
    {
      $this->atkNode("projectplan");
    }
    
    function action_plan()
    {
      global $g_layout, $g_sessionManager;
      
      /* @var layout $g_layout */
      //$g_layout->register_style(moduleDir("projectplan")."styles/planning.css");     
      
      $this->m_project = $this->_getProject($g_sessionManager->pageVar("selectedprojectid"));    

      $g_layout->ui_top(text("projectplan"));
      $g_layout->output('<div align="left">'.$this->info());
      $g_layout->output('<br><br>'.$this->planningPerPhase());
      $g_layout->output('<br><br>'.$this->summary());
      $g_layout->output('<br><br>'.$this->planningPerPeriod().'<br><br></div>');
      
      $g_layout->ui_bottom();
    }
    
      
    
   /**
    * Get all project teammembers that work on the current project 
    * with their contracts
    *
    * @return array Array with Teammembers and their contracts
    */
    function _getUsers()
    {
      global $g_db;
      static $result = array();      
      if (count($result)) return $result; // caching            
      
      $sql = "SELECT 
                p.id,
                p.lastname,
                p.firstname,
                p.functionlevel,
                uc.workingdays, 
                uc.uc_hours,
                uc.startdate,
                uc.enddate
              FROM 
                project_person pp,
                person p, 
                usercontract uc
              WHERE pp.projectid = ".$this->m_project["id"]."
                AND pp.personid = p.id
                AND p.id = uc.userid";
      
      $nrows = $g_db->getrows($sql);
      
      for($i=0, $_i=count($nrows);$i<$_i; $i++)
      {
        $personid = $nrows[$i]["id"];      
        $result[$personid]["name"] = $nrows[$i]["lastname"].", ".$nrows[$i]["firstname"];
        $result[$personid]["userid"]=$nrows[$i]["id"];
        $result[$personid]["functionlevel"]=$nrows[$i]["functionlevel"];
        $result[$personid]["contract"][] = array("workingdays"=>$nrows[$i]["workingdays"],
                                                 "hours"=>$nrows[$i]["uc_hours"],
                                                 "startdate"=>$nrows[$i]["startdate"],
                                                 "enddate"=>$nrows[$i]["enddate"]);
 
      }
      
      return $result;
    }
    
    
   /**
    * Generate the project info
    * @return string Project info text
    */
    function info()
    {
      $output = '<b>'.text("info", "projectplan").'</b><br><br>';
      $output.= '<table border="0"><tr><td class="table">'.text("project").': </td><td class="table">'.$this->m_project["name"].'</td></tr>';
      $output.= '<tr><td class="table">'.text("coordinator").': </td><td class="table">'.$this->m_project["coordinator"].'</td></tr>';
      $output.= '</table>';
      return $output;
    }
    
   /**
    * Generate the planning per phase
    * @return string Planning per phase
    */
    function planningPerPhase()
    {
      global $g_layout;      
      
      // Init project totals
      $this->m_project_totals['initial_amount'] = 0;
      $this->m_project_totals['current_amount'] = 0;
      $this->m_project_totals['planned_amount'] = 0;
      $this->m_project_totals['booked_amount'] = 0;

      $phases = $this->_getPhases();
      $output = '<b>'.text("planning_per_phase", "projectplan").'</b><br><br>';
      $output.= '<form name="editform" action="dispatch.php" method="post">';
      $output.= session_form(SESSION_NESTED);
      $output.= '<input type="hidden" name="atknodetype" value="'.$this->atknodetype().'">';
      $output.= '<input type="hidden" name="atkaction" value="update">';

      $output.= $g_layout->data_top();
      $output.= '<tr>'.$g_layout->ret_td_datatitle('', 'rowspan="2"');
      
      foreach($phases as $phase_id=>$phase_info)
      {
        $output.= $g_layout->ret_td_datatitle($phase_info['name'], 'colspan="4"');
      }
      $output.='</tr>';

      $output.= '<tr>';
      
      foreach($phases as $phase_id=>$phase_info)
      {
        $output.= $g_layout->ret_td_datatitle(text("initial_short", "projectplan"));
        $output.= $g_layout->ret_td_datatitle(text("current_short", "projectplan"));
        $output.= $g_layout->ret_td_datatitle(text("planned_short", "projectplan"));
        $output.= $g_layout->ret_td_datatitle(text("actual_short", "projectplan"));
      }
      $output.='</tr>';
  
      // And now the users      
      $users = array();
      foreach ($phases as $phase_id => $phaseinfo)
      {
        if(count($phaseinfo["emps"])>0)
        {
          foreach($phaseinfo["emps"] as $user_id=>$empinfo)
          { 
            if($user_id=="other")
            {
              $users[$user_id] = text("other");                     
            }
            else
            {                   
              $users[$user_id] = $empinfo["name"];                     
            }
          }
        }
      }            
      
      foreach($users as $user_id => $username)
      {        
        $output.= '<tr class="row1">';
        $output.= $g_layout->ret_td_datatitle($username);
        foreach($phases as $phase_id => $phaseinfo)
        {          
          if ($user_id!='other' && $this->allowed("edit"))
          {
            $current = $this->_getTimeEntry('phase['.$phase_id.']['.$user_id.']' ,$phaseinfo["emps"][$user_id]["total"]['current_time']);
            $class="row2";
          }
          else
          {
            $current = $this->minute_format($phaseinfo["emps"][$user_id]["total"]['current_time']);
            $class="row1";
          }
          $output.='<td class="table">'.$this->minute_format($phaseinfo["emps"][$user_id]["total"]['initial_time'].'</td>');
          $output.='<td class="table '.$class.'">'.$current.'</td>';
          $output.='<td class="table">'.$this->minute_format($phaseinfo["emps"][$user_id]['total']['planned_time']).'</td>';
          if($phaseinfo["emps"][$user_id]['total']['booked_time']>$phaseinfo["emps"][$user_id]['total']['planned_time'])
          {
            $class="errortext"; 
          }
          elseif($phaseinfo["emps"][$user_id]['total']['booked_time']<$phaseinfo["emps"][$user_id]['total']['planned_time'])
          {
            $class="oktext";
          }
          else
          {
            $class="normaltext";
          }
          $output.=$g_layout->ret_td($this->minute_format($phaseinfo["emps"][$user_id]['total']['booked_time']), 'class="'.$class.'"');          
          
        }
        $output.='</tr>';
      }

      // Totals 
      $output.='<tr>'.$g_layout->ret_td_datatitle();
      foreach($phases as $phase_id=>$phase_info)
      {
        $output.= $g_layout->ret_td_datatitle($this->amount_format($phase_info["total"]["initial_amount"]));
        $output.= '<td class="table" style="background-color: '.($this->m_phase_totals[$phase_id]["current_amount"]>$this->m_phase_totals[$phase_id]["initial_amount"]?COLOR_ERROR:COLOR_OK).'">'.$this->amount_format($this->m_phase_totals[$phase_id]["current_amount"]).'</td>';
        $output.= '<td class="table" style="background-color: '.($this->m_phase_totals[$phase_id]["planned_amount"]>$this->m_phase_totals[$phase_id]["current_amount"]?COLOR_ERROR:COLOR_OK).'">'.$this->amount_format($this->m_phase_totals[$phase_id]["planned_amount"]).'</td>';
        $output.= '<td class="table" style="background-color: '.($this->m_phase_totals[$phase_id]["booked_amount"]>$this->m_phase_totals[$phase_id]["planned_amount"]?COLOR_ERROR:COLOR_OK).'">'.$this->amount_format($this->m_phase_totals[$phase_id]["booked_amount"]).'</td>';
        $this->m_project_totals["initial_amount"]+=$this->m_phase_totals[$phase_id]["initial_amount"];
        $this->m_project_totals["current_amount"]+=$this->m_phase_totals[$phase_id]["current_amount"];
        $this->m_project_totals["planned_amount"]+=$this->m_phase_totals[$phase_id]["planned_amount"];
        $this->m_project_totals["booked_amount"]+=$this->m_phase_totals[$phase_id]["booked_amount"];
      }
      
      $output.='</tr>';
      $output.= $g_layout->data_bottom();
      return $output;
    }
    
   /**
    * Generate the project summary
    * @return string The project summary
    */ 
    function summary()
    {
      global $g_layout;
      $output = '<b>'.text("summary", "projectplan").'</b><br><br>';
      $output.= $g_layout->data_top();
      $output.= '<tr>'.$g_layout->ret_td_datatitle();
      $output.= $g_layout->ret_td_datatitle(text("quotation", "projectplan"));
      $output.= $g_layout->ret_td_datatitle(text("initial", "projectplan"));
      $output.= $g_layout->ret_td_datatitle(text("current", "projectplan"));
      $output.= $g_layout->ret_td_datatitle(text("planned", "projectplan"));
      $output.= $g_layout->ret_td_datatitle(text("actual", "projectplan"));
      $output.='</tr>';
      $output.='<tr class="row1"><td class="table">'.text("project_total").'</td>';
      $output.='<td class="table">'.$this->amount_format($this->m_project['fixed_price']).'</td>';
      $output.='<td class="table" style="background-color: '.($this->m_project_totals["initial_amount"]>$this->m_project["fixed_price"]?COLOR_ERROR:COLOR_OK).'">'.$this->amount_format($this->m_project_totals['initial_amount']).'</td>';      
      $output.='<td class="table" style="background-color: '.($this->m_project_totals["current_amount"]>$this->m_project_totals["initial_amount"]?COLOR_ERROR:COLOR_OK).'">'.$this->amount_format($this->m_project_totals['current_amount']).'</td>';      
      $output.='<td class="table" style="background-color: '.($this->m_project_totals["planned_amount"]>$this->m_project_totals["current_amount"]?COLOR_ERROR:COLOR_OK).'">'.$this->amount_format($this->m_project_totals['planned_amount']).'</td>';
      $output.='<td class="table" style="background-color: '.($this->m_project_totals["booked_amount"]>$this->m_project_totals["initial_amount"]?COLOR_ERROR:COLOR_OK).'">'.$this->amount_format($this->m_project_totals['booked_amount']).'</td>';
      $output.='</tr>';
      $output.= $g_layout->data_bottom();
      return $output;
    }
    
   /**
    * Generate planning per period 
    * @return planning per period
    */
    function planningPerPeriod()
    { 
      global $g_layout;
      
      $output = '<b>'.text("planning_per_period", "projectplan").'</b><br><br>';      
           
      $planperiods = $this->_getPlanPeriods();
      if (count($planperiods)==0)
      {
        $output.="Planning not possible (no start/enddates known)";
        return $output;
      }
      
      //$output.= '<form name="editform" action="dispatch.php" method="post">';
      //$output.= session_form(SESSION_NESTED);
      //$output.= '<input type="hidden" name="atknodetype" value="'.$this->atknodetype().'">';
      //$output.= '<input type="hidden" name="atkaction" value="update"';
      
      $phases = $this->_getPhases();                  
      
      $output.= $g_layout->data_top();
      
      // first header row, one spacer, plus a column per plan-period.
      $output.= '<tr>'.$g_layout->ret_td_datatitle('', 'rowspan="2"');
      
      foreach ($planperiods as $period)
      {
        $output.= $g_layout->ret_td_datatitle($this->_getPeriodLabel($period), 'colspan="3"');
      }
      $output.=$g_layout->ret_td_datatitle(text('left_to_plan', 'projectplan'), 'rowspan="2"');
      $output.=$g_layout->ret_td_datatitle(text('left_to_plan_total', 'projectplan'), 'rowspan="2"');
      
      $output.= '</tr>';
      
      // second header row, one spacer, plus 3 titles per period.
      $output.= '<tr>';
      
      foreach ($planperiods as $period)
      {        
        $output.= $g_layout->ret_td_datatitle(text("planned_short", "projectplan"));
        $output.= $g_layout->ret_td_datatitle(text("actual_short", "projectplan"));
        $output.= $g_layout->ret_td_datatitle(text("available_short", "projectplan"));
      }      
      $output.='</tr>';
      $leftover_total = 0;
      $left_over = array();
      $total_amounts = array();
      atk_var_dump($phases);
      // fase row loop
      foreach($phases as $phase_id=>$phase_info)
      {
        // Phase title row. 
        $output.='<tr>'.$g_layout->ret_td_datatitle($phase_info["name"]);               
      
        foreach ($planperiods as $period)
        {                  
          $output.= '<td class="row1">&nbsp;</td>';
          $output.= '<td class="row1">&nbsp;</td>';
          $output.= '<td class="row1">&nbsp;</td>';
        }              
        $output.=$g_layout->ret_td_datatitle();
        $output.=$g_layout->ret_td_datatitle();
        
                
        $output.='</tr>';                
        // rows per member
        if (is_array($phase_info["emps"]))
        {
          foreach ($phase_info["emps"] as $userid=>$emp_info)
          {
            $output.='<tr>'.$g_layout->ret_td_datatitle('&nbsp;&nbsp;'.$emp_info["name"], 'nowrap');                 
            foreach ($planperiods as $period)
            {                    
              $isPhaseEditable = $this->_isPhaseEditable($period, $phase_info["startdate"], $phase_info["enddate"]) && $userid!="other" && $this->allowed("edit");
              
              $planned = $phase_info["emps"][$userid][$period]["planned_time"];
              $total_amounts[$period]["planned_amount"]+=$phase_info["emps"][$userid][$period]["planned_amount"];
              $booked = $phase_info["emps"][$userid][$period]["booked_time"];
              $total_amounts[$period]["booked_amount"]+=$phase_info["emps"][$userid][$period]["booked_amount"];
              $available = $phase_info["emps"][$userid][$period]["available_time"];   
              $total_amounts[$period]["available_amount"]+=$phase_info["emps"][$userid][$period]["available_amount"];
              
              if (!$isPhaseEditable) 
              {
                $plan_edit = ($planned!=""?$this->minute_format($planned):"&nbsp;");
                $r=1;
                $align="left";
              }
              else
              {
                $r=2;
                $align="center";
                $plan_edit = $this->_getTimeEntry('plan['.$phase_id.']['.$userid.']['.$period.']', $planned);
              }
              
              $output.= '<td class="row'.$r.'" align="'.$align.'">'.$plan_edit.'</td>'; // planned
              $output.= '<td class="table row1">'.($booked!=""?$this->minute_format($booked):"&nbsp;").'</td>'; // actual
              //$output.= '<td class="table row1">'.($available!=""&&$isPhaseEditable?$this->minute_format($available):"&nbsp;").'</td>'; // available
              $output.= '<td class="table row1">'.($this->minute_format($available)).'</td>'; // available
            }              
            
            // leftover
            $output.=$g_layout->ret_td_datatitle($this->minute_format($phase_info["emps"][$userid]["total"]["leftover_time"]));
            if($left_over[$userid]["leftover_time_total"]=="")
            {
              $left_over[$userid]["leftover_time_total"]=$phase_info["emps"][$userid]["total"]["leftover_time"];
            }
            else 
            {
              $left_over[$userid]["leftover_time_total"]+=$phase_info["emps"][$userid]["total"]["leftover_time"];
            }
            $output.=$g_layout->ret_td_datatitle($this->minute_format($left_over[$userid]["leftover_time_total"]));
            
            $leftover_total+=$phase_info["emps"][$userid]["total"]["leftover_time"];
            $output.='</tr>';
          }
        }
        
        // Phase spacer row. 
        $output.='<tr class="row1"><td>&nbsp;</td>';               
        foreach ($planperiods as $period)
        {                
          $output.= '<td>&nbsp;</td>';
          $output.= '<td>&nbsp;</td>';
          $output.= '<td>&nbsp;</td>';
        }              
        $output.= $g_layout->ret_td_datatitle();
        $output.= $g_layout->ret_td_datatitle();
        $output.='</tr>';
      }
      
      // footer (totals)
      $output.= '<tr>'.$g_layout->ret_td_datatitle(text("total"));
      
      foreach ($planperiods as $period)
      {        
        $output.= $g_layout->ret_td_datatitle($this->amount_format($total_amounts[$period]["planned_amount"]));
        $output.= $g_layout->ret_td_datatitle($this->amount_format($total_amounts[$period]["booked_amount"]));
        $output.= $g_layout->ret_td_datatitle();
      }
      
      $output.= $g_layout->ret_td_datatitle($leftover_total);
      $output.= $g_layout->ret_td_datatitle();
      
      $output.='</tr>';
      
      $output.= $g_layout->data_bottom()."\n";
      
      $output.= '<br><input type="submit" value="'.text("refresh").'">';
      $output.= '</form>';
      return $output;      
    }        
    
    
   /**
    * Get project info
    * @param int $projectid Project id
    * @return array Project info
    */
    
    function _getProject($projectid)
    {
      global $g_db;
      
      $project = array();

      $sql = "SELECT DISTINCT
                     project.id, 
                     project.name,
                     project.fixed_price,
                     project.startdate as prj_startdate,
                     project.enddate as prj_enddate,
                     max(phase.enddate) as enddate, 
                     min(phase.startdate) as startdate,
                     COORDINATOR.lastname,
                     COORDINATOR.firstname,
                     CONTACT.company as organization_id
                FROM project,
                     phase
                     LEFT JOIN person COORDINATOR ON (COORDINATOR.id = project.coordinator)
                     LEFT JOIN project_person ON (project_person.projectid = project.id)
                     LEFT JOIN person CONTACT ON (CONTACT.id = project_person.projectid AND CONTACT.role='contact')
               WHERE project.id = phase.projectid
                 AND project.id = ".$projectid."
               GROUP BY project.id";
      $nrows = $g_db->getrows($sql);      
      
      $project["id"] = $nrows[0]["id"];
      $project["name"] = $nrows[0]["name"];
      
      $project["coordinator"] = $nrows[0]["lastname"].", ".$nrows[0]["firstname"];
      
      // if phases do not have dates set, use the project dates
      // and convert to 'yyyymmdd' format for use with dateutil package.
      $project["startdate"] = str_replace("-","",($this->_emptyDate($nrows[0]["startdate"])?$nrows[0]["prj_startdate"]:$nrows[0]["startdate"]));
      $project["enddate"] = str_replace("-","", ($this->_emptyDate($nrows[0]["enddate"])?$nrows[0]["prj_enddate"]:$nrows[0]["enddate"]));
      
      // organization id (used for rates)
      $project["organization_id"] = $nrows[0]["organization_id"];
      
      $project["fixed_price"] = $nrows[0]["fixed_price"];
      
      return $project;
    }
    
   /**
    * Get planned periods
    * @return array Planned periods
    */ 
    function _getPlanPeriods()
    {
      $result = array();
      
      $startdate = $this->m_project["startdate"];
      $enddate = $this->m_project["enddate"];            
      
      if ($startdate!="" && $enddate!="")
      {            
        $start = date("Ym", dateutil::str2stamp($startdate));
        $end = date("Ym", dateutil::str2stamp($enddate));                
      
        for ($walker = $start; $walker<= $end; $walker = $this->_nextMonth($walker))
        {
          $result[] = $walker;
        }
      }
      
      return $result;
    }
    
    /**
     * Get phases of the project
     * @return array Structure containing all data in format:
     * $result[$phaseid]["name"]
     *                  ["startdate"]
     *                  ["enddate"]
     *                  ["total"]["initial_time"]
     *                           ["initial_amount"]
     *                           ["current_time"]
     *                           ["current_amount"]
     *                           ["planned_time"]
     *                           ["planned_amount"]
     *                           ["booked_time"]
     *                           ["booked_amount"]
     *                  ["emps"][$employee_id]["name"]
     *                                        ["total"]["initial_time"]
     *                                                 ["initial_amount"]
     *                                                 ["current_time"]
     *                                                 ["current_amount"]
     *                                                 ["planned_time"]
     *                                                 ["planned_amount"]
     *                                                 ["booked_time"]
     *                                                 ["booked_amount"]
     *                                                 ["leftover_time"]
     *                                        [$yyyymm]["planned_time"]
     *                                                 ["planned_amount"]
     *                                                 ["booked_time"]
     *                                                 ["booked_amount"]
     *                                                 ["available_time"]
     *                                                 ["available_amount"]
     */
    function _getPhases()
    {      
      static $result = array();      
      if (count($result)) return $result; // caching
      
      $pid = $this->m_project["id"];
      $organization_id = $this->m_project["organization_id"];
      
      global $g_db;
      
      $users = $this->_getUsers();
      $planned_userids = array_keys($users);
      $planned_periods = $this->_getPlanPeriods();
      
      $phaserecs = $g_db->getrows("SELECT 
                                     phase.startdate,
                                     phase.enddate,
                                     phase.id,
                                     phase.name,
                                     phase.current_planning,
                                     phase.initial_planning
                                  FROM 
                                     phase
                                  WHERE 
                                     projectid = $pid");            
      
      foreach($phaserecs as $rec)
      {
        $id = $rec["id"];
        $result[$id]["name"] = $rec["name"];
        $result[$id]["startdate"] = str_replace("-","",$rec["startdate"]);
        $result[$id]["enddate"] = str_replace("-", "", $rec["enddate"]);
        
        // Load the users
        foreach($users as $userid=>$userinfo)
        {
          $result[$id]["emps"][$userid]["name"] = $userinfo["name"];
          // Initialize available minutes
          foreach($planned_periods as $period)
          {
             $result[$id]["emps"][$userid][$period]["available_time"]=$this->_getAvailableHours($userid,$period,$id)*60;
          }
          
        }
        
                
        // Load the initial userplanning per phase
        $phaseplanrecs = $g_db->getrows("SELECT initial_planning, current_planning, personid FROM project_phaseplanning WHERE phaseid = ".$id);
        foreach ($phaseplanrecs as $phaseplanrec)
        {
          $result[$id]["emps"][$phaseplanrec["personid"]]["total"]["initial_time"]+= $phaseplanrec["initial_planning"];
          $result[$id]["emps"][$phaseplanrec["personid"]]["total"]["current_time"]+= $phaseplanrec["current_planning"];
          $result[$id]["emps"][$phaseplanrec["personid"]]["total"]["leftover_time"]+= $phaseplanrec["current_planning"];
          
          $result[$id]["total"]["initial_time"]+= $phaseplanrec["initial_planning"];          
          
          $rate = $this->_getRate($phaseplanrec["personid"], substr($result[$id]["startdate"],0,6), '', $pid, $organization_id);
          $result[$id]["emps"][$phaseplanrec["personid"]]["total"]["initial_amount"]+= (($phaseplanrec["initial_planning"]/60)*$rate);
          $result[$id]["emps"][$phaseplanrec["personid"]]["total"]["current_amount"]+= (($phaseplanrec["current_planning"]/60)*$rate);
          $result[$id]["total"]["initial_amount"]+= (($phaseplanrec["initial_planning"]/60)*$rate);
          $result[$id]["total"]["current_amount"]+= (($phaseplanrec["current_planning"]/60)*$rate);
          
          $this->m_phase_totals[$id]["initial_amount"] += (($phaseplanrec["initial_planning"]/60)*$rate);
          $this->m_phase_totals[$id]["current_amount"] += (($phaseplanrec["current_planning"]/60)*$rate);
        }
        
        // Load the planitems.
        $itemrecs = $g_db->getrows("SELECT 
                                      projectplan_planitem.userid, month, planned, rate
                                    FROM 
                                      projectplan_planitem,
                                      person
                                      LEFT JOIN billing_rate ON (
                                                                     (startdate IS NULL OR concat(month,'01') > date_format(startdate,'%Y%m%d'))
                                                                 AND (enddate IS NULL OR concat(month,'01') < date_format(enddate,'%Y%m%d'))
                                                                 AND (user_id IS NULL OR user_id=0 OR user_id = projectplan_planitem.userid)
                                                                 AND (functionlevel_id IS NULL OR functionlevel_id=0 OR functionlevel_id = person.functionlevel)
                                                                 AND (project_id IS NULL OR project_id=0 OR project_id=$pid)
                                                                 AND (customer_id IS NULL OR customer_id=0 OR customer_id='$organization_id')
                                                                )
                                    WHERE
                                      person.id = projectplan_planitem.userid                                      
                                      AND phaseid='".$id."'
                                    ORDER BY
                                      userid, month, priority DESC");
        $prevhash = "";
        //var_dump($itemrecs);
        foreach ($itemrecs as $itemrec)
        {
          $hash = $itemrec["userid"].$itemrec["month"];
          if ($hash!=$prevhash) // otherwise it's a second rate for the same phase, which we should ignore
          {
            $userid = $itemrec["userid"]; $month = $itemrec["month"]; 
            $result[$id]["emps"][$userid][$month]["planned_time"] = $itemrec["planned"];
            $result[$id]["emps"][$userid]["total"]["planned_time"] += $itemrec["planned"];
            $result[$id]["emps"][$userid]["total"]["leftover_time"] -= $itemrec["planned"];
            
            $result[$id]["emps"][$userid][$month]["planned_amount"] = ($itemrec["planned"]/60)*$itemrec["rate"];
            $result[$id]["emps"][$userid]["total"]["planned_amount"] += ($itemrec["planned"]/60)*$itemrec["rate"];
            
            $result[$id]["total"]["planned_time"] += $itemrec["planned"];
            $result[$id]["total"]["planned_amount"] += ($itemrec["planned"]/60)*$itemrec["rate"];
            
            $this->m_phase_totals[$id]["planned_amount"] += ($itemrec["planned"]/60)*$itemrec["rate"];
            
            //$result[$id]["emps"][$userid][$month]["available_time"]-=$itemrec["planned"];
            $prevhash = $hash;
          }          
        }
        
        // Load the users
        foreach($users as $userid=>$userinfo)
        {
          $result[$id]["emps"][$userid]["name"] = $userinfo["name"];
          // Initialize available hours
          foreach($planned_periods as $period)
          {
             $rate = $this->_getRate($userid, $period, '', $pid, $organization_id);
             $result[$id]["emps"][$userid][$period]["available_amount"]=(($result[$id]["emps"][$userid][$period]["available_time"]/60)*$rate);
          }
          
        }
        // load the actuals
        $hourrecs = $g_db->getrows("SELECT 
                                      sum(time) as time, 
                                      hours.userid, 
                                      date_format(activitydate, '%Y%m') as month, 
                                      activityid
                                    FROM
                                      hours
                                    WHERE 
                                      phaseid='$id' 
                                    GROUP BY month, userid, phaseid, activityid");       
        
        foreach ($hourrecs as $hourrec)
        {
          $rate = $this->_getRate($hourrec["userid"], $hourrec["month"], $hourrec["activityid"], $pid, $organization_id);   
                 
          
          if (!in_array($hourrec["userid"], $planned_userids))
          {            
            $hourrec["userid"] = "other";
          }
          if (!in_array($hourrec["month"], $planned_periods))
          {
            if ($hourrec["month"]<$planned_periods[0])
            {
              $hourrec["month"] = "pre";
            }
            else
            {
              $hourrec["month"] = "post";
            }
          }
          $result[$id]["emps"][$hourrec["userid"]][$hourrec["month"]]["booked_time"]+= $hourrec["time"]; 
          $result[$id]["emps"][$hourrec["userid"]]["total"]["booked_time"] += $hourrec["time"];
          $result[$id]["emps"][$hourrec["userid"]][$hourrec["month"]]["booked_amount"]+= (($hourrec["time"]/60) * $rate); 
          $result[$id]["emps"][$hourrec["userid"]]["total"]["booked_amount"] += (($hourrec["time"]/60) * $rate);        
          $result[$id]["total"]["booked_time"]+=$hourrec["time"]; 
          $result[$id]["total"]["booked_amount"]+=($hourrec["time"]/60)*$rate; 
          $this->m_phase_totals[$id]["booked_amount"]+=($hourrec["time"]/60)*$rate; 
            
        }
      }     
        
      return $result;
    }
    
   /**
    * Calculate the next month
    * @param string $yyyymm year(4)+month(2)
    * @return string $yyymm + 1 month
    */
    function _nextMonth($yyyymm)
    {
      $year = substr($yyyymm, 0, 4);
      $month = substr($yyyymm, 4, 2);
      if ($month==12) return ($year+1)."01";
      return $year.sprintf("%02d",$month+1);
    }
    
   /**
    * Check for empty date
    * @param string $str Date
    * @return bool Empty date true/false
    */
    function _emptyDate($str)
    {
      return ($str==NULL||$str==""||$str=="0000-00-00");
    }
   /**
    * Is phase editable
    * @param string $requested_period Requested period
    * @param string $phase_startdate Phase startdate
    * @param string $phase_enddate Phase enddate
    * @return bool Phase editable true/false
    */
    function _isPhaseEditable($requested_period, $phase_startdate, $phase_enddate)
    { 
      return (substr($phase_startdate, 0, 6) <= $requested_period
              && substr($phase_enddate, 0, 6) >= $requested_period);
    }
    
   /**
    * Action update
    */
    function action_update()
    { 
      $planitem = &getNode("projectplan.planitem");     
      $planitem->storeItems($this->m_postvars["plan"]);            
      
      $phaseplanning = &getNode("project.phaseplanning");
      $phaseplanning->storePhases($this->m_postvars["phase"]);
      
      $this->redirect();
    }      
    
    
   /**
    * Get period label
    * @param string $yyyymm Period
    * @return string period label
    */
    function _getPeriodLabel($yyyymm)
    {
      return text(dateutil::monthname(substr($yyyymm,4,2)))." ".substr($yyyymm, 0, 4);
    }       
    
   /**
    * Get available hours of a employee
    * @param string $userid User-id
    * @param string $month Month
    * @param int $phaseid Phase id
    * @return int available hours
    */
    function _getAvailableHours($userid,$month,$phaseid)
    {
      $contractHours = $this->_getAvailableContractHours($userid,$month);
      $plannedHours = $this->_getPlannedHoursPerUser($userid,$month,$phaseid)/60;            
      return $contractHours-$plannedHours; 
    }
    
   /**
    * Get planned hours per user
    * @param string $userid User-id
    * @param string $month Month
    * @param int $phaseid Phase id
    * @return int Planned hours
    */
    function _getPlannedHoursPerUser($userid,$month,$phaseid)
    {
      global $g_db;
      $itemrec = $g_db->getrows("SELECT 
                                      sum(planned) as planned
                                    FROM 
                                      projectplan_planitem
                                    WHERE userid='$userid'
                                      AND month = '$month'");
//                                      AND phaseid!='".$phaseid."'");
      return $itemrec[0]["planned"]; 
    }
   /**
    * Get available contracthours for a user
    * @param string $userid User-id
    * @param string $month Month (yyyymm)
    * @return int Contracthours 
    */
    function _getAvailableContractHours($userid,$month)
    {
      $users = $this->_getUsers();
      $user_contracts = $users[$userid]["contract"];
      
      $hours = 0;
      
      for($i=1;$i<31;$i++)
      {
        $walk_date = $month.(strlen($i)==1?"0".$i:$i);
        // loop true the contracts to find a valid contract
        for($j=0;$j<count($user_contracts);$j++)
        {
          if($walk_date>=str_replace("-","",$user_contracts[$j]["startdate"]) &&
             ($walk_date<=str_replace("-","",$user_contracts[$j]["enddate"]) || $user_contracts[$j]["enddate"]=="") &&
             strpos($user_contracts[$j]["workingdays"],date("w",mktime(12,0,0,substr($month,4,2),$i,substr($month,0,4))))!==false
             )
             {
               $totalWorkingDays = count(explode('|',$user_contracts[$j]["workingdays"]));
               $hours+=($user_contracts[$j]["hours"]/$totalWorkingDays);
               break;
             }            
        }
      }
      atkdebug("Total Workinghours in $month : ".$hours);
      return $hours;
    }
   
   /**
    * Get rates
    * @param string $userid User-id
    * @param string $month Month
    * @param int $activityid Activity-id
    * @param int $projectid Project-id
    * @param int $organization_id Organization-id
    * @return array User Rates
    */
    function _getRate($userid, $month, $activityid, $projectid, $organization_id)
    {     
      static $rates = array();
      
      if ($rates[$userid][$month][$activityid][$projectid][$organization_id]=="")
      {
        global $g_db;
        $date = $month."01";
        
        $level = $this->_getLevel($userid);
        
        $raterecs = $g_db->getrows("SELECT rate FROM
                                      billing_rate
                                    WHERE
                                           (startdate IS NULL OR '$date' > date_format(startdate, '%Y%m%d'))
                                       AND (enddate IS NULL OR '$date' < date_format(enddate, '%Y%m%d'))
                                       AND (user_id IS NULL OR user_id=0 OR user_id = $userid) ".($activityid!=''?
                                       "AND (activity_id IS NULL OR activity_id = 0 OR activity_id = '$activityid')":"")."
                                       AND (functionlevel_id IS NULL OR functionlevel_id=0 OR functionlevel_id = '$level')
                                       AND (project_id IS NULL OR project_id=0 OR project_id=$projectid)
                                       AND (customer_id IS NULL OR customer_id=0 OR customer_id='$organization_id')
                                    ORDER BY priority DESC");
        atkdebug($raterecs[0]["rate"]);
        
        // the first is the rate with the highest priority.
        $rates[$userid][$month][$activityid][$projectid][$organization_id] = $raterecs[0]["rate"];
      }
      return $rates[$userid][$month][$activityid][$projectid][$organization_id];
    }
    
   /**
    * Get user functionlevel
    * @param string $userid User-id
    * @return int Level
    */
    function _getLevel($userid)
    {
      $users = $this->_getUsers();      
      return $users[$userid]["functionlevel"];
    }
    
    function amount_format($amount)
    {
      return number_format($amount, 2, ".", ",");
    }
    
    function minute_format($minutes)
    {
      //if ($minutes<=0) return "";
      
      $rec = array('tmp'=>$minutes);
      $tmp = &new atkDurationAttribute('tmp', "1m", "1000h", AF_DURATION_STRING, 5);
      return $tmp->display($rec);
    }
    
    function _getTimeEntry($name, $minutes)
    {
      $rec = array($name=>$minutes);
      $tmp = &new atkDurationAttribute($name, "1m", "1000h", AF_DURATION_STRING, 5);
      return $tmp->edit($rec);
    }
    
  }

?>
