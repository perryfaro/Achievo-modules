<?php

  $modifiers[] = "project.phase_activity";

  define("LC_HOURS", 1); // Only time registrations will affect the amount of leave taken
  define("LC_REQUESTS", 2); // Only approved requests will affect the amount of leave taken
  define("LC_HOURSANDREQUESTS", LC_HOURS|LC_REQUESTS); // Past time registrations and future approved requests will be combined when calculating the leave taken

  class mod_holidayreport extends achievoModule
  {
    function getNodes()
    {
      registerNode("holidayreport.correction", array("admin", "add", "edit", "delete"));
      registerNode("holidayreport.rule", array("admin", "add", "edit"));
      registerNode("holidayreport.holidayreport", array("report", "view_all"));
      registerNode("holidayreport.holidayrequest", array("admin", "add", "edit", "delete", "review", "manager"));
    }

    function getMenuItems()
    {
      $this->menuitem("holidayreports",dispatch_url("holidayreport.employeeholiday_report", "report"),"main",array("holidayreport.holidayreport", "report", "holidayreport.holidayrequest", "admin"),650);
      $this->menuitem("holidayoverview_employee", dispatch_url("holidayreport.employeeholiday_report", "report"), "holidayreports", array("holidayreport.holidayreport", "report"));
      $this->menuitem("holidayoverview", dispatch_url("holidayreport.holidayreport", "report"), "holidayreports", array("holidayreport.holidayreport", "view_all"));
      $this->menuitem("-", "", "holidayreports");
      $this->menuitem("holidayrequest", dispatch_url("holidayreport.holidayrequest", "admin"), "holidayreports", array("holidayreport.holidayrequest", "admin"));
      $this->menuitem("holidayrequest_review", dispatch_url("holidayreport.holidayrequest_review", "admin"), "holidayreports", array("holidayreport.holidayrequest", "review"));
      $this->menuitem("-", "", "holidayreports");
      $this->menuitem("correction_admin", dispatch_url("holidayreport.correction", "admin"), "holidayreports", array("holidayreport.correction", "admin"));
      $this->menuitem("leave_type_admin", dispatch_url("holidayreport.leave_type", "admin"), "holidayreports", array("holidayreport.rule", "admin"));
      $this->menuitem("rule_admin", dispatch_url("holidayreport.rule", "admin"), "holidayreports", array("holidayreport.rule", "admin"));
      $this->menuitem("-", "", "holidayreports");
    }

    function project_phase_activity_modifier(&$node)
    {
      useattrib("atkboolattribute");
      $node->add(new atkBoolAttribute("holidayreport_isholiday"));
    }

    function getDescription()
    {
      return "Holiday reports and administration.";
    }

    function getPimItems()
    {
      $items = array();
      $items["holidayreport_myrequests"] = "getMyRequests";
      $items["holidayreport_reviews"] = "getReviewItems";
      return $items;
    }

    function getMyRequests()
    {
      include_once(moduleDir("holidayreport")."pim.inc");
      return phr_myRequests();
    }

    function getReviewItems()
    {
      include_once(moduleDir("holidayreport")."pim.inc");
      return phr_getReviewItems();
    }


    //*************************************************************//
    //***** Functions are used by the weekreport module ******//
    //*************************************************************//

    /**
     * Function is called through the atkModuleHarvester
     *
     * @return an array with columnnames
     */
    function weekreport_getAdditionalColumns()
    {
      return array("holidayhours");
    }

    /**
     * Function can be used to expand the existing collectData query in the weekreport
     * module
     *
     * @param atkQuery $query
     */
    function weekreport_addToQuery(&$query)
    {
      /* @var $query atkQuery */
      $query->addField("holidayreport_isholiday AS isholiday","","phase_activity");
      $query->addJoin("phase_activity","", "phase_activity.activityid = hours.activityid AND phase_activity.phaseid=hours.phaseid",true);

      if(moduleExists("sick_leave"))
      {
        $query->addField("count_as AS count_as","","activity");
        $query->addJoin("activity","", "activity.id = hours.activityid",true);
      }
    }

    /**
     * Function determines if an hours-value in the record should be added to the
     * column given by columnname.
     *
     * @param array $record one row of data
     * @return boolean TRUE if value should be added to column, FALSE if not.
     */
    function weekreport_addToColumn($record, $columnname)
    {
      //check if the given record is suitable for use in the given column.
      //We use a switch, in the future we may add more columns.
      switch($columnname)
      {
        case "holidayhours":
          if(moduleExists("sick_leave"))
          {
            if(!isset($record["isholiday"]) && !isset($record["count_as"]))      return false;
            return (($record["isholiday"] == '1') || ($record["count_as"] == '2'));
          }
          else
          {
            if(!isset($record["isholiday"]))      return false;
            return ($record["isholiday"] == '1');
          }
          break;
        default:
          return false;
      }
    }
  }

  function getEmployeeData($userid, $birthdate, $selectedyear, $contracts, $types, $rules, $employer_id)
  {
    $db = &atkGetDb();

    $rldata = array();
    // get correction for this employee for this year
    $sql = "SELECT * FROM holidayreport_correction WHERE type='correction' AND year = '$selectedyear' AND userid = ".$userid;
    $res = $db->getRows($sql);
    if (count($res) > 0)
    {
      $subtotal = 0;
      for ($i=0;$i<count($res);$i++)
      {
        $rldata["corrections"][$i]["description"] = $res[$i]["description"];
        $rldata["corrections"][$i]["correction"] = $res[$i]["correction"];
        $subtotal = (double)$subtotal + (double)$res[$i]["correction"];
      }
      $rldata["total_correction"] = round($subtotal,2);
    }
    else
      $rldata["total_correction"] = "0.00";

    // get balance of last year for this employee
    $sql = "SELECT * FROM holidayreport_correction WHERE type = 'balance' AND year = '".($selectedyear-1)."' AND userid = ".$userid;
    $resyear = $db->getRows($sql);

    if (count($resyear) > 0)
      $rldata["lastyear"] = $resyear[0]["correction"];
    else
      $rldata["lastyear"] = NULL; // unavailable

    // calculate the age of the employee
    if ($birthdate != "0000-00-00" && $birthdate != NULL)
      $employeeage = $selectedyear - substr($birthdate,0,4);
    else
      $employeeage = 0;
    $total = 0;
    $rldata["types"] = array();
    $monthbirthdate = substr($birthdate,5,2);
    if (substr($monthbirthdate,0,1) == "0") $monthbirthdate = substr($monthbirthdate,1);

    for ($j=0;$j<count($types);$j++)
    {
      // calculate the number of hours using the rules defined
      // if there are no age rules defined for this leave type it is always 0,00
      if (count($rules[$types[$j]["description"]]) == 0)
        $rldata["types"][$types[$j]["description"]] = "0,00";
      else
      {
        $subtotal = 0;
        $flag = false;//are current rule is acceptable for current employee - thanks Saskia!
        for ($k=0;$k<count($rules[$types[$j]["description"]]);$k++)
        {
          $rule = $rules[$types[$j]["description"]][$k];
          if (count($rule) && $rule['employer_id'] == $employer_id)
          {
            $flag = true;
            
            if ($rule["type"] == "month" && $employeeage == $rule["age_max"])
            {
              // month type only applies when the given maximum age will be reached in the selected year
              // this type only applies for the months in which the maximum age is not reached.
              for ($l=1;$l<$monthbirthdate;$l++)
              {
                $monthtime = adodb_mktime(0, 0, 0, $l, 1, $selectedyear);
                $subtotal += (double) getFullContractFactor($contracts, date("Ymd", $monthtime), date("Ymt", $monthtime)) * $rule["value"];
              }
            }
            elseif ($rule["type"] == "monthafter" && $employeeage == $rule["age_max"])
            {
              // monthafter type only applies when the given maximum age will be reached in the selected year
              // this type only applies for the months in which the maximum age is reached until the end of the year.
              for ($l=$monthbirthdate;$l<=12;$l++)
              {

                $monthtime = adodb_mktime(0, 0, 0, $l, 1, $selectedyear);
                $subtotal += (double) getFullContractFactor($contracts, date("Ymd", $monthtime), date("Ymt", $monthtime)) * $rule["value"];
              }
            }
            elseif ($rule["type"] == "year" && ($employeeage >= $rule["age_min"] || empty($rule["age_min"])) && ($employeeage <= $rule["age_max"] || empty($rule["age_max"])))
            {
              // year type looks at the age that will be reached in the selected year
              // if that age is between the given minimum and maximum age then this rule applies
              $subtotal = (double) getFullContractFactor($contracts, $selectedyear.'0101', $selectedyear.'1231') * $rule["value"];
            }
          }
        }
        if($flag)
        {
          $total = (double)$total + (double)$subtotal;
          $rldata["types"][$types[$j]["description"]] = number_format($subtotal,2,",","");
        }
      }
    }
    $total_unrounded = (double)$total + (double)$rldata["total_correction"] + ((count($resyear)>0)?(double)$resyear[0]["correction"]:0);
    $total = (double)ceil($total) + (double)$rldata["total_correction"] + ((count($resyear)>0)?(double)$resyear[0]["correction"]:0);
    $rldata["total_leave"] = $total;
    $rldata["unrounded"] = round($total_unrounded,2);

    // Get future holidayrequests
    $leavesum = getLeaveSum($userid, $selectedyear);
    if ($leavesum != NULL)
    {
      $rldata["leave_taken"] = $leavesum/60;
      if (atkConfig::get("holidayreport", "round", atkconfig("holidayreport_round", true)))
        $rldata["balance"] = round((double)$total - (double)($leavesum/60),2);
      else
        $rldata["balance"] = round((double)$total_unrounded - (double)($leavesum/60),2);
    }
    else
    {
      $rldata["leave_taken"] = "0.00";
      if (atkConfig::get("holidayreport", "round", atkconfig("holidayreport_round", true)))
        $rldata["balance"] = $total;
      else
        $rldata["balance"] = $total_unrounded;
    }
    return $rldata;
  }

  function getLeaveSum($userid, $selectedyear)
  {
    // Calculate the
    $currentyear = date("Y");
    $datebeginyear = "$selectedyear-01-01";
    $dateendyear = "$selectedyear-12-31";
    $dateuntiltoday = ($currentyear != $selectedyear) ? $dateendyear : date("Y-m-d");
    $datefuture = ($currentyear != $selectedyear) ? $datebeginyear : date("Y-m-d");
    $leavecalcsetting = getLeaveCalculationSetting();

    // Start with an empty leave
    $leavesum = 0;

    // Calculate the leave that is already withdrawn from the total of leave hours for this year
    if (hasFlag($leavecalcsetting, LC_HOURS))
    {
      $leavesum += getHours($userid, $datebeginyear, hasFlag($leavecalcsetting, LC_REQUESTS) ? $dateuntiltoday : $dateendyear);
    }

    // Calculate the leave from approved holidayrequests in the future
    if (hasFlag($leavecalcsetting, LC_REQUESTS))
    {
      $leavesum += getApprovedRequests($userid, hasFlag($leavecalcsetting, LC_HOURS) ? $datefuture : $datebeginyear, $dateendyear);
    }

    // Return the leavesum for the given year
    return $leavesum;
  }

  function getLeaveCalculationSetting()
  {
    return atkconfig::get('holidayreport','leavecalculation', LC_HOURS);
  }

  function getHours($userid, $begindate, $enddate)
  {
    $db = &atkGetDb();
    $flag = moduleExists("sick_leave");
    
    
    $sql = "SELECT sum(time) as leavesum
              FROM hours, phase_activity".($flag ? ", activity " : "")."
              WHERE hours.activityid = phase_activity.activityid
                AND hours.phaseid = phase_activity.phaseid
                ".($flag ?
                " AND hours.activityid=activity.id 
                    AND (phase_activity.holidayreport_isholiday = 1 OR activity.count_as=2) " :
                " AND phase_activity.holidayreport_isholiday = 1 ")."
                AND userid = '$userid'
                AND activitydate >= '$begindate' AND activitydate <= '$enddate'";
    $records = $db->getRows($sql);
    $leavesum = (count($records) > 0) ? $records[0]["leavesum"] : 0;
    return $leavesum;
  }

  function dashedstr2stamp($dashedstr)
  {
    return adodb_mktime(12,0,0,substr($dashedstr,5,2), substr($dashedstr,8,2), substr($dashedstr,0,4));
  }

  function arr2stamp($arr)
  {
    return adodb_mktime(12,0,0,$arr["month"],$arr["day"],$arr["year"]);
  }

  function stamp2dashedstr($stamp)
  {
    return date("Y-m-d", $stamp);
  }


  function getApprovedRequests($userid, $begindate, $enddate)
  {
    require_once(atkConfig("atkroot")."atk/utils/adodb-time.inc.php");

    // Calculate the begin and enddate stamps
    $begindatestamp = dashedstr2stamp($begindate);
    $enddatestamp = dashedstr2stamp($enddate);

    // Get approved holidayrequests that intersect with the given period
    $holidayrequestnode = &atkGetNode("holidayreport.holidayrequest");
    $holidayrequestcondition = "status='granted'";
    $holidayrequests = $holidayrequestnode->selectDb($holidayrequestcondition, "", "", "", array("fromdate", "todate"));

    // Get the days on which this employee works and his contracthours per day
    $usercontractsnode = &atkGetNode("employee.usercontracts");

    // Get a list of holidays as lookup table
    $holidaynode = &atkGetNode("calendar.holidays");
    $holidays = $holidaynode->selectDb("");
    $holidayslookup = array();
    foreach($holidays as $holiday)
    {
      $holidayslookup[] = $holiday["date"];
    }

    // Start with $approvedleavehours = 0
    $approvedleavehours = 0;

    // Loop through the holidayrequests array
    foreach ($holidayrequests as $holidayrequest)
    {

      // Get the period that falls within both $begindate&$enddate AND within the holidayrequest's begin date & end date
      $requestbegindatestamp = arr2stamp($holidayrequest["fromdate"]);
      if ($begindatestamp > $requestbegindatestamp) $requestbegindatestamp = $begindatestamp;
      $requestenddatestamp = arr2stamp($holidayrequest["todate"]);
      if ($requestenddatestamp > $enddatestamp) $requestenddatestamp = $enddatestamp;

      // Loop through all dates within the intersection
      for($i=$requestbegindatestamp; $i<=$requestenddatestamp; $i+=86400)
      {

        // If it is a workday according to the persons contract AND it is not a holiday, add a day to the persons approvedleavedays
        $day = stamp2dashedstr($i);
        $weekday = date("w", $i);

        // Only add this day to the leave, if it isn't a holiday
        if (!in_array($day, $holidayslookup))
        {
          // Select only user contracts for the current employee and in which this day is a workday
          $whereclause = "usercontract.userid = '$userid' AND workingdays LIKE '%$weekday%' AND startdate <= '$day' AND (enddate >= '$day' OR enddate IS NULL)";
          $usercontracts = $usercontractsnode->selectDb($whereclause, "", "", "", array("uc_hours", "workingdays"));
          foreach($usercontracts as $usercontract)
          {
            $approvedleavehours += $usercontract["uc_hours"] / count($usercontract["workingdays"]);
          }
        }
        else
        {
          atkdebug("Holiday :) LEAVE THE LEAVE ALONE");
        }
      }

    }

    // Return the number of approved leave days multiplied by the amount of hours per day
    return $approvedleavehours*60;

  }

  function getTypes()
  {
    $db = &atkGetDb();
    $sql = "SELECT * FROM holidayreport_leave_type ORDER BY description";
    return $db->getRows($sql);
  }

  function getRules($typeid, $selectedyear)
  {
    $db = &atkGetDb();
    $sql = "SELECT * FROM holidayreport_rule WHERE beginyear <= '".$selectedyear."' AND (endyear IS NULL OR endyear = '' OR endyear >= '".$selectedyear."') AND typeid = ".$typeid;
    return $db->getRows($sql);
  }

  function getContracts($userid, $selectedyear)
  {
    $db = &atkGetDb();
    $sql = "SELECT * FROM usercontract WHERE startdate <= '".$selectedyear."-12-31' AND (enddate >= '".$selectedyear."-01-01' OR enddate IS NULL) AND userid = ".$userid;
    return $db->getRows($sql);
  }

  function getNumberofDays($month, $year)
  {
    $days = date("t", mktime(0,0,0,$month,1,$year));
    return floor($days/2);
  }
  
  /**
   * Calculate contract factor for fulltime week
   *
   * @param array   $contracts array with user contracts
   * @param string  $startdate period startday
   * @param string  $enddate   period enddate 
   * @return double Factor that indicates the perunage of one fulltime contract this employee actually is contracted
   */
  function getFullContractFactor($contracts, $startdate, $enddate)
  {
    // Start with an empty total
    $total = 0;
    atkimport("modules.utils.dateutil");
    
    // If we have found contract(s), we will loop through the list of it and start accumulating the total
    // amount of hours
    for ($i=0,$_i=count($contracts); $i<$_i; $i++)
    {
       // Convert the contract start and enddate to "yyyymmdd" format so we can use it in calculations
       $contractstartdate = date("Ymd", adodb_mktime(0, 0, 0, substr($contracts[$i]["startdate"], 5, 2), substr($contracts[$i]["startdate"], 8, 2), substr($contracts[$i]["startdate"], 0, 4)));
       $contractenddate = is_null($contracts[$i]["enddate"]) ? $enddate : date("Ymd", adodb_mktime(0, 0, 0, substr($contracts[$i]["enddate"], 5, 2), substr($contracts[$i]["enddate"], 8, 2), substr($contracts[$i]["enddate"], 0, 4)));

       // If the contract doesn't intersect the start/enddate, ignore it
       if ($contractenddate < $startdate || $contractstartdate > $enddate)
       {
         continue;
       }

       // The contractend should be changed to the end of the period if it spans over it and the
       // contractstart should be changed to the start of the period if it starts before the periodstart
       if ($contractenddate > $enddate) $contractenddate = $enddate;
       if ($contractstartdate < $startdate) $contractstartdate = $startdate;

       // Calculate the amount of calendar days within the period
       $numdays = dateutil::date_diff($contractstartdate,$contractenddate)+1;

       // Calculate the amount of full working days according to 40 nhours week
       $total += $numdays * $contracts[$i]["uc_hours"] / atkConfig::get("holidayreport", "fulltime", atkConfig("holidayreport_fulltime", 40));
    }

    $total /= dateutil::date_diff($startdate, $enddate) + 1;

    // If no contracts are found, we just return 0
    return $total;
  }

?>