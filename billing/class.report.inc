<?php
  /**
   * report node class file
   *
   * @package achievo_modules
   * @subpackage billing
   *
   * @author ivo <ivo@ibuildings.nl>
   *
   * @copyright (c) 2005 Ibuildings.nl BV
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision: 2728 $
   * $Id: class.report.inc 2728 2005-11-24 01:15:51Z ivo $
   */

  /**
   * @internal includes
   */
  useattrib("atkattribute");
  useattrib("atkdateattribute");

  atkimport("module.utils.dateutil");

  include_once("achievotools.inc");

  /**
   * The node class for billing reports
   *
   * @author ivo <ivo@ibuildings.nl>
   * @package achievo_modules
   * @subpackage billing
   */
  class report extends atkNode
  {
    /**
     * report constructor
     */
    function report()
    {
      $this->atkNode("report");
    }

    function getFilterRecord()
    {
      static $s_record = NULL;
      global $g_sessionManager;

      if ($s_record==NULL)
      {
        $s_record = array();
        $s_record["supervisor"] = $g_sessionManager->pageVar("supervisor");
        $s_record["from"] = $g_sessionManager->pageVar("from");
        $s_record["to"] = $g_sessionManager->pageVar("to");

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
      useattrib("employee.myemployeesattribute");

      $attrs["supervisor"] = &new myEmployeesAttribute("supervisor", "billing.report.any_user");

      $attrs["from"] = &new atkDateAttribute("from");
      $attrs["to"] = &new atkDateAttribute("to");

      return $attrs;
    }

    function getFilterBar()
    {
      $record = $this->getFilterRecord();

      $attrs = &$this->getFilterAttrs();

      $output = '<form action="dispatch.php" method="get">';
      $output.= session_form();
      $output.= '<input type="hidden" name="go" value="1">'; //trigger to prevent loading the first time

      if ($this->hasAnyUserPrivilege())
      {
        $output.= atktext("supervisor").": ".$attrs["supervisor"]->edit($record)." ";
      }

      $output.= "<br/>";

      $output.= atktext("period_from", "weekplanning")." ".$attrs["from"]->edit($record)." ";
      $output.= atktext("to", "weekplanning")." ".$attrs["to"]->edit($record);

      $output.= ' <input type="submit" value="'.atktext("refresh").'">';
      $output.= '</form>';

      return $output;
    }

    function action_ratereport()
    {
      $content = $this->getRateReport();

      // Add the given content to the page
      $page = &$this->getPage();
      $page->addContent($this->renderActionPage("ratereport", $content));
    }

    function action_declarabilityreport()
    {
      $content = $this->getDeclarabilityReport();

      // Add the given content to the page
      $page = &$this->getPage();
      $page->addContent($this->renderActionPage("declarabilityreport", $content));
    }

    function getRateReport()
    {
      // Get a reference to the sessionmanager
      global $g_sessionManager;

      $this->addStyle("style.css");

      // Try to get a reference to the User Interface object and raise an error if it fails
      $ui = &$this->getUi();

      $go = $g_sessionManager->pageVar("go");

      // Start composing the output by adding the filterbar as topmost item
      $output = $this->getFilterBar();

      $filterrecord = $this->getFilterRecord();

      // Add a newline (vertical spacing)
      $output.= '<br/>';

      // Only get the planning if the filter was set and submitted
      if ($go == 1)
      {
        $db = &$this->getDb();
        $strfrom = dateutil::arr2str($filterrecord["from"]);
        $strto = dateutil::arr2str($filterrecord["to"]);

        $empfilter = "";
        if ($filterrecord["supervisor"]!=NULL)
        {
          $empfilter = " AND e.supervisor=".$filterrecord["supervisor"]["id"];
        }
        else if (!$this->hasAnyUserPrivilege()) // only show your own employees
        {
          $user = getUser();
          $empfilter = " AND e.supervisor=".$user["id"];
        }

        $rows = $db->getrows("SELECT
                                e.id as userid,
                                e.firstname, e.lastname,
                                date_format(hours.activitydate, '%x %v') as week,
                                sum(time) as minutes,
                                billing_rate,
                                sum((time/60)*billing_rate) as total
                              FROM
                                hours, person e
                              WHERE
                                hours.userid=e.id
                                AND hours.activitydate BETWEEN '$strfrom' AND '$strto'
                                $empfilter
                              GROUP BY
                                e.id, e.firstname, e.lastname, date_format(hours.activitydate, '%x %v'), billing_rate
                              ORDER BY
                                e.lastname, e.firstname, date_format(hours.activitydate, '%x %v'), billing_rate");

         $data[] = array(atktext("employee"), atktext("week"), atktext("time"), atktext("rate", "billing"), atktext("total"));

         $total_time = 0;
         $total_rate = 0;

         $previousname="";

         for ($i=0, $_i=count($rows); $i<$_i; $i++)
         {
           $line = array();

           $name = $rows[$i]["firstname"]." ".$rows[$i]["lastname"];
           if ($name!=$previousname)
           {
             $detailparams["startdate"] = $strfrom;
             $detailparams["enddate"] = $strto;
             $detailparams["userid"] = $rows[$i]["userid"];

             $line[] = href(dispatch_url("reports.hoursurvey", "report", $detailparams), $name, SESSION_NESTED);

             $previousname = $name;
           }
           else
           {
             $line[] = "";
           }

           $line[] = $rows[$i]["week"];
           $line[] = time_format($rows[$i]["minutes"]);
           $line[] = sprintf("%0.02f", $rows[$i]["billing_rate"]);
           $line[] = sprintf("%0.02f", $rows[$i]["total"]);

           $total_time += $rows[$i]["minutes"];
           $total_rate += $rows[$i]["total"];

           $data[] = $line;
         }

         $data[] = array(atktext("total"), "", time_format($total_time), "", sprintf("%0.02f", $total_rate));

         $tr = &atknew("atk.utils.atktablerenderer");
         $output.= $tr->render($data, TBL_DATA|TBL_FOOTER, "recordlist")."<br/>";

      }

      // Render a box around the generated output
      $boxedoutput = $ui->renderBox(array("title"=>$this->actionTitle('ratereport'), "content"=>$output));

      // Return the generated boxed output
      return $boxedoutput;
    }


    function getDeclarabilityReport()
    {
      // Get a reference to the sessionmanager
      global $g_sessionManager;

      $this->addStyle("style.css");

      // Try to get a reference to the User Interface object and raise an error if it fails
      $ui = &$this->getUi();

      $go = $g_sessionManager->pageVar("go");

      // Start composing the output by adding the filterbar as topmost item
      $output = $this->getFilterBar();

      $filterrecord = $this->getFilterRecord();

      // Add a newline (vertical spacing)
      $output.= '<br/>';

      // Only get the planning if the filter was set and submitted
      if ($go == 1)
      {
        $db = &$this->getDb();
        $strfrom = dateutil::arr2str($filterrecord["from"]);
        $strto = dateutil::arr2str($filterrecord["to"]);

        $empfilter = "";
        if ($filterrecord["supervisor"]!=NULL)
        {
          $empfilter = " AND e.supervisor=".$filterrecord["supervisor"]["id"];
        }
        else if (!$this->hasAnyUserPrivilege()) // only show your own employees
        {
          $user = getUser();
          $empfilter = " AND e.supervisor=".$user["id"];
        }

        // determine zoommode
        $type_id = $g_sessionManager->pageVar("projecttype_id");
        if ($type_id==""||$type_id==-1)
        {

          $types = $db->getrows("SELECT pt.id, pt.description FROM billing_projecttype pt");


           $data[] = array(atktext("billing_type", "billing"), atktext("time_declarable", "billing"), atktext("time_nondeclarable", "billing"), atktext("declarability", "billing"));

           $total_decl = 0;


           for ($i=0, $_i=count($types); $i<$_i; $i++)
           {
             $line = array();
             $line[] = href("?projecttype_id=".$types[$i]["id"], $types[$i]["description"]);

             $qry = "SELECT
                       sum(h.time) as time
                     FROM
                       hours h,
                       phase ph,
                       project p,
                       person e
                     WHERE
                       h.phaseid = ph.id
                       AND ph.projectid = p.id
                       AND e.id = h.userid
                       AND p.billing_type = ".$types[$i]["id"]."
                       AND h.activitydate BETWEEN '$strfrom' AND '$strto'
                       $empfilter
                       AND p.billing_hoursdeclarable=";

             $declarable_recs = $db->getrows($qry."1");
             $nondeclarable_recs = $db->getrows($qry."0");

             $declarable = $declarable_recs[0]["time"];
             $nondeclarable = $nondeclarable_recs[0]["time"];

             $line[] = time_format($declarable);
             $line[] = time_format($nondeclarable);

             if ($declarable+$nondeclarable==0)
             {
               $decl = "0";
             }
             else
             {
               $decl = $declarable/($declarable+$nondeclarable)*100;
             }
             $line[] = sprintf("%0.02f", $decl)."%";

             $total_decl+=$decl;

             $data[] = $line;
           }

           if (count($types)==0)
           {
             $avg = "0";
           }
           else
           {
             $avg = $total_decl/(count($types));
           }
           $data[] = array(atktext("average"), "", "", sprintf("%0.02f", $avg)."%");

           $tr = &atknew("atk.utils.atktablerenderer");
           $output.= $tr->render($data, TBL_DATA|TBL_FOOTER, "recordlist")."<br/>";
        }
        else
        {
          $typerecs = $db->getrows("SELECT description FROM billing_projecttype WHERE id=".$db->escapeSQL($type_id));

          $output.= "<b>".$typerecs[0]["description"].'</b> ';
          $output.= href("?projecttype_id=-1", atktext("back"))."<br/><br/>";

          // Zoom in on employees within type.
          $emps = $db->getrows("SELECT
                                  e.lastname, e.firstname,
                                  e.id,
                                  e.billing_declarability
                                FROM
                                  person e
                                WHERE
                                  role='employee' $empfilter
                                ORDER BY
                                  e.lastname, e.firstname");
          $data[] = array(atktext("employee"), atktext("time_declarable", "billing"), atktext("time_nondeclarable", "billing"), atktext("billing_declarability", "billing"), atktext("declarability", "billing"));


          for ($i=0, $_i=count($emps); $i<$_i; $i++)
          {
            $line = array();
            $line[] = $emps[$i]["firstname"]." ".$emps[$i]["lastname"];

            $qry = "SELECT
                     sum(h.time) as time
                   FROM
                     hours h,
                     phase ph,
                     project p,
                     person e
                   WHERE
                     h.phaseid = ph.id
                     AND ph.projectid = p.id
                     AND e.id = h.userid
                     AND p.billing_type = ".$type_id."
                     AND h.activitydate BETWEEN '$strfrom' AND '$strto'
                     AND h.userid = ".$emps[$i]["id"]."
                     AND p.billing_hoursdeclarable=";

             $declarable_recs = $db->getrows($qry."1");
             $nondeclarable_recs = $db->getrows($qry."0");

             $declarable = $declarable_recs[0]["time"];
             $nondeclarable = $nondeclarable_recs[0]["time"];

             $line[] = time_format($declarable);
             $line[] = time_format($nondeclarable);

             $line[] = sprintf("%0.02f", $emps[$i]["billing_declarability"])."%";

             if ($declarable+$nondeclarable==0)
             {
               $decl = "0";
             }
             else
             {
               $decl = $declarable/($declarable+$nondeclarable)*100;
             }
             $declstr = sprintf("%0.02f", $decl)."%";

             if ($decl<$emps[$i]["billing_declarability"])
             {
               $declstr = '<span style="color: '.COLOR_ERROR.';">'.$declstr.'</span>';
             }
             $line[] = $declstr;

             $data[] = $line;

          }

          $tr = &atknew("atk.utils.atktablerenderer");
          $output.= $tr->render($data, TBL_DATA, "recordlist")."<br/>";
        }
      }

      // Render a box around the generated output
      $boxedoutput = $ui->renderBox(array("title"=>$this->actionTitle('declarabilityreport'), "content"=>$output));

      // Return the generated boxed output
      return $boxedoutput;
    }

    function hasAnyUserPrivilege()
    {
      return $this->allowed("any_user");
    }

  }
?>