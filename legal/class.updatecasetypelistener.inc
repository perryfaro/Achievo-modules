<?php
  /**
   * One line description of the file
   *
   * Full description of the file
   *
   * @package achievo_modules
   * @subpackage legal
   *
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2006 Ibuildings.nl BV
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision: 3236 $
   * $Id: class.updatecasetypelistener.inc 3236 2006-07-25 09:21:26Z guido $
   */

  /**
   * @internal Load used classes
   */
  atkimport("atk.utils.atkactionlistener");

  /**
   * updateCasetypeListener class
   *
   * @author guido <guido@ibuildings.nl>
   * @package achievo_modules
   * @subpackage legal
   */
  class updateCasetypeListener extends atkActionListener
  {
    /**
     * updateCasetypeListener constructor
     */
    function updateCasetypeListener($actionfilter=array())
    {
      $this->atkActionListener($actionfilter);
    }

    function tryRename($employeedir,$olddir,$newdir)
    {
atk_var_dump(array(
"employeedir"=>$employeedir,
"olddir"=>$olddir,
"newdir"=>$newdir,
));
      if (file_exists($employeedir.$olddir))
        rename($employeedir.$olddir, $employeedir.$newdir);
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
        atkerror("Cannot call the updateCasetypeListener when performing a '$action' action on the '" . $this->m_node->m_type . "' node");
        return;
      }

      // Description for directories
      $olddir = sprintf('%02d', atkArrayNvl($record["atkorgrec"], "number")) . ' ' . atkArrayNvl($record["atkorgrec"], "description");
      $newdir = sprintf('%02d', atkArrayNvl($record, "number")) . ' ' . atkArrayNvl($record, "description");


      if (($olddir!=$newdir) && moduleExists("docmanager"))
      {
        $legaldir = atkconfig("documentpath", "documents/");
        if (substr($legaldir, -1) != "/")
          $legaldir .= "/";

        $employeenode = &atkGetNode("employee.employee");
        $employees = $employeenode->selectDb("(initials IS NOT NULL)", "", "", "", array("initials"));
        foreach ($employees as $employee)
        {
          $initials = str_replace(".", "", $employee["initials"]);
          if (!empty($initials))
            $this->tryRename($legaldir.$initials.'/', $olddir, $newdir);
        }
        $this->tryRename($legaldir, $olddir, $newdir);

      }
    }
  }
?>