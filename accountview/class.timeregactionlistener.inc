<?php
  /**
   * timeregActionListener class file
   *
   * @package achievo_modules
   * @subpackage accountview
   *
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2006 Ibuildings.nl BV
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision: 2502 $
   * $Id: class.timeregactionlistener.inc 2502 2006-05-03 00:22:21Z guido $
   */

  /**
   * The class for timeregActionListener.
   *
   * @author guido <guido@ibuildings.nl>
   * @package achievo_modules
   * @subpackage accountview
   */
  class timeregActionListener extends atkActionListener
  {

    /**
     * timeregActionListener constructor
     */
    function timeregActionListener($actionfilter=array())
    {
      $this->atkActionListener($actionfilter);
    }

    /**
     * Notification handler function to be triggered when the action we're listening to is executed
     *
     * @param String $action Name of the action that was executed
     * @param Array $record The record being saved/updated
     */
    function actionPerformed($action, $record)
    {
      // Return if phaseid unknown
      if (empty($record["phaseid"]["phaseid"]))
      {
        atkerror("timeregActionListener::actionPerformed: Unknown phaseid");
        return;
      }

      // Return if activitydate unknown
      if (empty($record["activitydate"]))
      {
        atkerror("timeregActionListener::actionPerformed: Unknown activitydate");
        return;
      }

      // Try to select the projectid from the phase in the database
      $phasenode = &atkGetNode("project.phase");
      $phaseselector = sprintf("`phase`.`id`='%d'", $record["phaseid"]["phaseid"]);
      $phases = $phasenode->selectDb($phaseselector, "", "", "", array("projectid"));

      // Return if phase not found
      if (count($phases) == 0)
      {
        atkerror("timeregActionListener::actionPerformed: Given phase not found");
        return;
      }

      // Add the projectdayneedsupdate record to db if it doesn't exist yet
      $projectdayneedsupdatenode = &getNode("accountview.projectdayneedsupdate");
      $projectdayneedsupdatenode->setProjectDayNeedsUpdate($phases[0]["projectid"], $record["activitydate"]);
    }

  }

?>