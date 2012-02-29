<?php
  /**
   * weekmastergantt node class file
   *
   * @package achievo_modules
   * @subpackage weekplanning
   *
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2005 Ibuildings.nl BV
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision: 3684 $
   * $Id: class.weekmastergantt.inc 3684 2006-07-17 16:02:57Z guido $
   */

  /**
   * @internal includes
   */
  atkimport("module.project.mastergantt");

  /**
   * The node class for mastergantt
   *
   * @author guido <guido@ibuildings.nl>
   * @package achievo_modules
   * @subpackage weekplanning
   */
  class weekmastergantt extends mastergantt
  {

    /**
     * weekmastergantt constructor
     */
    function weekmastergantt()
    {
      $this->mastergantt("weekmastergantt");
    }

    /**
     * Get the total planned hours per week for the given project
     *
     * @param int $projectid Project id for which the weekplanning totals should be retrieved
     * @return array Array with week=>minutes key/value pairs
     */
    function getProjectWeekplanningTotals($projectid, $from, $to)
    {
      // Get a db reference
      $db = &atkGetDb();

      // Collect a list of phaseids
      $phaseidarrayresult = $db->getRows("SELECT
                                            phase.id AS phaseid
                                          FROM
                                            phase
                                          LEFT JOIN
                                            project ON phase.projectid = project.id
                                          WHERE
                                            project.id = '$projectid'");

      // Initalize the array in which the totals will be stored
      $plannedperweek = array();

      // If any phases found, start to retrieve and process their weekplanning data
      if (count($phaseidarrayresult) > 0)
      {

        // Convert the phaseids to a comma seperated list
        $phaseidarray = array();
        for($i=0,$_i=count($phaseidarrayresult); $i<$_i; $i++)
          $phaseidarray[] = $phaseidarrayresult[$i]["phaseid"];
        $phaseidstring = implode(",", $phaseidarray);

        // Read all planned hours for the given phase ids, grouped per week
        $plannedperweekresults = $db->getRows("SELECT
                                                 SUM(weekplanning_planning.minutes) AS totalminutes,
                                                 weekplanning_planning.monday AS weekstart
                                               FROM
                                                 weekplanning_planning
                                               WHERE
                                                 weekplanning_planning.phaseid IN ($phaseidstring)
                                                 AND monday >= '$from'
                                                 AND monday <= '$to'
                                               GROUP BY
                                                 weekplanning_planning.monday");

        foreach($plannedperweekresults as $plannedperweekresult)
        {
          $plannedperweek[$plannedperweekresult["weekstart"]] = $plannedperweekresult["totalminutes"];
        }
      }

      // Return the gathered weekplanning totals for the given project
      return $plannedperweek;
    }

    /**
     * Adds weekplanning data to each project
     *
     * @param unknown_type $gantt
     * @param unknown_type $from
     * @param unknown_type $to
     */
    function addWeekplanningTotalsToGantt(&$gantt, $from, $to)
    {
      $projectids = array_keys($gantt);
      for($i=0,$_i=count($gantt); $i<$_i; $i++)
      {
        $gantt[$projectids[$i]]['weekplanning'] = $this->getProjectWeekplanningTotals($projectids[$i], $from, $to);
      }
    }

//    function getPlanned($projectid)
//    {
//      $db = &atkGetDb();
//      $rows = $db->getrows("SELECT sum(pl.minutes) as planned FROM weekplanning_planning pl LEFT JOIN phase ph ON pl.phaseid=ph.id
//                              WHERE ph.projectid=".$projectid);
//      return $rows[0]["planned"];
//    }

    function drawProject(&$graph, $index, $project, $from, $to, $plannedbooked)
    {
      // Projects that extent the selected period should be cut off.
      if ($project['startdate']<$from) $project['startdate']=$from;
      if ($project['enddate']>$to || $project['enddate']=='') $project['enddate']=$to;

      // Get the colornode
      $colornode = &atkGetNode("project.mastergantt_colorconfig");

      // Add the project bar
      $name = sprintf('%s [%s]', $project["name"], time_format($project[$plannedbooked], true));
      $activity = new GanttBar($index, $name, $project['startdate'], $project['enddate'], "");
      $activity->SetPattern(BAND_SOLID, '#cccccc');
      $activity->SetHeight(14);
      $activity->SetFillColor('#cccccc');
      $graph->add($activity);

      if ($plannedbooked == "planned")
      {
        // Add weekplanning bars for each week
        foreach ($project["weekplanning"] as $monday => $minutes)
        {
          $caption = "";//"[".time_format($minutes, true)."]";
          $sunday = dateutil::stamp2str(dateutil::str2stamp(str_replace("-","",$monday))+(6*86400));
          $activity = new GanttBar($index, $name, $monday, $sunday, $caption);

          $colorbase = $minutes;

          $color = $colornode->getColor($colorbase/60);
          $activity->SetPattern(BAND_SOLID, $color);
          $activity->SetHeight(10);
          $activity->SetFillColor($color);
          $graph->add($activity);
        }
      }
    }

    function action_planninggraph(&$handler)
    {
      $params = $this->getParams();

      $gant = $this->getGant($params["from"], $params["to"], $params["coordinator"]);

      $this->addWeekplanningTotalsToGantt($gant, $params["from"], $params["to"]);

      usort($gant, array("mastergantt", "cmp"));

      $this->showGraph($gant, $params["from"], $params["to"], $params["plannedbooked"]);
    }
  }
?>