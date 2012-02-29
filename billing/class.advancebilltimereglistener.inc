<?php
  /**
   * advanceBillTimeregListener class file
   *
   * @package achievo_modules
   * @subpackage billing
   *
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2006 Ibuildings.nl BV
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision: 2898 $
   * $Id: class.advancebilltimereglistener.inc 2898 2007-03-20 14:12:32Z guido $
   */

  /**
   * @internal Load used classes
   */
  atkimport("atk.utils.atkactionlistener");

  /**
   * The advanceBillTimeregListener class.
   *
   * @author guido <guido@ibuildings.nl>
   * @package achievo_modules
   * @subpackage billing
   */
  class advanceBillTimeregListener extends atkActionListener
  {

    /**
     * Notify the listener of an action on a record.
     *
     * This method should be overriden in custom action listeners, to catch
     * the action event.
     *
     * @param String $action The action being performed
     * @param array $record The record on which the action is performed
     */
    function actionPerformed($action, $record)
    {
      // Show an error if called on wrong node/action
      if (($this->m_node->m_type != "hours") || ($action != "save"))
      {
        atkerror("advanceBillTimeregListener::actionPerformed(): The advanceBillTimeregListener can not be triggered when performing a $action action on the {$this->m_node->m_type} node.");
        return;
      }

      // Use the bill node (for instance, we use it's BILL_ADVANCE constant)
      atkimport("module.billing.bill");

      // Determine the phaseid for the registered hours
      $phaseid = $record["phaseid"]["phaseid"];

      // Get the phase from the database in order to determine the projectid
      $phasenode = &getNode("project.phase");
      $phaserecord = $phasenode->selectDb("`phase`.`id`='$phaseid'");
      $projectid = $phaserecord[0]["projectid"]["id"];

      // Search for advance bills linked to project
      $billnode = &getNode("billing.bill");
      $bills = $billnode->selectDb(sprintf("`billing_bill`.`project_id`='%s' AND `billing_bill`.`billtype`='%s'", $projectid, BILL_ADVANCE));

      // Get a database object reference
      $db = &atkGetDb();

      // Get billing amount for registered hours
      $hours = $db->getRows("SELECT `hours`.`billing_rate`, `hours`.`time` FROM `hours` WHERE `hours`.`id`='{$record["id"]}'");
      $currentregistrationamount = (($hours[0]["time"] / 60) * $hours[0]["billing_rate"]);

      // Do not process
      foreach ($bills as $bill)
      {
        // Use the first one to link the hours to
        $billid = $bill["id"];

        // Get the bill lines linked to the node
        $billlinenode = &getNode("billing.bill_line");
        $billlines = $billlinenode->selectDb("`billing_bill_line`.`bill_id`='$billid'");

        // If at least one bill line found,
        foreach ($billlines as $billline)
        {

          // Get the bill line id
          $billlineid = $billline["id"];

          // Determine the available amount using the fixed amount of this bill line
          $availableamount = $billline["fixed_amount"];

          // And subtracting all billing_rates of hour registrations already assigned to this billline
          $hourslinkedtocurrentbillline = $db->getRows("SELECT `hours`.`billing_rate`, `hours`.`time` FROM `hours` WHERE `hours`.`billing_bill_line_id`='$billlineid'");
          foreach ($hourslinkedtocurrentbillline as $hourlinkedtocurrentbillline)
            $availableamount -= (($hourlinkedtocurrentbillline["time"] / 60) * $hourlinkedtocurrentbillline["billing_rate"]);

          // Only update the database if there's space left in the current bill line
          if ($availableamount >= $currentregistrationamount)
          {

            // And update it in the database
            $db = &atkGetDb();
            $db->query("UPDATE `hours` SET `billing_bill_line_id`='$billlineid' WHERE `hours`.`id`='{$record["id"]}'");

            // Stop searching for remaining bills or bill lines
            return;
          }
        }
      }
    }
  }
?>
