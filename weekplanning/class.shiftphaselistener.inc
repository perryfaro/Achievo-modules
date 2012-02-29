<?php
  /**
   * shiftPhaseListener class file
   *
   * @package achievo_modules
   * @subpackage weekplanning
   *
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2005 Ibuildings.nl BV
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision: 3632 $
   * $Id: class.shiftphaselistener.inc 3632 2005-12-15 15:02:45Z guido $
   */

  /**
   * @internal Load used classes
   */
  atkimport("atk.utils.atkactionlistener");

  /**
   * The shiftPhaseListener class
   *
   * @author guido <guido@ibuildings.nl>
   * @package achievo_modules
   * @subpackage weekplanning
   */
  class shiftPhaseListener extends atkActionListener
  {

    /**
     * shiftPhaseListener constructor
     */
    function shiftPhaseListener($actionfilter=array())
    {
      $this->atkActionListener($actionfilter);
    }

    /**
     * Set the owning node of the listener and add the NF_TRACK_CHANGES flag to it
     *
     * When using atkNode::addListener to add a listener to a node it is not
     * necessary to call this method as addListener will do that for you.
     *
     * @param atkNode $node The node to set as owner
     */
    function setNode(&$node)
    {
      // Call the setNode on the parent class
      parent::setNode($node);

      // Add the NF_TRACK_CHANGES flag to the node this listener is
      // registered in; this is required to see the difference between the
      // old and new record when updating a phase
      $node->addFlag(NF_TRACK_CHANGES);
    }

    /**
     * Notification handler function to be triggered when the action we're listening to is executed
     *
     * @param String $action Name of the action that was executed
     * @param Array $record The record being updated
     */
    function actionPerformed($action, $record)
    {
      // This handler should only be called when updating a phase
      if ($action != "update")
      {
        atkerror("Cannot call the shiftPhaseListener when performing a '$action' action on the '" . $this->m_node . "' node");
        return;
      }

      // Get the new startdate
      $newstartdate = adodb_mktime(0, 0, 0, $record["startdate"]["month"], $record["startdate"]["day"], $record["startdate"]["year"]);

      // Get the old startdate
      $oldrecord = $record["atkorgrec"];
      $oldstartdate = adodb_mktime(0, 0, 0, $oldrecord["startdate"]["month"], $oldrecord["startdate"]["day"], $oldrecord["startdate"]["year"]);

      // Calculate the number of weeks difference between the oldstartdate and newstartdate
      $daydifference = round(($newstartdate -  $oldstartdate) / 86400);

      // Calculate the number of weeks to shift the phases rounded towards zero
      $weekdifference = ($daydifference >= 0) ? floor($daydifference / 7) : ceil($daydifference / 7);

      // Only shift the weekplanning for this phase if the weekdifference is not zero
      if ($weekdifference != 0)
      {
        atkdebug("Shifting the weekplanning for this phase by $weekdifference weeks.");

        // Calculate the weekdifference in seconds
        $weekdifferenceinseconds = $weekdifference * 7 * 86400;

        // Shift the planning the given number of weeks
        $weekplanningnode = &getNode("weekplanning.weekplanning") or atkerror("Could not load weekplanning node");

        // Get all weekplanning records for the updated phase
        $weekplanningrecords = $weekplanningnode->selectDb("weekplanning_planning.phaseid = '" . $record["id"] . "'");

        // Shift the monday of each record by $weekdifferenceinseconds
        foreach($weekplanningrecords as $weekplanningrecord)
        {
          // Get the old monday, convert it to seconds, add the
          // weekdifference in seconds, convert back to an array for db
          // storage and put it back in the weekplanning record
          $oldmondayarray = $weekplanningrecord["monday"];
          $oldmonday = adodb_mktime(0, 0, 0, $oldmondayarray["month"], $oldmondayarray["day"], $oldmondayarray["year"]);
          $newmonday = $oldmonday + $weekdifferenceinseconds;
          $newmondayarray = array("month" => date("m", $newmonday), "day" => date("d", $newmonday), "year" => date("Y", $newmonday));
          $weekplanningrecord["monday"] = $newmondayarray;

          // Update the weekplanning record in the database
          $weekplanningnode->updateDb($weekplanningrecord);
        }
      }
      else
      {
        atkdebug("Startdate not changed or changed by less than 7 days. Weekplanning will not be shifted.");
      }
    }

  }
?>