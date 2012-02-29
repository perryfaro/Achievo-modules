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
   * @version $Revision: 3232 $
   * $Id: class.addcaselistener.inc 3232 2006-07-07 18:32:01Z guido $
   */

  /**
   * @internal Load used classes
   */
  atkimport("atk.utils.atkactionlistener");

  /**
   * addCaseListener class
   *
   * @author guido <guido@ibuildings.nl>
   * @package achievo_modules
   * @subpackage legal
   */
  class addCaseListener extends atkActionListener
  {
    /**
     * addCaseListener constructor
     */
    function addCaseListener($actionfilter=array())
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
      if ($action != "save")
      {
        atkerror("Cannot call the addCaseListener when performing a '$action' action on the '" . $this->m_node->m_type . "' node");
        return;
      }

      // Get a reference to the database:
      $db = &atkGetDb();

      // Read the person initials, person id, casetypenumber and person nextcasenumber
      $queryresult = $db->getRows("SELECT person.initials AS initials,
                                          project.coordinator AS personid,
                                          legal_casetype.number AS number,
                                          person.legal_nextcasenumber AS nextcasenumber
                                   FROM project LEFT JOIN person ON project.coordinator = person.id
                                   LEFT JOIN legal_casetype ON project.legal_type = legal_casetype.id
                                   WHERE project.id=".$record["id"]);

      // Get the initials (but leave out the dots)
      $initials = str_replace(".", "", $queryresult[0]["initials"]);

      // Get the typenumber
      $typenumber = $queryresult[0]["number"];

      // Get the per-employee case number with padding zero's in case the number is smaller than 100
      $number = $queryresult[0]["nextcasenumber"];

      // Set the new nextcasenumber for the lawyer if found
      if (!is_null($queryresult[0]["personid"]))
        $db->query("update person set legal_nextcasenumber='".($number+1)."' where id=".$queryresult[0]["personid"]);

      // Get a 2-digit representation of the current year
      $year = date("y");

      // Determine the casenumber for the recently added project
      $newcasenumber = sprintf("%02d.%02d.%03d %s", $typenumber, $year, $number, $initials);

      // Set the case number in the database
      $db->query("update project set legal_casenumber='$newcasenumber' where id=".$record["id"]);

      // Only do this if the docmanager is installed
      if (moduleExists("docmanager"))
      {

        // Get the home path for the directory structure
        $legalmod = &getModule("legal");
        $subdirstring = $legalmod->getCustomProjectDocumentDir($newcasenumber, "");
        $subdirs = explode('/', $subdirstring);

        $legaldir = atkconfig("documentpath", "documents/");

        // Make all subdirs one by one
        foreach ($subdirs as $dir)
        {
          // If the subdir doesn't and with a /, add it
          if (substr($legaldir, -1) != "/")
            $legaldir .= "/";

          // Add the current subdir component
          $legaldir .= $dir;

          // Make the folder if not yet existing
          if (!file_exists($legaldir))
            if (mkdir($legaldir))
              chmod("$legaldir", 02770);
        }

        // Get the subfolders that should be made in the case folder
        $legalmodule = &getModule("legal");
        $legaldoctypes = $legalmodule->getLegalDocTypes();

        // Make the subfolders
        foreach ($legaldoctypes as $legaldoctype)
        {
          $legaldoctypetranslation = atktext($legaldoctype, "legal", "", atkconfig("language"));
          if (!file_exists("$legaldir/$legaldoctypetranslation"))
            if (mkdir("$legaldir/$legaldoctypetranslation"))
              chmod("$legaldir/$legaldoctypetranslation", 02770);
        }
      }
    }
  }
?>