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
   * $Id: class.updatecaselistener.inc 3236 2006-07-25 09:21:26Z guido $
   */

  /**
   * @internal Load used classes
   */
  atkimport("atk.utils.atkactionlistener");

  /**
   * updateCaseListener class
   *
   * @author guido <guido@ibuildings.nl>
   * @package achievo_modules
   * @subpackage legal
   */
  class updateCaseListener extends atkActionListener
  {
    /**
     * updateCaseListener constructor
     */
    function updateCaseListener($actionfilter=array())
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
      // This handler should only be called when updating a phase
      if ($action != "update")
      {
        atkerror("Cannot call the updateCaseListener when performing a '$action' action on the '" . $this->m_node->m_type . "' node");
        return;
      }

      $oldname = atkArrayNvl($record["atkorgrec"], "name");
      $newname = atkArrayNvl($record, "name");
      $casenumber = atkArrayNvl($record, "legal_casenumber");

      if (!empty($oldname) && !empty($newname) && moduleExists("docmanager"))
      {
        $legalmod = &getModule("legal");
        $olddir = $legalmod->getCustomProjectDocumentDir($casenumber, "", false, "", $oldname);
        $newdir = $legalmod->getCustomProjectDocumentDir($casenumber, "", false, "", $newname);

        // Strip ending slash from dir.
        if (substr($olddir, -1) == "/") $olddir = substr($olddir, 0, strlen($olddir)-1);
        if (substr($newdir, -1) == "/") $newdir = substr($newdir, 0, strlen($newdir)-1);

        if ($olddir!=$newdir)
        {
          $legaldir = atkconfig("documentpath", "documents/");
          if (substr($legaldir, -1) != "/")
            $legaldir .= "/";
          rename($legaldir.$olddir, $legaldir.$newdir);
        }

      }
    }
  }
?>