<?php
  /**
   * weekplanning node class file
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
   * $Id: class.weekplanning.inc 3686 2006-09-13 13:17:21Z harrie $
   */

  // Load the used attribute/relation classes
  useattrib("atkattribute");
  useattrib("atkdateattribute");
  useattrib("atknumberattribute");
  useattrib("atkboolattribute");
  useattrib("weekplanning.weekplanningprojectphaseattribute");
  userelation("atkmanytoonerelation");

  atkimport("module.utils.dateutil");

  /**
   * The node class for weekplanning.
   *
   * @author ivo <ivo@ibuildings.nl>
   * @author guido <guido@ibuildings.nl>
   * @package achievo_modules
   * @subpackage weekplanning
   */
  class weekplanning extends atkNode
  {
    /**
     * weekplanning constructor
     */
    function weekplanning($type = "weekplanning", $flags = 0)
    {
      $this->atkNode($type, $flags);
      $this->add(new atkAttribute("id", AF_AUTOKEY));
      $this->add(new atkDateAttribute("monday", AF_OBLIGATORY));
      $this->add(new atkNumberAttribute("minutes", AF_OBLIGATORY));
      //$this->add(new atkManyToOneRelation("phaseid", "project.phase", AF_OBLIGATORY));
      $this->add(new atkNumberAttribute("phaseid")); // TEMP! until phase support is added
      $this->add(new atkManyToOneRelation("activityid", "project.activity", AF_OBLIGATORY));
      $this->add(new atkManyToOneRelation("employeeid", "employee.employee", AF_OBLIGATORY));
      $this->add(new atkBoolAttribute("estimate"));
      $this->setTable("weekplanning_planning");

      // disable the keyboardhandler as we can't handle this many grid cells properly
      global $config_use_keyboard_handler;
      $config_use_keyboard_handler = false;
    }

    /**
     * Update handler function
     */
    function action_update()
    {
      atkimport("module.weekplanning.conflictlistener");
      $this->addListener(new conflictListener(array("update", "save")));

      // Read the posted values
      $data = $this->m_postvars["data"];
      $olddata = $this->m_postvars["olddata"];

      // Get a reference to the database object
      $db = &$this->getDb();

      $analysis_queue = array();

      // Loop through all cells
      foreach($data as $cell => $value)
      {

        // If the value is defferent from the original (old) value, then start update this particular weekplanning value
        if ($value != $olddata[$cell]) {

          // Determine the employee_id, monday and phase_id variable according to the cellname
          list($employee_id, $monday, $phase_id) = explode("_", $cell);

          $db->query("DELETE FROM weekplanning_planning WHERE employeeid='".$employee_id."' AND monday='".$monday."' AND phaseid='".$phase_id."'");

          // Store the given planningvalue if set and higher than 0
          if ($value > 0)
          {
            atkdebug("This module is ".$this->m_module);
            $id = $db->nextid("weekplanning");
            $db->query("INSERT INTO weekplanning_planning (id, employeeid, monday, phaseid, minutes)
                               VALUES ($id, $employee_id, '$monday', $phase_id, ".($value*60).")");
            $analysis_queue[] = array("phaseid"=>$phase_id, "employeeid"=>$employee_id, "monday"=>$monday);
            atkdebug("This module is now ".$this->m_module);
          }
        }

      }

      // After new situation is stored, perform all notifications at once, so we analyse conflicts
      // only for the complete situation.
      foreach ($analysis_queue as $item)
      {
        $this->notify("update", $item);
      }

      // Redirect to the user to the admin page
      $this->redirect();
    }

    /**
     * Return record descriptor (for use in relations etc.)
     *
     * @return String A template containing fieldnames between brackets.
     */
    function descriptor_def()
    {
      // @todo: replace this with proper descriptor fields.
      return "[id]";
    }

    /**
     * Generic admin actionhandler for the weekplanning module
     *
     * This function implements the call to the specific adminPage functio
     * which is to be implemented in any classes that derive from this
     * class. the adminPage function has a simple interface; it takes no
     * parameters:
     *
     *   function adminPage()
     *
     * if the adminPage is not implemented or the admin action is directly
     * performed on an instance of this class, then an atkerror is thronwn.
     *
     * @param atkAdminHandler Reference to the default atkActionHandler for the admin action (note: this parameter is of no use and may be omitted as the recordlistbehaviour of the default atkAdminHandler isn't what we want)
     */
    function action_admin($handler = null)
    {
      // If adminPage implemented, call it and store the content result.
      $content = $this->adminPage();

      // Add the given content to the page
      $page = &$this->getPage();
      $page = &atkPage::getInstance();
      $page->register_style(moduleDir("weekplanning")."style/style.css");
      $page->addContent($this->renderActionPage("admin", $content));
    }

    /**
     * Generates the HTML content to be displayed when using the admin function
     *
     * @return string HTML Content to be displayed
     */
    function adminPage()
    {
      // Get a reference to the sessionmanager
      global $g_sessionManager;

      // Use the generic style
      $this->addStyle("style.css");

      // Try to get a reference to the User Interface object and raise an error if it fails
      $ui = &$this->getUi();
      if (!is_object($ui))
      {
        atkerror("Failed to create ui object");
        return "";
      }

      $go = $g_sessionManager->pageVar("go");

      // Start composing the output by adding the filterbar as topmost item
      if (!$this->isPrinter())
      {
        $output = $this->getFilterBar();
        if ($go==1)
        {
          $output.= $this->getDateNavBar();
        }
      }

      // Add a newline (vertical spacing)
      $output.= '<br/>';

      // Only get the planning if the filter was set and submitted
      if ($go == 1)
      {
        $output .= $this->getPlanning().'<br/>';
      }

      // Render a box around the generated output
      $boxedoutput = $ui->renderBox(array("title"=>$this->actionTitle('admin'), "content"=>$output));

      // Return the generated boxed output
      return $boxedoutput;
    }

    function isPrinter()
    {
      return (array_key_exists("mode", $this->m_postvars) &&
              $this->m_postvars["mode"]=="print");
    }

    /**
     * Composes relative date-shifted navigation links
     *
     * Composes a date navigation link used to increase or decrease the
     * starting or ending date by a fixed amount depending on the current
     * filterrecord.
     *
     * @param array $record Current filter record
     * @param integer $sd Shift factor for the starting day
     * @param integer $sm Shift factor for the starting month
     * @param integer $sy Shift factor for the starting year
     * @param integer $ed Shift factor for the ending day
     * @param integer $em Shift factor for the ending month
     * @param integer $ey Shift factor for the ending year
     * @param String $linktext Text to show as link
     * @param String $translationmodule Module from which the translation of given linktext can be retrieved
     * @return String HTML Link plus hard space (&nbsp;)
     */
    function getDateNavLink($record, $sd, $sm, $sy, $ed, $em, $ey, $linktext, $translationmodule = "")
    {
      $nmf = dateutil::stamp2arr(mktime(12,0,0,$record["from"]["month"]+$sm, $record["from"]["day"]+$sd, $record["from"]["year"]+$sy));
      $nmt = dateutil::stamp2arr(mktime(12,0,0,$record["to"]["month"]+$em, $record["to"]["day"]+$ed, $record["to"]["year"]+$ey));

      return href("?from[year]=".$nmf["year"]."&from[month]=".$nmf["month"]."&from[day]=".$nmf["day"].
                     "&to[year]=".$nmt["year"]."&to[month]=".$nmt["month"]."&to[day]=".$nmt["day"] , atktext($linktext, $translationmodule))."&nbsp;";
    }

    /**
     * Composes a bar containing links to navigate through the weekplanning by jumping fixed amounts of time back and forward
     *
     * @return String HTML Navigation bar
     */
    function getDateNavBar()
    {
      $record = $this->getFilterRecord();
      $output = $this->getDateNavLink($record, -7, 0, 0, -7, 0, 0, "previousweek");
      $output.= $this->getDateNavLink($record, 0, -1, 0, 0, -1, 0, "previous_month");
      $output.= $this->getDateNavLink($record, 0, 1, 0, 0, 1, 0, "next_month");
      $output.= $this->getDateNavLink($record, 7, 0, 0, 7, 0, 0, "nextweek");
      $output.= $this->getDateNavLink($record, 0, 0, 0, 7, 0, 0, "add_week", "weekplanning");
      $output.= $this->getDateNavLink($record, 0, 0, 0, -7, 0, 0, "remove_week", "weekplanning");
      $output.= href("?mode=print", atktext("report_output_printable"));
      return $output."<br/>";
    }

    /**
     * Formats a week identifier so it can be displayed in a more friendly format
     *
     * @param String $yyyyww Week identifier in yyyyww format
     * @return String Week display value in yyyy-ww format
     */
    function weekFormat($yyyyww)
    {
      return substr($yyyyww, 0, 4)."-".substr($yyyyww, 4);
    }

    /**
     * Calculates the weeknumber, startdate/enddate of the week for each week within the posted from and to values
     *
     * @return array Array containing arrays like "week"=>weeknumber,"startdate"=>startdate,"enddate"=>enddate
     */
    function getWeeksBetween()
    {
      $filterrecord = $this->getFilterRecord();
      // Initialize the weekdata array
      return dateUtil::weeksBetween(dateUtil::arr2str($filterrecord["from"]),
                                       dateUtil::arr2str($filterrecord["to"]));
    }

    function hasAnyUserPrivilege()
    {
      return $this->allowed("any_user");
    }

    function hasAnyProjectPrivilege()
    {
      return $this->allowed("any_project");
    }

    function getFilterParams()
    {
      $record = $this->getFilterRecord();
      return array("from[year]"=>$record["from"]["year"],
                   "from[month]"=>$record["from"]["month"],
                   "from[day]"=>$record["from"]["day"],
                   "to[year]"=>$record["to"]["year"],
                   "to[month]"=>$record["to"]["month"],
                   "to[day]"=>$record["to"]["day"],
                   "go"=>1);
    }

    function getConflicts()
    {
      $cn = &getNode("weekplanning.weekplanningconflictlog");

      if ($cn->allowed("admin"))
      {
        $output = "<b>Conflicten</b>:<br/><br/>";

        // List all present conflicts.
        $conflicts = $cn->selectDb();

        $rl = &atknew("atk.recordlist.atkrecordlist");

        $output.= $rl->render($cn, $conflicts, array(), RL_NO_SEARCH|RL_NO_SORT);

        return $output;
      }
      return "";
    }

    function getContracts($userid, $startdate, $enddate)
    {
      // determine contracthours per week..
      $usercontractsnode = &getNode("employee.usercontracts");
      $whereclause = "usercontract.userid = '$userid' AND startdate <= '$enddate' AND (enddate >= '$startdate' OR enddate IS NULL)";
      $records = $usercontractsnode->selectDb($whereclause, "", "", "", array("uc_hours", "startdate", "enddate"));

      $contracts = array();

      $startstamp = dateUtil::str2stamp($startdate);
      $end = endOfWeek(date("Y-m-d", dateUtil::str2stamp($enddate)));
      $newendstamp = adodb_mktime(0,0,0,substr($end,5,2),substr($end,8,2)+1,substr($end,0,4));
      for ($j=$startstamp;$j<=$newendstamp;$j+=ONEWEEK)
      {
        $weekstamp = weekstamp($j);
        $contracts[$weekstamp] = 0;
        $start_week = dateUtil::startOfWeek(dateUtil::stamp2str($j));
        $end_week = dateUtil::endOfWeek(dateUtil::stamp2str($j));
        for ($i=0;$i<count($records);$i++)
        {
          $recordstartdate = dateutil::arr2str($records[$i]["startdate"]);
          $recordenddate = dateutil::arr2str($records[$i]["enddate"]);
          if ($recordstartdate <= $start_week && ($recordenddate >= $start_week || $recordenddate==NULL))
          {
            $contracts[$weekstamp] += $records[$i]["uc_hours"]*60;
          }
        }
      }
      return $contracts;
    }
    
    /**
     * Get estimate for phase $phase, accumulated for every
     * week in $weeks
     *
     * @param array $phase The phase we are getting the estimate for
     * @param array $weeks Weeks to get the estimate for
     * @return int The estimate costs
     */
    function getEstimate($phase,$weeks,$totaldata,$filterrecord="",&$totalestimate_array,$personid)
    {
      $db = &atkGetDb();
      
      // no billing module, so nothing to estimate
      if (!moduleExists("billing")) return 0;
      
      // no hours planned, so no costs estimated
      if (array_sum($totaldata) == 0) return 0;

      $totalestimate = 0;
      
      $ratenode = &getNode("billing.rate");
      $functionlevelid = ($filterrecord['functionlevel']['id'] && is_numeric($filterrecord['functionlevel']['id']) ? $filterrecord['functionlevel']['id'] : false);
      $projectid = $phase['projectid']['id'];
      $phaseid = $phase['id'];
      
      $result = array_pop($db->getRows("SELECT billing_type,customer FROM project WHERE id='$projectid'"));
      $billingtype = is_numeric($result['billing_type']) && $result['billing_type'] > 0 ? $result['billing_type'] : false;
      $customerid = is_numeric($result['customer']) && $result['customer'] > 0 ? $result['customer'] : false;
      
      foreach($weeks as $week)
      {
        $hours_planned = $totaldata[$week['startdate']];
        if ($hours_planned == 0) continue;
        
        $startdate = substr($week['startdate'],0,4) . "-" . substr($week['startdate'],4,2) . "-" . substr($week['startdate'],6,2);
        $enddate = substr($week['enddate'],0,4) . "-" . substr($week['enddate'],4,2) . "-" . substr($week['enddate'],6,2);
        
        $rate = $ratenode->getRateByParams($projectid,$phaseid,$personid,$startdate,$enddate,$functionlevelid,$billingtype,$customerid);
        $totalestimate += ($hours_planned * $rate);
        $totalestimate_array[$week['startdate']] += ($hours_planned * $rate);
        
        atkdebug("<b>getRateByParams(projectid=$projectid,phaseid=$phaseid,employeeid=$personid,startdate=$startdate,enddate=$enddate,functionlevel=$functionlevelid,billingtype=$billingtype,customerid=$customerid) = $rate</b>");
      }
      
      return $totalestimate;
    }
    
  }
?>
