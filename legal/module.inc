<?php

  $modifiers[] = "employee.employee";
  $modifiers[] = "project.project";
  $modifiers[] = "project.project_personcontact";
  $modifiers[] = "docmanager.document";
  $modifiers[] = "docmanager.documenttype";
  $modifiers[] = "timereg.hours";
  $modifiers[] = "person.title";
  $modifiers[] = "project.select_contact";

  atkimport("atk.atklanguage");
  $al = &atkLanguage::getInstance();
  $al->addOverrideModule("legal");

  class mod_legal extends atkModule
  {

    function getDependencies($mode)
    {
      return array("docmanager");
    }

    function employee_employee_modifier(&$node)
    {
      useattrib("atkattribute");
      $node->add(new atkAttribute("legal_lawyernumber", AF_SEARCHABLE, 5));
      $node->add(new atkAttribute("legal_nextcasenumber", AF_HIDE));
      $node->add(new atkAttribute("legal_informalsignature", AF_HIDE_LIST));
      $node->add(new atkAttribute("legal_formalsignature", AF_HIDE_LIST));
    }

    function sanitizeDoubleChars($string, $checkchar)
    {
      $result = "";
      $char = "";
      for($i=0,$_i=strlen($string); $i<$_i; $i++)
      {
        $oldchar = $char;
        $char = substr($string, $i, 1);
        if (($char != $oldchar) || ($checkchar != $char)) $result .= $char;
      }
      return $result;
    }

    function sanitizeDirName($name)
    {
      $result = "";
      $allowedchars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_. ";
      $replacechar = " ";
      for($i=0,$_i=strlen($name); $i<$_i; $i++)
      {
        $char = substr($name, $i, 1);
        $result .= (strpos($allowedchars, $char) !== false) ? $char : $replacechar;
      }
      return $this->sanitizeDoubleChars($result, ' ');
    }

    function getCustomProjectDocumentDir($casenumber, $defaultdir, $oldstucture=false, $overridecasedescription="", $overrideprojectname="")
    {
      $elements = explode(".", str_replace(" ", ".", $casenumber));

      // If not in the old structure, then add the casetype description and projectname
      if (!$oldstucture)
      {
        // Add the typename to the typenumber
        if (!empty($overridecasedescription))
        {
          $elements[0] .=  " " . trim($this->sanitizeDirName($overridecasedescription));
        }
        else
        {
          $casetypenode = &atkGetNode("legal.casetype");
          $casetypes = $casetypenode->selectDb(sprintf("legal_casetype.number='%s'", $elements[0]), "", "", "", array("description"));
          if ((count($casetypes) > 0) && (!empty($casetypes[0]["description"])))
            $elements[0] .= " " . trim($this->sanitizeDirName($casetypes[0]["description"]));
        }

        // Add the projectname to the number
        if (!empty($overrideprojectname))
        {
          $elements[2] .= " " . trim($this->sanitizeDirName($overrideprojectname));
        }
        else
        {
          $projectnode = &atkGetNode("project.project");
          $projectnode->addFlag(NF_NO_FILTER);
          $projects = $projectnode->selectDb(sprintf("project.legal_casenumber='%s'", $casenumber), "", "", "", array("name"));
          if ((count($projects) > 0) && (!empty($projects[0]["name"])))
            $elements[2] .= " " . trim($this->sanitizeDirName($projects[0]["name"]));
        }
      }

      $result = sprintf("%s%s/%s/%s/%s/", $defaultdir, $elements[3], $elements[0], $elements[1], $elements[2]);

      return $result;
    }

    function getCustomDocumentDir($rec, $defaultdir, $oldstucture=false)
    {
      // Determine specific directory to store documents related to a project
      if ($rec["master"] == "project.project")
      {
        $masterid = $rec["master_id"];
        $db = &atkGetDb();
        $casenumbers = $db->getRows("SELECT project.legal_casenumber FROM project WHERE project.id='$masterid'");
        if (count($casenumbers) > 0)
        {
          $legaldoctypes = $this->getLegalDocTypes();
          $projectdoctype = atktext($legaldoctypes[isset($rec["legal_projectdoctype"]) ? $rec["legal_projectdoctype"] : 0], "legal", "", atkconfig("language"));
          return $this->getCustomProjectDocumentDir($casenumbers[0]["legal_casenumber"], $defaultdir, $oldstucture) . $projectdoctype . '/';
        }
      }

      // In other cases, return the default dir
      return $defaultdir;
    }

    function getBillNumber(&$record)
    {
      // Get a reference to the database
      $db = &atkGetDb();

      // Get the lawyer initials
      $projectid = $record["project_id"]["projectid"];
      if (is_array($projectid)) $projectid = $projectid["id"]; //sometimes the id is put within yet another array
      $lawyerinitials = $db->getrows("SELECT `person`.`legal_lawyernumber`
                                      FROM `project`
                                      LEFT JOIN `person` ON `project`.`coordinator` = `person`.`id`
                                      WHERE `project`.`id`='$projectid'");

      // Just before we add the bill to the database, we generate a unique number.
      $postfix = sprintf(".%02d.%s", date("y"), str_replace(".", "", $lawyerinitials[0]["legal_lawyernumber"]));
      $cnt = $db->getrows("SELECT bill_nr FROM billing_bill ORDER BY bill_nr DESC LIMIT 1");
      $billnr = substr($cnt[0]["bill_nr"], 0, 4)+1;

      // Set the bill_nr to the newly generated number
      return sprintf("%04d", $billnr).$postfix;
    }

    function _getproject_persontypes()
    {
      return array(
        "client",
        "counterparty",
        "thirdparties",
      );
    }

    function project_project_personcontact_modifier(&$node)
    {
      useattrib("atklistattribute");
      $node->add(new atkListAttribute("legal_type", $this->_getproject_persontypes(), "", AF_HIDE_LIST));
      $node->add(new atkAttribute("legal_reference"), 10);
    }

    function project_select_contact_modifier(&$node)
    {
      $node->add(new atkAttribute("openingswords", AF_HIDE_LIST));
    }

    function getLegalDocTypes()
    {
      return array(
        0 => "correspondence",
        1 => "pieces",
        2 => "other",
      );
    }

    function docmanager_document_modifier(&$node)
    {
      $node->add(new atkAttribute("legal_projectdoctype", AF_HIDE|AF_FORCE_LOAD));
    }

    function getprojectdoctypevisibilityscript()
    {
      return "if (document.entryform.master.value == 'project.project_personcontact') { showAttrib('legal_projectdoctype'); } else { hideAttrib('legal_projectdoctype'); }";
    }

    function getDocmanagerCopyColumns()
    {
      return array("legal_projectdoctype");
    }

    function docmanager_documenttype_modifier(&$node)
    {
      useattrib("legal.legalprojectdoctype");
      $node->add(new legalprojectdoctype("legal_projectdoctype", AF_FORCE_LOAD));

      $masterattr = &$node->getAttribute("master");
      $masterattr->addOnChangeHandler($this->getprojectdoctypevisibilityscript());

    }


    function project_project_modifier(&$node)
    {
      // Add a relation to the legal projecttype to the projectnode
      $node->add(new atkManyToOneRelation("legal_type", "legal.casetype", AF_OBLIGATORY|AF_HIDE_LIST|AF_RELATION_AUTOLINK));
      $node->addFlag(NF_TRACK_CHANGES);

      // Show the coordinator in the add screen and make it obligatory
      $coordinatorattr = &$node->getAttribute("coordinator");
      $coordinatorattr->addFlag(AF_OBLIGATORY);
      $coordinatorattr->removeFlag(AF_HIDE_ADD);

      // Hide the abbreviation field and make it non-obligatory (legal users use the casenumber to identify cases/projects)
      $abbreviationattr = &$node->getAttribute("abbreviation");
      $abbreviationattr->addFlag(AF_HIDE);
      $abbreviationattr->removeFlag(AF_OBLIGATORY);

      // Add the case number
      $node->add(new atkAttribute("legal_casenumber", AF_HIDE_ADD|AF_READONLY|AF_SEARCHABLE));

      // Add a relation to the awarded points to the projectnode
      $node->add(new atkOneToManyRelation("points","legal.points","projectid",AF_HIDE_LIST|AF_TOTAL), "finance");

      // Add the caseadd listener (sets the casenumber as soon as a case is added)
      atkimport("module.legal.addcaselistener");
      atkimport("module.legal.updatecaselistener");
      $node->addListener(new addCaseListener(array("save")));
      $node->addListener(new updateCaseListener(array("update")));

      // Get all project_person types
      $project_persontypes = $this->_getproject_persontypes();

      // First of all, get a reference to the existing generic contacts relation and remove it
      $attr = &$node->getAttribute("contacts");
      $node->remove("contacts");

      // Start looping through all roles and add a new tab for each of them
      foreach ($project_persontypes as $project_persontype)
      {
        // Translate the project_person type to a name
        $tabname = atktext($project_persontype, "legal");

        // Clone the contacts tab
        if (version_compare(phpversion(), '5.0') < 0)
          $newattr = $attr;
        else
          $newattr = clone($attr);

        // Set it's name to the project_person type
        $newattr->m_name = $project_persontype;

        // Add a destination filter and so only contacts are shown that belong in the current tab
        $newattr->setDestinationFilter("project_person.legal_type='" . $project_persontype . "'");

        $newattr->removeFlag(AF_OBLIGATORY);

        // Add the attribute to a tab named after the role
        $node->add($newattr, $tabname);
      }

      // Add a legal_projectdir attribute to maintain the folder in which the documents are stored
      $node->add(new atkAttribute("legal_projectfolder", AF_HIDE|AF_FORCE_LOAD));

      // First of all, get a reference to the existing generic contacts relation and remove it
      $attr = &$node->getAttribute("documents");
      $node->remove("documents");

      $project_documenttypes = $this->getLegalDocTypes();

      // Start looping through all roles and add a new tab for each of them
      foreach ($project_documenttypes as $id => $project_documenttype)
      {
        // Translate the project_person type to a name
        $tabname = atktext($project_documenttype, "legal");

        // Clone the documentmanager attribute
        if (version_compare(phpversion(), '5.0') < 0)
          $newattr = $attr;
        else
          $newattr = clone($attr);

        // Set it's name to the project_person type
        $newattr->m_name = $project_documenttype;

        // Add a destination filter so only documents are shown that belong in the current tab
        $newattr->setDestinationFilter("legal_projectdoctype='$id'");

        // Add the attribute to a tab named after the role
        $node->add($newattr, $tabname);
      }
    }

    function timereg_hours_modifier(&$node)
    {
      atkimport("module.legal.hourslistener");
      $node->addListener(new pointHoursListener(array("save")));
    }

    function person_title_modifier(&$node)
    {
      $titleattr = &$node->getAttribute("title");
      $titleattr->setAttribSize(50);
    }
  }

?>