<?php

  include_once("achievotools.inc"); // for time_format method

  useattrib("atkdurationattribute");

  class employeeholiday_report extends atkNode
  {
    function employeeholiday_report()
    {
      $this->atkNode("employeeholiday_report");
      $this->add(new atkAttribute("activitydate"));
      $this->add(new atkAttribute("project"));
      $this->add(new atkAttribute("remark"));
      $this->add(new atkDurationAttribute("time", "1m", "", AF_TOTAL));

      $this->setSecurityAlias("holidayreport.holidayreport");
    }

    function getEmployee($user_id)
    {
      $db = &atkGetDb();
      $sql = "SELECT * FROM person WHERE id='$user_id'";
      $record = $db->getrows($sql);
      return $record[0];
    }

    function getHolidayHours($userid, $begindate, $enddate)
    {
      $db = &atkGetDb();
      $flag = moduleExists("sick_leave");
      
      $sql = "SELECT time, activitydate, remark, project.name as project
              FROM hours, phase_activity, phase, project ".($flag ? ", activity " : "")."
              WHERE hours.activityid = phase_activity.activityid
                AND hours.phaseid = phase_activity.phaseid
                ".($flag ?
                " AND hours.activityid=activity.id 
                    AND (phase_activity.holidayreport_isholiday = 1 OR activity.count_as=2) " :
                " AND phase_activity.holidayreport_isholiday = 1 ")."
                AND phase_activity.phaseid = phase.id
                AND phase.projectid = project.id
                AND userid = '$userid'
                AND activitydate >= '$begindate' AND activitydate <= '$enddate'
              ORDER BY activitydate";
      $records = $db->getRows($sql);
      for ($i=0, $_i=count($records); $i<$_i; $i++)
      {
        $records[$i]["time"] = $this->m_attribList["time"]->db2value($records[$i]);
      }
      return $records;
    }

    function get_employees($userid)
    {
      $db = &atkGetDb();

      $sql = "SELECT id, lastname,firstname,userid
              FROM person
              WHERE (role = 'employee' and status = 'active') OR id = '$userid'
              ORDER BY lastname";

      $records = $db->getrows($sql);

      for($i=0;$i<count($records);$i++)
      {
        if ($userid==$records[$i]["id"]) $selected="selected";
        else $selected="";
        $employee_code.='<OPTION VALUE="'.$records[$i]["id"].'" '.$selected.'>'.$records[$i]["lastname"].', '.$records[$i]["firstname"].'</OPTION>';
      }
      return $employee_code;
    }

    function action_report()
    {
      $user = atkGetUser();
      $securityManager = &atkGetSecurityManager();
      //$atkorderby = $_REQUEST["atkorderby"];
      $userid = $_REQUEST["userid"];
      if (!isset($userid)||$userid=="")
      {
        $userid=$user["id"];
      }

      $selectedyear = $this->m_postvars["year"];
      if ($selectedyear == "") $selectedyear = date("Y");

      $newcorrection = $_REQUEST["correction"];
      $db = &atkGetDb();
      if ($newcorrection!="")
      {
        // insert new correction
        $newid = $db->nextid("holidayreport_correction");
        $sql = "INSERT INTO holidayreport_correction (id, year, userid, correction, type, description) VALUES ('$newid', '$selectedyear', '$userid', '$newcorrection', 'correction', '".$_REQUEST["description"]."')";
        $db->query($sql);
      }

      $newbalance = $_REQUEST["balance"];
      if ($newbalance!="")
      {
        // check if there already exists an balance for last year
        $sql = "SELECT id FROM holidayreport_correction WHERE year = ".($selectedyear-1)." AND userid = '".$userid."' AND type = 'balance'";
        $res = $db->getRows($sql);
        if (count($res))
        {
          // update balance
          $sql = "UPDATE holidayreport_correction SET correction = '$newbalance' WHERE id = ".$res[0]["id"];
          $db->query($sql);
        }
        else
        {
          // insert new balance
          $newid = $db->nextid("holidayreport_correction");
          $sql = "INSERT INTO holidayreport_correction (id, year, userid, correction, type) VALUES ('$newid', '".($selectedyear-1)."', '$userid', '$newbalance', 'balance')";
          $db->query($sql);
        }
      }

      /** $outputType gives control over the kind of output:
       *        0 standard(onscreen in compliance with theme, default)
       *        1 printable (onscreen in a format that easily can be printed on paper))
       *             [was $printable]
       *        2 export ( to a file on local computer ex. achievo.csv)
       */
      $outputType = $_REQUEST["outputType"];
      if (!isset($outputType)||$outputType=="") $outputType=0;

      if (!isset($atkorderby)||$atkorderby=="") $atkorderby = "week";
      $view_all = $securityManager->allowed('holidayreport.holidayreport','view_all');

      $col = $this->m_postvars["col"];

      $page = &$this->getPage();
      $this->addStyle("style.css");
      $ui = &$this->getUI();
      $boxes = array();
      $dispatchLocation = getDispatchFile();
      
      if ($outputType==0)
      {

        $content = '<form action="'.$dispatchLocation.'" method="get" name="entryform">';

        $content.= session_form();

        $content.= '<input type="hidden" name="atknodetype" value="holidayreport.employeeholiday_report">';
        $content.= '<input type="hidden" name="atkaction" value="'.$this->m_action.'">';
        $content.= '<table border="0" cellspacing="0" cellpadding="2">';
        $content.= '<tr>';
        $content.= '<td colspan="2"><b>'.$this->text('setyearfilter').':</b></td>';
        $content.= '</tr><tr>';

        $content.= '</tr><tr>';
        $content.= '<td>'.$this->text("name").':</b> </td>';

        if($view_all)
        {
          $content.= '<td><SELECT name="userid">'.$this->get_employees($userid).'</SELECT></td>';
        }
        else
        {
          $userid = $user["id"];
          $content.= '<td>'.$user["name"].'<input type="hidden" name="userid" value="'.$user["id"].'"></td>';
        }

        $content.= '</tr><tr>';

        $content.= '<td>'.$this->text("year").': </td>';
        $day = date("d");
        $month = date("m");
        $year = date("Y");
        $content.= '<td><select name="year">';
        for ($i=0;$i<3;$i++)
        {
          $newyear = date("Y", mktime(0,0,0,$month,$day,$year-$i));
          $content.= '<option value="'.$newyear.'" '.($selectedyear==$newyear?"selected":"").'>'.$newyear;
        }
        $content.= '</select></td>';

        $content.= '</tr></table><br><input type="submit" value="'.atktext("refresh").'"></form>';

        $box = array(
          "title" => $this->text("parameters"),
          "content" => $content,
        );
        $boxedresult = $ui->renderBox($box);

        // Return the boxed result HTML
        $boxes[] = $boxedresult;


      }

      if ($userid!="" && $selectedyear!="") // If we haven't got a userid by now, don't display the report.
      {
        $rldata = array();

        // get leave types
        $types = getTypes();
        for ($i=0;$i<count($types);$i++)
        {
           // get the age rules
          $rules[$types[$i]["description"]] = getRules($types[$i]["id"], $selectedyear);
        }

        // get employeedata
        $employee = $this->getEmployee($userid);

        // get the contracthours of the employee of the selected period
        $contracts = getContracts($employee["id"], $selectedyear);

        if (count($contracts)>0)
        {
          $rldata = getEmployeeData($employee["id"], $employee["birthdate"], $selectedyear, $contracts, $types, $rules, $employee['employer_id']);
          $rldata["employeename"] = $employee["firstname"]." ".$employee["lastname"];
          $rldata["birthdate"] = $employee["birthdate"];
          $rldata["userid"] = $employee["id"];

          // sorting..
          $suppress = array();
          $rldata["year"] = $selectedyear;

          if ($outputType=="0")
          {
            $boxes[] = $this->viewPage($rldata);
          }
          else if ($outputType=="1")
          {
            //$layout->output($this->specialRecordList($rldata,"<tr>","<td>","</td>","</tr>", "0","",$suppress));
          }
          else if ($outputType=="2")
          {
            // special RecordList can export to file
            //$this->specialRecordList($rldata, "", '"', '";', "\r\n", "1", "",$suppress);
          }
        }
        else
        {
          $box = array(
            "title" => $this->text("employeeholiday_report")." ".$selectedyear,
            "content" => "<br>".$this->text("nocontract")."<br><br>",
          );
          $boxes[] = $ui->renderBox($box);
        }
      }
      $actionpage = $this->renderActionPage("report", $boxes);
      $page->addContent($actionpage);
    }

    /**
     * Creates an view (=readonly) page
     */
    function viewPage($record)
    {
      $securityManager = &atkGetSecurityManager();

      $content ='<br>';

      $content.='<div align="left"><table>';
      $content.='<tr><td class="table">'.$this->text("year").':</td><td class="table">'.$record["year"].'</td></tr>';
      $content.='<tr><td class="table">'.$this->text('reportdate').':</td><td class="table">'.date("d-m-Y").'</td></tr>';
      $content.='<tr><td class="table">'.$this->text("employee").':</td><td class="table">'.$record["employeename"].'</td></tr>';
      $content.='<tr><td class="table">'.$this->text("birthdate").':</td><td class="table">'.substr($record["birthdate"],8,2).'-'.substr($record["birthdate"],5,2).'-'.substr($record["birthdate"],0,4).'</td></tr>';
      $content.='</table></div>';

      $content.='<hr>';

      // overview
      $content.='<div align="left"><table width="70%" border="0" cellspacing="0" cellpadding="2"><tr>';
      $content.='<td width="50%" class="table"><b>'.$this->text("leave")."</b></td>";
      $content.='<td width="40%" class="table" align="right"><b>'.$this->text('hours').'</b></td>';
      $content.='<td width="10%" class="table">&nbsp;</td>';
      $content.='</tr><tr>';

      $url = dispatch_url("holidayreport.employeeholiday_report", "report", array("year"=>($record["year"]-1), "userid"=>$_REQUEST["userid"]));
      if ($securityManager->allowed('holidayreport.correction','edit'))
      {
        $url2 = dispatch_url("holidayreport.employeeholiday_report", "report", array("userid"=>$record["userid"], "year"=>$record["year"]));
        $content.='<td width="50%" class="table"><form action="'.$url2.'" method="post">'.$this->text("balance_previousyear").' ('.href($url, ($record["year"]-1)).')'.'</td>';
        $content.='<td width="40%" class="table" align="right"><input type="text" size="5" name="balance" value="'.($record["lastyear"] != NULL ? $record["lastyear"] : "0.00").'"></td>';
        $content.='<td width="10%"><input type="submit" value="'.$this->text("edit_balance").'"></form></td>';
        $content.='</tr><tr>';
      }
      else
      {
        $content.='<td width="50%" class="table">'.$this->text("balance_previousyear").' ('.href($url, ($record["year"]-1)).')'.'</td>';
        $content.='<td width="40%" class="table" align="right">'.number_format($record["lastyear"],2,",","").'</td>';
        $content.='<td width="10%" class="table">'.($record["lastyear"]==NULL?"(".$this->text("unavailable").")":"&nbsp;").'</td>';
        $content.='</tr><tr>';
      }

      $rounded = false;
      foreach ($record["types"] as $key => $value)
      {
        $content.='<td width="50%" class="table">'.$key.'</td>';
        $content.='<td width="40%" class="table" align="right">'.$value.'</td>';
        if (atkconfig("holidayreport_round", true))
        {
          $rounded = true;
        }
        $content.='<td width="10%" class="table">&nbsp;&nbsp;'.($rounded?'*':'').'</td>';
        $content.='</tr><tr>';
      }

      if (count($record["corrections"])>0)
      {
        for ($i=0;$i<count($record["corrections"]);$i++)
        {
          $content.='<td width="50%" class="table">'.$this->text("correction").' ('.$record["corrections"][$i]["description"].')</td>';
          $content.='<td width="40%" class="table" align="right">'.number_format($record["corrections"][$i]["correction"],2,",","").'</td>';
          $content.='<td width="10%" class="table">&nbsp;</td>';
          $content.='</tr><tr>';
        }
      }
      else
      {
        $content.='<td width="50%" class="table">'.$this->text("correction").'</td>';
        $content.='<td width="40%" class="table" align="right">0,00</td>';
        $content.='<td width="10%" class="table">&nbsp;</td>';
        $content.='</tr><tr>';
      }

      if ($securityManager->allowed('holidayreport.correction','edit'))
      {
        $url = dispatch_url("holidayreport.employeeholiday_report", "report", array("userid"=>$record["userid"], "year"=>$record["year"]));
        $content.='<td width="50%" class="table"><form action="'.$url.'" method="post"><input type="text" size="25" name="description" value=""></td>';
        $content.='<td width="40%" class="table" align="right"><input type="text" size="3" name="correction" value="0.00"></td>';
        $content.='<td width="10%"><input type="submit" value="'.$this->text("add_correction").'"></form></td>';
        $content.='</tr><tr>';
      }

      $content.='<td width="50%" class="table">&nbsp;</td>';
      $content.='<td width="40%" class="table" align="right"><hr width="25%" align="right"></td>';
      $content.='<td width="10%" class="table">+</td>';
      $content.='</tr><tr>';

      $content.='<td width="50%" class="table">'.$this->text("total").'</td>';
      if (atkconfig("holidayreport_round", true))
      {
        $content.='<td width="40%" class="table" align="right">'.number_format($record["total_leave"],2,",","").'</td>';
        $unrounded = ($record["unrounded"]!=$record["total_leave"]?"(".number_format($record["unrounded"],2,",","").")":"&nbsp;");
      }
      else
      {
        $content.='<td width="40%" class="table" align="right">'.number_format($record["unrounded"],2,",","").'</td>';
        $unrounded = "&nbsp;";
      }

      $content.='<td width="10%" class="table">'.$unrounded.'</td>';
      $content.='</tr><tr>';

      $content.='<td width="50%" class="table">'.$this->text("leave_taken").'</td>';
      $content.='<td width="40%" class="table" align="right">'.number_format($record["leave_taken"],2,",","").'</td>';
      $content.='<td width="10%" class="table">&nbsp;</td>';
      $content.='</tr><tr>';

      $content.='<td width="50%" class="table">&nbsp;</td>';
      $content.='<td width="40%" class="table" align="right"><hr width="25%" align="right"></td>';
      $content.='<td width="10%" class="table">-</td>';
      $content.='</tr><tr>';

      $content.='<td width="50%" class="table"><b>'.$this->text("balance").'</b></td>';
      $content.='<td width="40%" class="table" align="right"><b>'.number_format($record["balance"],2,",","").'</b></td>';
      $content.='<td width="10%" class="table">&nbsp;</td>';
      $content.='</tr><tr>';
      $content.='<td colspan="3">&nbsp;</td>';
      $content.='</tr><tr>';
      if ($rounded)
      {
        $content.='<td colspan=3 class="table">* '.$this->text("explanation_roundinghours").'.</td>';
        $content.='</tr><tr>';
      }

      $content.='</tr></table></div>';

      $hours = $this->getHolidayHours($record["userid"], $record["year"].'-01-01', $record["year"].'-12-31');
      if (count($hours)>0)
      {
        $content.='<div align="left"><hr><table><tr><td class="table">'.$this->text("leave_taken").' '.$record["year"].':</td></tr>';
        $rl = &atknew("atk.recordlist.atkrecordlist");
        $content.= '<tr><td class="table" align="left">'.$rl->render($this, $hours, array(), RL_NO_SORT, array());
        $content.='</td></tr></table></div>';
      }

      if (atkLevel()>0)
      {
        $content.="<br>".atkButton("<< ".atktext("back"), "", SESSION_BACK)."<br>";
      }
      $content.='<br>';

      $box = array(
        "title" => $this->text("employeeholiday_report")." ".$record["year"],
        "content" => $content,
      );
      $ui = &$this->getUI();
      $boxedresult = $ui->renderBox($box);
      return $boxedresult;
    }
  }

?>
