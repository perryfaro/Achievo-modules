<?php

  function employee_cmp($a, $b) { return compare_by_field($a, $b, "employee"); }
  function birthdate_cmp($a, $b) { return compare_by_field($a, $b, "birthdate"); }
  function total_leave_cmp($a, $b) { return compare_by_field($a, $b, "total_leave"); }

  if(!function_exists("compare_by_field")) {
	  function compare_by_field($a, $b, $field)
	  {
	    if ($a[$field]==$b[$field])
	    {
	      // equal items are always sorted by employee
	      if ($a["employee"]==$b["employee"]) return 0;
	      return ($a["employee"] < $b["employee"]) ? -1 : 1;
	    }
	    return ($a[$field] < $b[$field]) ? -1 : 1;
	  }
  }
  
  class holidayreport extends atkNode
  {
    function holidayreport()
    {
      $this->atkNode("holidayreport");
      $this->add(new atkAttribute("employee"));
      $this->add(new atkAttribute("userid", AF_HIDE));
      $this->add(new atkAttribute("year", AF_HIDE));
      $this->add(new atkAttribute("birthdate"));
      $this->add(new atkAttribute("lastyear"));

      $this->m_securityMap["report"] = "view_all";
    }

    function getEmployees()
    {
      $db = &atkGetDb();

      $sql = "SELECT id, lastname, firstname, userid, birthdate, employer_id
            FROM person
            WHERE role = 'employee'
            ORDER BY lastname";

      $records = $db->getrows($sql);
      return $records;
    }

    function employee_display($record)
    {
      $outputtype = array_key_exists("outputType", $this->m_postvars) ? $this->m_postvars["outputType"] : 0;
      if ($outputtype==0)
      {
        $url = dispatch_url("holidayreport.employeeholiday_report", "report", array("userid"=>$record["userid"], "year"=>$record["year"]));
        return href($url, $record["employee"], SESSION_NESTED);
      }
      return $record["employee"];
    }

    function fmt($value)
    {
      $res = number_format($value, 2, ",", "");
      $outputtype = array_key_exists("outputType", $this->m_postvars) ? $this->m_postvars["outputType"] : 0;
      if ($outputtype!=2) // file export may not contain html
      {
        $res = '<div align="right">'.$res.'</div>';
      }
      return $res;
    }

    function lastyear_display($record)
    {
      return $this->fmt($record["lastyear"]);
    }

    function total_correction_display($record)
    {
      return $this->fmt($record["total_correction"]);
    }

    function total_leave_display($record)
    {
      return $this->fmt($record["total_leave"]);
    }

    function unrounded_display($record)
    {
      return $this->fmt($record["unrounded"]);
    }

    function leave_taken_display($record)
    {
      return $this->fmt($record["leave_taken"]);
    }

    function balance_display($record)
    {
      return $this->fmt($record["balance"]);
    }

    function action_report()
    {
      $sessionmanager = &atkGetSessionManager();
      $atkorderby = $sessionmanager->pageVar("atkorderby");

      /** $outputType gives control over the kind of output:
       *        0 standard(onscreen in compliance with theme, default)
       *        1 printable (onscreen in a format that easily can be printed on paper))
       *             [was $printable]
       *        2 export ( to a file on local computer ex. achievo.csv)
       */
      $outputType = $sessionmanager->pageVar("outputType");
      if (!isset($outputType)||$outputType=="") $outputType=0;
      if (!isset($atkorderby)||$atkorderby=="") $atkorderby = "employee";

      $selectedyear = $sessionmanager->pageVar("year");
      if ($selectedyear == "") $selectedyear = date("Y");

      $page = &$this->getPage();
      $this->addStyle("style.css");
      $ui = &$this->getUI();
      $boxes = array();
      $dispatchLocation = getDispatchFile();

      //standard
      if ($outputType==0)
      {
        $content = '<form action="'.$dispatchLocation.'" method="get" name="entryform">';

        $content.= '<input type="hidden" name="atknodetype" value="holidayreport.holidayreport">';
        $content.= '<input type="hidden" name="atkaction" value="'.$this->m_action.'">';
        $content.= '<table border="0" cellspacing="0" cellpadding="2">';
        $content.= '<tr>';
        $content.= '<td colspan="2"><b>'.$this->text('setyearfilter').':</b></td>';
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

        $content.= '</tr><tr>';

        $content.= '<td colspan="2"><br><b>'.$this->text("report_output_options").'</b></td>';

        $content.= '</tr><tr>';

        $content.= '<td>'.$this->text("report_output_type").'</td>';
        $content.= '<td><select name="outputType">
                     <option VALUE="0" selected>'.$this->text("report_output_standard").'
                     <option value="1">'.$this->text("report_output_printable").'
                     <option value="2">'.$this->text("report_output_export").'
                     </select></td>';

        $content.= '</tr><tr>';
        $content.= '<td>'.$this->text("orderby").': </td>';

        $content.= '<td><select name="atkorderby">
                     <option VALUE="employee" '.($atkorderby=="employee"?"selected":"").'>'.$this->text("employee").'
                     <option VALUE="birthdate" '.($atkorderby=="birthdate"?"selected":"").'>'.$this->text("birthdate").'
                     <option VALUE="total_leave" '.($atkorderby=="total_leave"?"selected":"").'>'.$this->text("leave").'
                     </select></td>';



        $content.= '</tr></table><br><input type="submit" value="'.$this->text("refresh").'"></form>';

        $box = array(
          "title" => $this->text("parameters"),
          "content" => $content,
        );
        $boxes[] = $ui->renderBox($box);
      }

      if ($selectedyear!="") // If we haven't got a year by now, don't display the report.
      {
        $rldata = array();
        // get leave types
        $types = getTypes();
        for ($i=0;$i<count($types);$i++)
        {
          $this->add(new atkAttribute($types[$i]["description"]));
           // get the age rules
          $rules[$types[$i]["description"]] = getRules($types[$i]["id"], $selectedyear);
        }
        $this->add(new atkAttribute("total_correction"));
        $this->add(new atkAttribute("total_leave", AF_TOTAL));
        $this->add(new atkAttribute("unrounded", AF_TOTAL));
        $this->add(new atkAttribute("leave_taken", AF_TOTAL));
        $this->add(new atkAttribute("balance", AF_TOTAL));

        // get employeedata
        $employees = $this->getEmployees();
        for ($i=0;$i<count($employees);$i++)
        {
          // get the contracthours of the employee of the selected year
          $contracts = getContracts($employees[$i]["id"], $selectedyear);

          if (count($contracts)>0)
          {
            $rldata[$i] = getEmployeeData($employees[$i]["id"], $employees[$i]["birthdate"],$selectedyear, $contracts, $types, $rules, $employees[$i]['employer_id']);

            foreach($rldata[$i]["types"] as $key => $value)
            {
              $rldata[$i][$key] = $value;
            }
            $rldata[$i]["employee"] = $employees[$i]["lastname"].", ".$employees[$i]["firstname"];
            $rldata[$i]["userid"] = $employees[$i]["id"];
            $rldata[$i]["year"] = $selectedyear;
            $rldata[$i]["birthdate"] = $employees[$i]["birthdate"];

          }
        }

        // sorting..
        usort($rldata,$atkorderby."_cmp");
        $suppress = array();
        $rl = null;


        //standard
        if ($outputType=="0")
        {
          $rl = &atknew("atk.recordlist.atkrecordlist");
          $box = array(
            "title" => $this->text("holidayreport")." ".$selectedyear,
            "content" => '<br>'.$rl->render($this, $rldata, array(),false,$suppress).'<br><br>',
          );
          $boxes[] = $ui->renderBox($box);

          $actionpage = $this->renderActionPage("report", $boxes);
          $page->addContent($actionpage);
        }
        //printer-friendly
        else if ($outputType=="1")
        {
          $rl = &atknew("atk.recordlist.atkcustomrecordlist");
          $content.= $rl->render($this,$rldata,"<tr>","<td>","</td>","</tr>", "0","",$suppress);
          $page->addContent($content);
        }
        //export to file
        else if ($outputType=="2")
        {
          $rl = &atknew("atk.recordlist.atkcustomrecordlist");
          $rl->render($this, $rldata, "", ";", "", "\r\n", "1", "",$suppress);
        }
      }
    }
  }

?>