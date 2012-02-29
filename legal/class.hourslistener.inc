<?php
  /**
   * Listener for hours
   *
   * After saving hours, a message about the new points balance is put in the atkMessageQueue
   *
   * @package achievo_modules
   * @subpackage legal
   *
   * @author patrick <patrick@ibuildings.nl>
   *
   * @copyright (c) 2006 Ibuildings.nl BV
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision: 3237 $
   * $Id: class.hourslistener.inc 3237 2006-07-26 14:43:35Z guido $
   */

  /**
   * @internal Load used classes
   */
  atkimport("atk.utils.atkactionlistener");
  atkimport("atk.utils.atkmessagequeue");

  /**
   * hoursListener class
   *
   * @author patrick <patrick@ibuildings.nl>
   * @package achievo_modules
   * @subpackage legal
   */
  class pointHoursListener extends atkActionListener
  {
    /**
     * hoursListener constructor
     */
    function pointHoursListener($actionfilter=array())
    {
      $this->atkActionListener($actionfilter);
    }

    /**
     * Notification handler function to be triggered when the action we're listening to is executed
     *
     * @param String $action Name of the action that was executed
     * @param Array $record The record being updated
     */
    function actionPerformed($action, $record)
    {
      // Determine the phaseid for the registered hours
      $phaseid = $record["phaseid"]["phaseid"];

      // Get the phase from the database in order to determine the projectid
      $phasenode = &getNode("project.phase");
      $phaserecord = $phasenode->selectDb("`phase`.`id`='$phaseid'");
      $projectid = $phaserecord[0]["projectid"]["id"];

      // Get a reference to the database:
      $db = &atkGetDb();

      // get points
      $queryresult = $db->getRows("SELECT SUM(pt.points) AS points
                                   FROM project pr
                                   LEFT JOIN legal_points pt ON pr.id = pt.projectid
                                   WHERE pr.id = " . $projectid);
      $points = $queryresult[0]["points"];

      // only calculate balance if projects has points
      if ($points == 0) return;

      // calculate point balance
      $queryresult = $db->getRows("SELECT SUM(CASE WHEN ISNULL(hr.time) THEN 0 ELSE hr.time END) AS used,
                                          pr.name
                                   FROM project pr
                                   LEFT JOIN phase ph ON pr.id = ph.projectid
                                   LEFT JOIN hours hr ON ph.id = hr.phaseid
                                   WHERE pr.id = " . $projectid .
                                  " GROUP BY pr.id, pr.name");
      $name = $queryresult[0]["name"];
      $used = $queryresult[0]["used"];
      $remainder = round((($points) - $used), 2);

      // Use the points display to create the display for the amount of remaining points:
      useattrib("legal.legalpointsattribute");
      $pointsattr = new legalPointsAttribute("dummy");
      $remainderdisplay = $pointsattr->display(array("dummy"=>$remainder));

      // add message to atkMessageQueue
      atkMessageQueue::addMessage(sprintf(atktext("message_points_balance"), $name, $remainderdisplay));
    }
  }
?>