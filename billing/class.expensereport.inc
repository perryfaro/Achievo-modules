<?php

include_once("achievotools.inc");

useattrib("atkdateattribute");
useattrib("atknumberattribute");
useattrib("atkdateattribute");
useattrib("atkdummyattribute");
useattrib("atklistattribute");
useattrib("atkcurrencyattribute");

userelation("atkmanytoonerelation");

define("ONEDAY", 24*60*60);
define("ONEWEEK", 7*ONEDAY);

class timeDisplayAttribute extends atkAttribute
{
  function display($rec)
  {
    $time = $rec[$this->fieldName()];
    if ($time==0) return "";
    if ($time < 0) return sprintf("-%02d",(floor(abs($time)/60))).':'.sprintf("%02d",(abs($time)%60));
    return sprintf("%02d",floor($time/60)).':'.sprintf("%02d",$time%60);
  }
}

function status_cmp($a, $b) { return compare_by_field($a, $b, "status"); }
function week_cmp($a, $b) { return compare_by_field($a, $b, "week"); }
function user_cmp($a, $b) { return compare_by_field($a, $b, "lastname"); }
function project_cmp($a, $b) { return compare_by_field($a, $b, "project"); }

function compare_by_field($a, $b, $field)
{
  if ($a[$field]==$b[$field])
  {
    // equal items are always sorted by week
    if ($a["week"]==$b["week"]) return 0;
    return ($a["week"] < $b["week"]) ? -1 : 1;
  }
  return ($a[$field] < $b[$field]) ? -1 : 1;
}

class expensereport extends atkNode
{
  function expensereport()
  {
    // node() constructor is *not* called automatically!
    $this->atkNode("expensereport",NF_NO_ADD|NF_NO_DELETE|NF_NO_EDIT);
    $this->add(new atkAttribute("user"));
    $this->add(new atkAttribute("expensedate"));
    $this->add(new atkAttribute("week"));
    $this->add(new atkAttribute("expensename"));
    $this->add(new atkAttribute("project"));
    $this->add(new atkAttribute("employer"));

    $this->add(new atkListAttribute("type",array("investment","regular"),array("i","r")));
    
    $p_attr = &$this->add(new atkManyToOneRelation("category","billing.expense_category"));
    $p_attr->addDependee("subcategory");
    
    $p_attr = &$this->add(new atkManyToOneRelation("subcategory","billing.expense_category"));
    $p_attr->setDestinationFilter("pid='[category.id]'");

    $this->add(new atkAttribute("billable"));
    $this->add(new atkAttribute("status"));
    $this->add(new atkCurrencyAttribute("amount", AF_TOTAL, 10, atkconfig("default_currency", "EUR")));

    $this->add(new atkCurrencyAttribute("write_off", AF_TOTAL, 10, atkconfig("default_currency", "EUR")));
    $this->add(new atkCurrencyAttribute("write_off_vat", AF_TOTAL, 10, atkconfig("default_currency", "EUR")));
    //$this->add(new atkAttribute("end_of_write_off"));
    
    $this->addSecurityMap("edit","report");
  }

  function user_display($rec)
  {
    return sprintf("%s %s", $rec['lastname'], $rec['firstname']);
  }

  function get_employee($user_id)
  {
    $db = &$this->getDb();
    $sql = "SELECT * FROM person WHERE status='active' AND id='$user_id'";
    $record = $db->getrows($sql);
    return $record[0];
  }

  function get_employees($userid)
  {
    $db = &$this->getDb();

    $sql = "SELECT id, lastname,firstname,userid
            FROM person
            WHERE status='active' AND role = 'employee'
            ORDER BY lastname
           ";

    $records = $db->getrows($sql);

    $employee_code = '<OPTION VALUE="all">'.atktext("all_users").'</OPTION>';
    for($i=0;$i<count($records);$i++)
    {
      $selected = ($userid==$records[$i]["id"]) ? "selected" : "";
      $employee_code.= '<OPTION VALUE="'.$records[$i]["id"].'" '.$selected.'>'.$records[$i]["lastname"].', '.$records[$i]["firstname"].'</OPTION>';
    }
    return $employee_code;
  }

  function get_projects($userid, $selectedprojectid)
  {
    $db = &$this->getDb();
    global $g_securityManager;

    $view_all = $g_securityManager->allowed('billing.expensereport','view_all');

    if ($view_all)
    {
      $sql = "SELECT id, name
              FROM project
              WHERE status='active'
              ORDER BY name";
    }
    else
    {
      $sql = "SELECT id, name
              FROM project, project_person
              WHERE status='active' AND project_person.personid=$userid AND project.id=project_person.projectid
              ORDER BY name";
    }
    $records = $db->getrows($sql);

    $project_code = '<OPTION VALUE="all">'.atktext("allprojects","project").'</OPTION>';
    for($i=0;$i<count($records);$i++)
    {
      $selected= ($selectedprojectid==$records[$i]["id"]) ? "selected" : "";
      $project_code.= '<OPTION VALUE="'.$records[$i]["id"].'" '.$selected.'>'.$records[$i]["name"].'</OPTION>';
    }
    return $project_code;
  }

  function get_employers($employer_id)
  {
    $db = &atkGetDB();

    $sql = "SELECT organization.id,organization.name,count(organization.id) AS cnt
              FROM person
              INNER JOIN organization ON person.employer_id = organization.id
              WHERE person.role='employee'
              GROUP BY organization.id";

    $records = $db->getrows($sql);
    $employerscode = "<OPTION VALUE='all'>".atktext('allemployers')."</OPTION>";
    foreach ($records as $employer)
    {
      $sel = $employer['id'] == $employer_id ? " SELECTED" : "";
      $employerscode .= "<OPTION VALUE='".$employer['id']."'$sel>".$employer['name']."</OPTION>";
    }
    return $employerscode;
  }

  function action_report()
  {
    global $g_securityManager, $g_user, $g_sessionManager;

    $atkorderby = $g_sessionManager->pageVar("atkorderby");
    $userid = $g_sessionManager->pageVar("userid");
    $projectid = $g_sessionManager->pageVar("projectid");
    $billablefilter = $g_sessionManager->pageVar("billablefilter");
    $nameswitch = $g_sessionManager->pageVar("nameswitch");
    $showdetails = $g_sessionManager->pageVar("showdetails");
    $lockfilter = $g_sessionManager->pageVar("lockfilter");
    $startdate = $g_sessionManager->pageVar("startdate");
    $enddate = $g_sessionManager->pageVar("enddate");
    $type = $g_sessionManager->pageVar("type");
    $employer_id = $g_sessionManager->pageVar("employer_id");
    $showemployer = $g_sessionManager->pageVar("showemployer");

    $categorypair =       explode("=",$g_sessionManager->pageVar("category"));
    $category =           stripQuotes($categorypair[1]);
    if(!isset($category)) $category = "";
    
    $subcategorypair =       explode("=",$g_sessionManager->pageVar("subcategory"));
    $subcategory =           stripQuotes($subcategorypair[1]);
    if(!isset($subcategory)) $subcategory = "";

    /** $outputType gives control over the kind of output:
    *        0 standard(onscreen in compliance with theme, default)
    *        1 printable (onscreen in a format that easily can be printed on paper))
    *             [was $printable]
    *        2 export ( to a file on local computer ex. achievo.csv)
    */
    $outputType = $g_sessionManager->pageVar("outputType");
    if (!isset($outputType)||$outputType=="") $outputType=0;

    if (!isset($atkorderby)||$atkorderby=="") $atkorderby = "week";
    if (!isset($projectid)||$projectid=="") $projectid="all";
    if (!isset($billablefilter)||$billablefilter=="") $billablefilter="both";
    if (!isset($lockfilter)||$lockfilter=="") $lockfilter = "both";
    if (!isset($userid)||$userid=="") $userid=$g_user["id"];
    if (!isset($nameswitch)||$nameswitch=="") $nameswitch="name";
    if (!isset($employer_id)||$employer_id=="") $employer_id="all";

    $view_all = $g_securityManager->allowed('billing.expensereport','view_all');

    if (is_array($startdate))
    {
      $startdate = $startdate["year"]."-".sprintf("%02d",$startdate["month"])."-".sprintf("%02d",$startdate["day"]);
    }
    else
    {
      if (!isset($startdate)||$startdate=="")
      {
        $startdate =  date("Y-m-d" ,time()-(86400*7));
      }
    }
    if (is_array($enddate))
    {
      $enddate = $enddate["year"]."-".sprintf("%02d",$enddate["month"])."-".sprintf("%02d",$enddate["day"]);
    }
    else
    {
      if (!isset($enddate)||$enddate=="")
      {
        $enddate = date('Y-m-d');
      }
    }

    $ui = &$this->getUi();
    $page = &$this->getPage();
    $this->addStyle("style.css");
    $page->register_script(atkconfig("atkroot")."atk/javascript/formsubmit.js");
    $page->register_script(atkconfig("atkroot")."atk/javascript/tools.js");
    $page->register_script(atkconfig("atkroot")."atk/javascript/dhtml_formtools.js");
    
    if ($outputType==0)
    {
      $content='<form action="'.getDispatchFile().'" method="get" name="entryform" id="entryform">';

      $content.= '<input type="hidden" name="atknodetype" value="billing.expensereport">';
      $content.= '<input type="hidden" name="atkaction" value="'.$this->m_action.'">';
      $content.= '<table border="0" cellspacing="0" cellpadding="2">';
      $content.= '<tr>';
      $content.= '<td colspan="2"><b>'.atktext('setexpensesfilter', 'billing').'</b></td>';
      $content.= '</tr><tr>';

      // we have to pass a 'dummy' record to the attributes to set their default value.
      $dummyrec = array("startdate"=>array("year"=>substr($startdate,0,4),
                                           "month"=>substr($startdate,5,2),
                                           "day"=>substr($startdate,8,2)),
                        "enddate"=>array("year"=>substr($enddate,0,4),
                                         "month"=>substr($enddate,5,2),
                                         "day"=>substr($enddate,8,2)),
                        "type"=>$type,
                        "category" => array("id"=>$category),
                        "subcategory" => array("id"=>$subcategory)
                                         );

      $content.= '<td><SELECT name="nameswitch"><OPTION VALUE="name">'.atktext("name").': <OPTION VALUE="supervisor" '.($nameswitch=="supervisor"?"selected":"").'>'.atktext("supervisor").': </SELECT></td>';

      if($view_all)
      {
        $content.= '<td><SELECT name="userid">'.$this->get_employees($userid).'</SELECT></td>';
      }
      else
      {
        $userid = $g_user["id"];
        $content.= '<td>'.$g_user["name"].'<input type="hidden" name="userid" value="'.$g_user["id"].'">';
      }

      /*// TODO: Even if you don't have view_all right, you should still be able to view employees
      // that you supervise.
      if($view_all)
      {
        $layout->output('</tr><tr>');
        $layout->td(atktext("supervisor").':</b> ');
        $layout->td('<SELECT name="supervisor">'.$this->get_supervisors($supervisor).'</SELECT>');
      }    */

      $content.= '</tr><tr>';
      $content.= '<td>'.atktext("project").': </td>';
      $content.= '<td><SELECT name="projectid">'.$this->get_projects($g_user["id"],$projectid).'</SELECT></td>';
      $content.= '</tr><tr>';
      
      $content.= '</tr><tr>';
      $content.= '<td>'.atktext("employer_id").': </td>';
      $content.= '<td><SELECT name="employer_id">'.$this->get_employers($employer_id).'</SELECT></td>';
      $content.= '</tr><tr>';

      $p_attr = new atkListAttribute("type",array("investment","regular"),array("i","r"));
      $content.= '</tr><tr>';
      $content.= '<td>'.atktext("type").': </td>';
      $content.= '<td'.$p_attr->edit($dummyrec).'</td>';
      $content.= '</tr><tr>';
      
      $p_attr = &$this->add(new atkManyToOneRelation("category","billing.expense_category"));
      $p_attr->addDestinationFilter("pid IS NULL");
      $p_attr->addDependee("subcategory");
      
      $p_attr->_registerDependees();

      $content.= '</tr><tr>';
      $content.= '<td>'.atktext("category").': </td>';
      $content.= '<td>'.$p_attr->edit($dummyrec).'</td>';
      $content.= '</tr><tr>';

      $p_attr = &$this->add(new atkManyToOneRelation("subcategory","billing.expense_category"));
      $p_attr->addDestinationFilter("pid='[category.id]'");

      $content.= '</tr><tr>';
      $content.= '<td>'.atktext("subcategory").': </td>';
      $content.= '<td id="billing_expensereport_subcategory">'.$p_attr->edit($dummyrec).'</td>';
      $content.= '</tr><tr>';

      $content.= '<td>'.atktext("timespan").': </td>';

      $startdateatt = new atkDateAttribute("startdate","","", 0, date("Ymd"));
      $enddateatt = new atkDateAttribute("enddate","","", 0, date("Ymd"));

      $content.= '<td>'.$startdateatt->edit($dummyrec).' &nbsp;'.
                      atktext("until").
                      '&nbsp; '.$enddateatt->edit($dummyrec).'</td>';

      $content.= '</tr><tr>';

      $content.= '<td>'.atktext("status").'</td>';
      $content.= '<td><select name="lockfilter">
                     <option VALUE="both" '.($lockfilter=="both"?"selected":"").'>'.atktext("both").'
                     <option VALUE="locked" '.($lockfilter=="locked"?"selected":"").'>'.atktext("locked").'
                     <option VALUE="unlocked" '.($lockfilter=="unlocked"?"selected":"").'>'.atktext("unlocked").'
                     </select></td>';

      $content.= '</tr><tr>';

      $content.= '<td>'.atktext("billable", "billing").'</td>';
      $content.= '<td><select name="billablefilter">
                     <option VALUE="both" '.($billablefilter=="both"?"selected":"").'>'.atktext("both").'
                     <option VALUE="1" '.($billablefilter=="1"?"selected":"").'>'.atktext("billable", "billing").'
                     <option VALUE="0" '.($billablefilter=="0"?"selected":"").'>'.atktext("nonbillable", "billing").'
                     </select></td>';

      $content.= '</tr><tr>';

      $content.= '<td colspan="2"><b>'.atktext("report_output_options").'</b></td>';

      $content.= '</tr><tr>';

      $content.= '<td>'.atktext("report_output_type").'</td>';
      $content.= '<td><select name="outputType">
                     <option VALUE="0" selected>'.atktext("report_output_standard").'
                     <option value="1">'.atktext("report_output_printable").'
                     <option value="2">'.atktext("report_output_export").'
                     </select></td>';

      $content.= '</tr><tr>';

      $content.= '<td>'.atktext("report_show_details").'</td>';
      if ($showdetails==1) $checked = "checked"; else $checked = "";
      $content.= '<td><input type="checkbox" name="showdetails" value="1" '.$checked.'></td>';

      $content.= '</tr><tr>';

      $content.= '<td>'.atktext("report_show_employer","billing").'</td>';
      if ($showemployer==1) $checked = "checked"; else $checked = "";
      $content.= '<td><input type="checkbox" name="showemployer" value="1" '.$checked.'></td>';

      $content.= '</tr><tr>';

      $content.= '<td>'.atktext("orderby").': </td>';

      $content.= '<td><select name="atkorderby">
                     <option VALUE="week" '.($atkorderby=="week"?"selected":"").'>'.atktext("week").'
                     <option VALUE="status" '.($atkorderby=="status"?"selected":"").'>'.atktext("status").'
                     <option VALUE="user" '.($atkorderby=="user"?"selected":"").'>'.atktext("employee").'
                     <option VALUE="project" '.($atkorderby=="project"?"selected":"").'>'.atktext("project").'
                     </select></td>';



      $content.= '</tr></table><input type="submit" value="'.atktext("refresh").'"></form><br>';

      $page->addContent($ui->renderBox(array("title"=>atktext("parameters"), "content"=>$content)));
      $page->register_scriptcode("var atkErrorFields = new Array();");
    }

    if ($userid!="") // If we haven't got a userid by now, don't display the
                     // report.
    {
      $weekdata = array();

      // first fill the weeks and start/end dates..
      $startstamp = mktime(12,0,0,substr($startdate,5,2),
                                 substr($startdate,8,2),
                                 substr($startdate,0,4));
      $endstamp = mktime(12,0,0,substr($enddate,5,2),
                               substr($enddate,8,2),
                               substr($enddate,0,4));

      $report_start_week = weekstamp($startstamp);
      $report_end_week = weekstamp($endstamp);

      // determine expenses of given period
      $query = "SELECT
                  expensedate, billing_expensetype.name as expensename, amount,
      person.id as userid , lastname, firstname,
        project.name as project, project.id as projectid, billable,
        IF( ((billing_expense.period IS NOT NULL) AND (billing_expense.period>0)),amount/billing_expense.period,'') write_off,
        IF( ((billing_expense.period IS NOT NULL) AND (billing_expense.period>0)),
        amount*( IF( tax.percentage IS NULL,0,tax.percentage)/100+1)/billing_expense.period,'') write_off_vat,billing_expense.period,
        billing_expense.type,category.category,subcategory.category as subcategory,organization.name as employer
                FROM
                   billing_expense
                   LEFT JOIN billing_expense_category category ON billing_expense.category=category.id
                   LEFT JOIN billing_expense_category subcategory ON billing_expense.subcategory=subcategory.id
                   LEFT JOIN billing_taxrate tax ON tax.id=billing_expense.vat,
                   billing_expensetype, project, person
                   LEFT JOIN organization on person.employer_id=organization.id
                WHERE ";

      if($subcategory != "") $query.= " billing_expense.subcategory = '$subcategory' AND ";
      elseif($category != "") $query.= " billing_expense.category = '$category' AND ";
      
      if ($employer_id!="all") $query.= " person.employer_id = '$employer_id' AND ";
      
      if ($projectid!="all") $query.= " project.id = '$projectid' AND ";
      
      if ($type!="") $query.= " billing_expense.type = '$type' AND ";

      if ($billablefilter!="both") $query.= " billable = ".$billablefilter." AND ";

      if ($nameswitch=="supervisor")
      {
        if ($userid!="all") $query.= " person.supervisor = ".$userid." AND ";
      }
      else
      {
        if ($userid!="all") $query.= " billing_expense.user_id =".$userid." AND ";
      }
      $query.= "  expensedate BETWEEN '$startdate' AND '$enddate'
                  AND billing_expense.user_id=person.id
                  AND billing_expense.expensetype_id=billing_expensetype.id
      AND billing_expense.project_id=project.id
                  ORDER BY
                  expensedate";
  $db = &$this->getDb();
  $expenses = $db->getrows($query);

  // get locks of this period
  $query = "SELECT
                  week, userid
                FROM
                  billing_expense_lock
                WHERE
                  week BETWEEN $report_start_week AND $report_end_week";
  $locks = $db->getrows($query);

  for ($i=0;$i<count($locks);$i++)
  {
    $lockedweeks[$locks[$i]["week"]][$locks[$i]["userid"]]=1;
  }

  $rldata = array();

  // get weekstamp and lock state for each entry
  for ($i=0;$i<count($expenses);$i++)
  {
    $expenses[$i]["week"] = weekstamp(mktime(0,0,0,substr($expenses[$i]["expensedate"],5,2),
        substr($expenses[$i]["expensedate"],8,2),
        substr($expenses[$i]["expensedate"],0,4)));

    if ( $lockedweeks[$expenses[$i]["week"]][$expenses[$i]["id"]] ||
      $lockedweeks[$expenses[$i]["week"]][0] )
      $expenses[$i]["status"]=atktext("locked");
    else
      $expenses[$i]["status"]=atktext("unlocked");

    if ( $expenses[$i]["billable"] ) $expenses[$i]["billable"] = atktext("billable");
    else $expenses[$i]["billable"] = atktext("nonbillable");
    
    $expenses[$i]["category"] = array("category"=>$expenses[$i]["category"]);
    $expenses[$i]["subcategory"] = array("category"=>$expenses[$i]["subcategory"]);
    
    //if(!is_null($expenses[$i]['period']))
    //  $expenses[$i]["end_of_write_off"] = date("Y-m-d",mktime(0,0,0,substr($expenses[$i]["expensedate"],5,2)+$expenses[$i]['period'],
    //    substr($expenses[$i]["expensedate"],8,2),
    //    substr($expenses[$i]["expensedate"],0,4)));

    if ($lockfilter=="both" || $expenses[$i]["status"]==atktext($lockfilter))
      $rldata[$i]=$expenses[$i];
  }

      // sorting..
      if ($atkorderby=="") $atkorderby = "week";
      usort($rldata,$atkorderby."_cmp");

      // detail view ...
      $suppress=array();
      if ($showdetails!=1)
      {
        $suppress[] = "project";
        $suppress[] = "billable";
        $suppress[] = "week";
      }
      
      if ($showemployer!=1)
      {
        $suppress[] = "employer";
      }
      
      if ($nameswitch=="name" && $userid!="all") $suppress[] = "user";

      if ($outputType=="0")
      {
        $title = atktext("weekreport")." ".atktext("report_intimespan")." ".$startdate." t/m ".$enddate;
        $content = '';
        if ($userid!="all")
        {
          $emprec = $this->get_employee($userid);      ;

          $content = '<br><table border="0" cellspacing="0" cellpadding="2"><tr>';

          if ($nameswitch=="supervisor")
          {
            $label = atktext("supervisor");
          }
          else
          {
            $label = atktext("name");
          }
          $content.= '<td>'.$label.": </td>";
          $content.= '<td>'.$emprec["lastname"].", ".$emprec["firstname"].'</td>';
          $content.= '</tr><tr>';
          $content.= '<td>'.atktext("email").": </td>";
          $content.= '<td>'.$emprec["email"].'</td>';
          $content.= '</tr></table>';
          $content.= '<br>';
        }

        $rl = &atknew("atk.recordlist.atkrecordlist");
        $content .= '<br>'.$rl->render($this, $rldata, array(),false,$suppress);
        $content .= '<br><br>';
        $page->addContent($ui->renderBox(array("title"=>$title, "content"=>$content)));
      }
      else if ($outputType=="1")
      {
        $rl = &atknew("atk.recordlist.atkcustomrecordlist");
        $page->addContent($rl->render($this,$rldata,"<tr>","<td>","</td>","</tr>", "0","",$suppress));
      }
      else if ($outputType=="2")
      {
        // special RecordList can export to file
        $rl = &atknew("atk.recordlist.atkcustomrecordlist");
        $rl->render($this,$rldata, "", '"', '";', "\r\n", "1", "",$suppress);
      }

    }
  }
}

?>
