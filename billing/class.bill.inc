<?php
  /**
   * bill node file
   *
   * @package achievo_modules
   * @subpackage billing
   *
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2006 Ibuildings.nl BV
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision: 2901 $
   * $Id: class.bill.inc 2901 2007-05-22 13:49:04Z guido $
   */

  // Load used classes
  useattrib("atkdateattribute");
  useattrib("atktextattribute");
  useattrib("atkdummyattribute");
  userelation("atkmanytoonerelation");
  userelation("atkonetomanyrelation");
  useattrib("atklistattribute");
  useattrib("atkCurrencyAttribute");
  atkimport("module.billing.attributes.totalbillprice");

  // Define some configuration defaults
  define("DEFAULT_TAX_SYMBOL", "BTW");
  define("DEFAULT_TAX_INCLUDED", False);
  define("DEFAULT_TAX_RATE", 19);

  // Define the Bill types
  define("BILL_NORMAL", 0);
  define("BILL_ADVANCE", 1);
  define("BILL_CREDIT", 2);

  /**
   * bill class
   *
   * @author guido <guido@ibuildings.nl>
   * @package achievo_modules
   * @subpackage billing
   */
  class bill extends atkNode
  {

    /**
     * Constructor
     */
    function bill($name="bill", $flags=0)
    {
      // Call the parent constructor
      $this->atkNode($name, $flags|NF_EDITAFTERADD);

      // Add all attributes and relations
      $this->add(new atkAttribute("id" ,AF_AUTOKEY));
$p = &$this->add(new atkManyToOneRelation("project_id", "project.project", AF_READONLY_EDIT|AF_SEARCHABLE|AF_OBLIGATORY|AF_LARGE));
      $this->add(new atkManyToOneRelation("contactperson", "project.select_contact", AF_READONLY_EDIT|AF_SEARCHABLE|AF_OBLIGATORY));
      $p->addDependee("contactperson");


      $this->add(new atkDateAttribute("creation_date",AF_OBLIGATORY|AF_READONLY|AF_HIDE_ADD));
      $this->add(new atkDateAttribute("send_date","","",0,0,AF_HIDE_LIST|AF_HIDE_ADD));
      $this->add(new atkDateAttribute("expire_date", AF_OBLIGATORY|AF_HIDE_ADD));
      $this->add(new atkManyToOneRelation("owner","employee.employee", AF_OBLIGATORY|AF_READONLY|AF_HIDE_ADD));
      $this->add(new atkTextAttribute("description", AF_HIDE_ADD|AF_HIDE_LIST));
      $this->add(new atkDummyAttribute("fixed_price","",AF_HIDE_ADD|AF_HIDE_LIST));
      $this->add(new atkAttribute("bill_nr" ,AF_HIDE_ADD|AF_READONLY_EDIT|AF_SEARCHABLE, 15));
      $this->add(new atkOneToManyRelation("bill_line","billing.bill_line","bill_id",AF_HIDE_LIST|AF_CASCADE_DELETE));
      $states = array("new", "final", "sent", "cancelled", "paid");
      $this->add(new atkListAttribute("status", $states, "", AF_HIDE_ADD|AF_READONLY_EDIT|AF_SEARCHABLE));
      $this->add(new atkListAttribute("billtype", array("normal", "advance", "credit"), array(BILL_NORMAL, BILL_ADVANCE, BILL_CREDIT), AF_OBLIGATORY|AF_READONLY_EDIT|AF_FORCE_LOAD));
      $this->add(new totalBillPriceAttribute("billtotal", AF_TOTAL));
      $this->add(new atkOneToManyRelation("bill_reminder","billing.bill_reminder_number","bill_id",AF_HIDE_LIST));

      // Set the table
      $this->setTable("billing_bill");
      $this->setOrder("billing_bill.creation_date DESC");

      // Set some security mappings
      $this->m_securityMap["document"] = "view";
      $this->m_securityMap["setstate"] = "edit";
    }

    /**
     * Determines the default values to be used when adding a new bill
     *
     * @return Array Default values
     */
    function initial_values()
    {
      // Calculate the date over six weeks
      $sixweeks = mktime(12,0,0,date("m"), date("d")+42, date("Y"));

      // Assamble the array containing default values and return it
      return array("creation_date"=>array("year"=>date("Y"),
                                          "month"=>date("m"),
                                          "day"=>date("d")),
                   "expire_date"=>array("year"=>date("Y", $sixweeks),
                                        "month"=>date("m", $sixweeks),
                                        "day"=>date("d", $sixweeks)),
                   "owner"=>getUser(),
                   "status"=>"new",
                   "billtype"=>BILL_NORMAL,
                  );
    }

    /**
     * Assigns a bill record and any important relations to a documentwriter
     *
     * @param atkDocumentWrter $documentWriter Document writer to which the record should be assigned
     * @param string $selector SQL Where clause to be used when selecting the bill from the database
     */
    function assignDocumentVars(&$documentWriter, $selector)
    {
      // Register the bill info into the documentwriter
      $bills = $this->selectDb($selector);
      $documentWriter->assignDocumentSingleRecord($this, $bills[0]);

      // Dirty but quick way to get the projectid, personid and coordinatorid
      $query = "SELECT `billing_bill`.`project_id` AS projectid,
                       `billing_bill`.`contactperson` AS personid,
                       `project`.`coordinator` AS coordinatorid
                FROM `billing_bill`
                LEFT JOIN `project` ON `project`.`id` = `billing_bill`.`project_id`
                WHERE $selector";
      $db = &atkGetDb();
      $relations = $db->getRows($query);

      // Assign project
      $projectnode = &getNode("project.project");
      $projectrecord = $projectnode->selectDb("project.id='".$relations[0]["projectid"]."'", "", "", "", "", "view");
      $documentWriter->assignDocumentSingleRecord($projectnode, $projectrecord[0], "project_");

      // Assign project coordinator
      $employeenode = &getNode("employee.employee");
      $coordinatorrecord = $employeenode->selectDb("person.id='".$relations[0]["coordinatorid"]."'", "", "", "", "", "view");
      $documentWriter->assignDocumentSingleRecord($employeenode, $coordinatorrecord[0], "coordinator_");

      // Assign contactperson
      $contactnode = &getNode("project.select_contact");
      $contactrecord = $contactnode->selectDb("`person`.`id`='".$relations[0]["personid"]."'", "", "", "", "", "view");
      $documentWriter->assignDocumentSingleRecord($contactnode, $contactrecord[0], "person_");

      // Assign the referenced personcontact's organization (if available)
      $organizationnode = &getNode("organization.organization");
      $organizationrecord = $organizationnode->selectDb("organization.id='".$contactrecord[0]["company"]["id"]."'", "", "", "", "", "view");
      if (count($organizationrecord) > 0)
      {
        $documentWriter->assignDocumentSingleRecord($organizationnode, $organizationrecord[0], "organization_");
      }

      // Use an evil global to let the tbsooo engine use the precence of an organization
      global $documentvar_organizationpresent;
      $documentvar_organizationpresent = (count($organizationrecord) > 0) ? 1 : 0;

      // Register the bill lines into the documentwriter
      $billlinenode = &getNode("billing.bill_line");
      $billlines = $billlinenode->selectDb("`billing_bill_line`.`bill_id`='".$bills[0]["id"]."'", "", "", "", "", "view");
      $documentWriter->assignDocumentMultiRecord($billlinenode, $billlines);

      // Get the bill line id's into an array
      $billlineids = array();
      foreach ($billlines as $billline)
        $billlineids[] = $billline["id"];

      // Get all hours
      $hoursnode = &atkGetNode("timereg.hours");
      if (count($billlineids) > 0)
      {
        $hours = $hoursnode->selectDb(sprintf("hours.billing_bill_line_id IN (%s) AND hours.billing_billed=1", implode(",", $billlineids)), "", "", "", "", "view");
      }
      else
      {
        $hours = array();
      }
      $documentWriter->assignDocumentMultiRecord($hoursnode, $hours);

      // Calculate the total amount of the bill
      $total = 0;
      $totaltax = 0;
      foreach ($billlines as $billline)
      {
        $total += $billline["fixed_amount"];
        $totaltax += $billlinenode->calculate_tax($billline);
      }
      if (atkconfig("tax_included", DEFAULT_TAX_INCLUDED))
      {
        $totalexcltax = $total - $totaltax;
        $totalincltax = $total;
      }
      else
      {
        $totalexcltax = $total;
        $totalincltax = $total + $totaltax;
      }

      // Assign the total amount (both euros & cents seperated and written as one value)
      $curattr = new atkCurrencyAttribute("dummy");
      $documentWriter->assign("bill_total", $curattr->display(array("dummy"=>$totalincltax)));
      $documentWriter->assign("bill_totalexcltax", $curattr->display(array("dummy"=>$totalexcltax)));
      $documentWriter->assign("bill_totaltax", $curattr->display(array("dummy"=>$totaltax)));
      $documentWriter->assign("bill_totalincltax", $curattr->display(array("dummy"=>$totalincltax)));
      list($euro,$cent) = explode(".", str_replace(",", ".", sprintf("%0.2f", $totalincltax)));
      $documentWriter->assign("bill_total_euro", sprintf("%d", $euro));
      $documentWriter->assign("bill_total_cent", sprintf("%02d", $cent));

      // Assign some generic vars to the documentwriter
      $documentWriter->assignDocumentGenericVars();
    }

    function isBillNew($billid)
    {
      $records = $this->selectDb("`billing_bill`.`id`='$billid'", "", "", "", array("status"));
      return ($records[0]["status"] == "new");
    }

    function getBillNr($billid)
    {
      $records = $this->selectDb("`billing_bill`.`id`='$billid'", "", "", "", array("bill_nr"));
      return $records[0]["bill_nr"];
    }

    function generateBill($id, $store = false, $reminder_id = -1, $output_filename = "", $billtemplateoverride = "", $add_to_docmanager_master="", $add_to_docmanager_master_id=0)
    {
      // Get the billtype, bill_nr and status in order to determine the source and destination document
      $bills = $this->selectDb("`billing_bill`.`id`='$id'", "", "", "", array("billtype","bill_nr","status"));

      // Get document template name

      if ($billtemplateoverride != "")
      {
        $billtemplateconfig = $billtemplateoverride;
      }
      elseif ($reminder_id > -1)
      {
        // reminder templates
        $billtemplateconfig = "reminder";
      }
      else
      {
        // bill templates
        switch($bills[0]["billtype"])
        {
          case 0: $billtemplateconfig = "bill"; break;
          case 1: $billtemplateconfig = "bill_advance"; break;
          case 2: $billtemplateconfig = "bill_credit"; break;
        }
      }

      // Determine the result filename
      if (!empty($bills[0]["bill_nr"]))
        $resultname = $bills[0]["bill_nr"] . ".sxw";
      else
        $resultname = $billtemplateconfig . ".sxw";

      if ($output_filename != "")
        $resultname = $output_filename;

      // Try to get it using the config (defaults to bill.sxw, bill_advance.sxw or bill_credit.sxw in the templates subfolder of the billing module)
      $billtemplate = atkconfig($billtemplateconfig . "_template", $billtemplateconfig . ".sxw");
      $billtemplatedir = atkconfig("bill_templatedir", moduleDir("billing") . "templates/");

      // Throw an error if the document does not exist
      if (!file_exists($billtemplatedir . $billtemplate))
      {
        atkerror("bill::action_document: The given document template ($billtemplatedir$billtemplate) does not exist.");
        return;
      }

      // Get the opendocument writer
      atkimport("atk.document.atkdocumentwriter");
      $opendocumentwriter = &atkDocumentWriter::getInstance("opendocument");

      // Assign the selected bill record to the document writer
      $this->assignDocumentVars($opendocumentwriter, "`billing_bill`.`id`='$id'");

      if ($reminder_id > -1)
        $opendocumentwriter->assign('reminder_id',$reminder_id);

      if ($store)
      {
        // Store the document

        if ($add_to_docmanager_master == '')
          $resultfolder = atkConfig::get("billing", "bill_billdir", atkconfig("bill_billdir", moduleDir("billing") . "bills/"));
        else
          $resultfolder = atkconfig("documentpath", "documents/"); // get folder for docmanager

        $opendocumentwriter->store($billtemplatedir . $billtemplate, $resultfolder . $resultname);

        if ($add_to_docmanager_master != "")
        {
          $user = &getUser();

          $add_record = array(
            "name" => substr($resultname,0,strrpos($resultname,".")),
            "filename" => array("filename"=>$resultname),
            "owner" => $user['userid'],
            "master" => $add_to_docmanager_master,
            "master_id" => $add_to_docmanager_master_id,
            "entrydate" => date("Y-m-d"),
            "version" => 1,
            "status" => "accepted",
            "confidential" => false,
            "internal" => false
          );

          $document_node = &getNode("docmanager.document");
          $document_node->addDb($add_record);
        }
      }
      else
      {
        // Display the document
        $opendocumentwriter->display($billtemplatedir . $billtemplate, $resultname);
      }
    }

    /**
     * Custom document action handler
     *
     * Overrides the default document action handler for this node becaus the
     * document won't be retrieved from it's default location, but from the
     * billing module's template subfolder.
     *
     * @param atkDocumentHandler $handler Default document action handler
     */
    function action_document(&$handler)
    {
      // Check if the id is available
      if (!isset($this->m_postvars["id"]))
      {
        atkerror("bill::action_document: id was not set in postvars.");
        return;
      }

      // Store the bill id
      $id = $this->m_postvars["id"];

      // If it is a new bill, then generate it
      if ($this->isBillNew($id))
      {
        // Generate it and serve
        $this->generateBill($id, false);

        // Halt script execution as we don't want atkOutput to resend headers
        die();
      }

      // If it is not a new bill, then load it from cache
      else
      {

        // Determine the name of the cached version
        $resultfolder = atkConfig::get("billing", "bill_billdir", atkconfig("bill_billdir", moduleDir("billing") . "bills/"));
        $resultname = $this->getBillNr($id) . ".sxw";

        // Redirect to the cached version
        $this->redirect($resultfolder . $resultname);

      }
    }

    /**
     * Admin action handler extension
     *
     * Customizes the admin action in order to support two views: open and archive
     *
     * @param atkAdminHandler $handler Default handler for the admin action
     * @return String Default admin action output
     */
    function action_admin(&$handler)
    {
      // Use the session manager to get the current view
      global $g_sessionManager;
      $this->m_view = $g_sessionManager->pageVar("view"); // Can be 'open' or 'archive'.

      // If the view is not archive (and for example unknown), it must be open (which is the default)
      if ($this->m_view != "archive")
        $this->m_view = "open";

      // If in open view, show all new/final/sent bills
      if ($this->m_view == "open")
      {
        $this->addFilter("billing_bill.status IN ('new', 'final', 'sent')");
      }

      // If in archive view, show only cancelled and paid bills and disable the add new bill feature
      else
      {
        $this->addFlag(NF_NO_ADD);
        $this->addFilter("billing_bill.status IN ('cancelled', 'paid')");
      }

      // Use the default admin action handler
      return $handler->action_admin();
    }

    /**
     * Custom edit function for the contactperson relation
     *
     * Limits the choosable contactperson options to those attached to the chosen project
     *
     * @param Array $record Bill record in which the projectid is to be found
     * @param String $fieldprefix  Prefix for the fieldname
     * @return Piece of html code that can be used in a form to edit this
     */
    function contactperson_edit($record, $prefix="", $mode="")
    {
      // Get a reference to the contactperson relation
      $contactrelation = &$this->getAttribute("contactperson");

      // Add a filter to the contactperson relation
      $db = &atkGetDb();
      $personidrecs = $db->getrows("SELECT personid FROM project_person WHERE projectid='".$record["project_id"]["id"]."'");
      $personids = array();
      foreach($personidrecs as $personidrec)
      {
        $personids[] = $personidrec["personid"];
      }
      $contactrelation->setDestinationFilter("`person`.`id` IN ('".implode("','", $personids)."')");
      $result = $contactrelation->edit($record, $prefix, $mode);

      // Then use the default edit function to render the html for the relation
      return $result;
    }

    function adminPage(&$handler)
    {
      $attr = &$this->getAttribute("contactperson");
      $attr->addFlag(AF_LARGE);
      return $handler->adminPage();
    }

    /**
     * Adddb extension function
     *
     * Determines the bill_nr at the moment of adding a bill
     *
     * @param array $record The record to save.
     * @param boolean $exectrigger Indicates whether the postAdd trigger
     *                             should be fired.
     * @param array $excludelist List of attributenames that should be ignored
     *                           and not stored in the database.
     * @return boolean True if succesful, false if not.
     */
    function addDb(&$record, $exectrigger=true, $mode="add")
    {
      // Set a default description if available through config
      if ($record["description"] == "")
      {
        $billtypestrings = array(BILL_NORMAL=>"normal", BILL_ADVANCE=>"advance", BILL_CREDIT=>"credit");
        $record["description"] = $description = atkconfig("billing_initialdescription_".$billtypestrings[$record["billtype"]]."bill");
      }

      // Call the parent's default addDb method and return it's result (succesfull or not)
      return parent::addDb($record, $exectrigger, $mode);
    }

    /**
     * Display a "archive" header above the recordlist
     *
     * @return string HTML Header
     */
    function adminHeader()
    {
      // If in archive mode, show it in the admin header
      if ($this->m_view=="archive")
      {
        return "<br><b>".atktext("archive", "billing")."</b>";
      }

      // Otherwise leave an empty admin header
      else
      {
        return "";
      }
    }

    /**
     * Display a footer containing a link to the other (open or archive) view
     *
     * @return string HTML Footer
     */
    function adminFooter()
    {
      if ($this->m_view == "archive")
      {
        return href(dispatch_url("billing.bill", $this->m_action, array("view"=>"open")), atktext("showopenbills", "billing", "bill"))."<br>";
      }
      else
      {
        return href(dispatch_url("billing.bill", $this->m_action, array("view"=>"archive")), atktext("showarchive", "billing", "bill"))."<br>";
      }
    }

    /**
     * Determines how this record should be displayed in relations
     *
     * @return string Descriptor template
     */
    function descriptor_def()
    {
      return "[bill_nr]";
    }

    /**
     * Checks if user has appropriate rights and protects editing of non-new bills
     *
     * @param String $action Action which is about to be executed
     * @param Array $record The record on which the action is about to be performed
     * @return Boolean True if allowed, False if not
     */
    function allowed($action, $record="")
    {
      // Return false (disallow) if the status is not new and an edit is attempted
      if (isset($record["status"])&&$record["status"]!="new"&&$action=="edit")
        return false;

      // Otherwise use the default allowed function
      return parent::allowed($action, $record);
    }

    /**
     * Stores the project's fixed price in the session and shows the fixed price in a warning if set
     *
     * @param array $rec Bill record for which the fixed attribute should be drawn and the fixed price should be evaluated
     * @return string HTML output
     */
    function fixed_price_edit($rec)
    {
      // Query the database for the fixed price of the referenced project
      $db = &$this->getDb();
      $projectfixedpriceresult = $db->getrows("SELECT project.fixed_price FROM project where project.id = '{$rec["project_id"]["id"]}'");

      // If a fixed price is set for the referenced project:
      if ($projectfixedpriceresult["0"]["fixed_price"] > 0)
      {
        // Store the project's fixed price in the session
        sessionStore("fixed_price", $projectfixedpriceresult["0"]["fixed_price"]);

        // Return a warning about the fixed price
        return sprintf("<B>%s %s!</B>", atktext("warning_fixedprice1", "billing"), number_format($projectfixedpriceresult["0"]["fixed_price"], 2,",","."));
      }
      else
      {
        // Store zero as fixed price in the session
        sessionStore("fixed_price", "0");

        // Return an empty string (nothing to display)
        return "";
      }
    }

    /**
     * Function that is called for each record in a recordlist, to determine
     * what actions may be performed on the record.
     *
     * @param array $record The record for which the actions need to be determined.
     * @param array &$actions Associative array of actionname=>url pairs
     * @param array &$mraactions Associative array of multirecordactionname=>url pairs
     */
    function recordActions($record, &$actions)
    {
      // Only 'closed' (cancelled or paid) bills may be deleted.
      if ($record["status"]!="cancelled"&&$record["status"]!="paid")
        unset ($actions["delete"]);

      // Only 'new' bills may be edited.
      if ($record["status"]!="new")
        unset($actions["edit"]);

      // Determine to which states this bill can be set, depending on it's current state
      $defaultbillworkflow = array(
        "new" => array("final", "cancelled"),
        "final" => array("sent", "cancelled"),
        "sent" => array("paid", "cancelled"),
        "cancelled" => array(),
        "paid" => array(),
      );
      $states = atkArrayNvl(array_merge($defaultbillworkflow, atkConfig("billing_billworkflow", array())), $record["status"], array());

      // Add set_state actions for each state to which this bill can be set
      for($i=0;$i<count($states);$i++)
      {
        $actions["set_".$states[$i]] = dispatch_url("billing.bill", "setstate", array("status"=>$states[$i], "id"=>"[id]"));
      }

      // Add a document action so the bill can be viewed as opendocument
      $actions["document"] = dispatch_url("billing.bill", "document", array("id"=>$record["id"]));
    }

    /**
     * Sets the state of a bill according to the postvars
     *
     * @param atkActionHandler $handler Default aciton handler
     */
    function action_setstate(&$handler)
    {
      // Get the bill id and new status
      $bill_id = atkArrayNvl($this->m_postvars, "id");
      $newstatus = atkArrayNvl($this->m_postvars, "status");

      // Throw an error if id or newstatus not set
      if ((empty($bill_id)) || (empty($newstatus)))
      {
        halt("bill->action_setstate: Id or Newstatus not set", "critical");
      }

      // Determine the bill no and the old status
      $bill_no = $this->getBillNr($bill_id);
      $oldstatus = $this->getStatus($bill_id);
      $db = &$this->getDb();

      $this->preStatusChange($bill_id, $oldstatus, $newstatus);

      // If setting the status to cancelled, unlink all resources that are allocated to the bill lines
      if ($newstatus == "cancelled")
      {
        $this->unlinkRelatedItems($bill_id);
      }


      // Determine the billnr if the status is set from new to anything else than cancelled
      if ($oldstatus == "new" && empty($bill_no) && $newstatus != "cancelled")
      {
        $new_bill_no = $this->getNewBillNumber($bill_id);
        $db->query("UPDATE billing_bill
                    SET bill_nr = '$new_bill_no'
                    WHERE id = '$bill_id'");
      }

      // If changing status of document from new to final or cancelled, generate and cache the bill
      if ($oldstatus == "new")
      {
        $this->generateBill($bill_id, true);
      }

      // Update the bill in the database
      $db->query("UPDATE billing_bill
                  SET status = '$newstatus'
                  WHERE id = '$bill_id'");

      $this->postStatusChange($bill_id, $oldstatus, $newstatus);

      // Go one session level back (probably to the admin page)
      $this->redirect();
    }


    /**
     * Allows change of flow before status is changed.
     *
     * @param   int     $bill_id    db id of the bill
     * @param   string  $oldstatus  status of the bill before change
     * @param   string  $newstatus  status of the bill after change
     * @return  void
     * @access  public
     */
    function preStatusChange(&$bill_id, &$oldstatus, &$newstatus)
    {
    }

    /**
     * Allows change of flow after status is changed.
     *
     * @param   int     $bill_id    db id of the bill
     * @param   string  $oldstatus  status of the bill before change
     * @param   string  $newstatus  status of the bill after change
     * @access  public
     * @return  void
     */
    function postStatusChange(&$bill_id, &$oldstatus, &$newstatus)
    {
    }

    /**
     * Get status of specified bill
     *
     * @param   mixed   $bill_id db id of the bill
     * @return  string  status
     * @access  public
    */
    function getStatus($bill_id)
    {
      $db = &$this->getDb();
      $status = $db->getRows("SELECT `billing_bill`.`status`
                              FROM `billing_bill`
                              WHERE `billing_bill`.`id`='{$bill_id}'");

      return $status[0]['status'];
    }


    /**
     * unlinkRelatedItems
     *
     * @param   int   $bill_id  db id of the bill
     * @return  void
     * @access  public
    */
    function unlinkRelatedItems($bill_id)
    {
      $billlinenode = &atkGetNode("billing.bill_line");
      $billlines = $billlinenode->selectDb("billing_bill_line.bill_id='{$bill_id}'");
      foreach ($billlines as $billline)
      {
        $billlinenode->unlinkRelatedItems($billline);
      }
    }


    /**
     * Get/generate new bill number
     *
     * @param   id  $bill_id  db id of the bill we wish to generate
     * @return  mixed         new bill number
     * @access  public
     */
    function getNewBillNumber($bill_id)
    {
      // Try to let other modules determine the billnumber
      $records = $this->selectDb("billing_bill.id='$bill_id'", "", "", "", "", "view");
      $record = $records[0];
      $externalmoduleresults = atkHarvestModules("getBillNumber", $record);

      // If at least one external module determined a bill number, then
      // use the first one
      if (count($externalmoduleresults) > 0)
      {
        $new_bill_no = $externalmoduleresults[0];
      }

      // Otherwise we will determine the bill number ourselves
      else
      {
        // Get a reference to the database
        $db = &$this->getDb();

        // Just before we add the bill to the database, we generate a unique number.
        $prefix = date("Ymd");

        $configured_prefix_template = atkConfig::get('billing','billnumber_template');
        if ($configured_prefix_template!="")
        {
          $vars = array(
            "year"=>date("Y"),
            "month"=>date("m"),
            "day"=>date("d")
          );

          $fin_year = atkConfig::get('billing','financial_year');
          if (!$fin_year || $fin_year == 0)
            $vars['fin_year'] = date("Y");
          else
            $vars['fin_year'] = $fin_year;

          $prefix = $configured_prefix_template;
          foreach($vars as $k=>$v)
            $prefix = str_replace("[$k]",$v,$prefix);
        }

        // Get the previous bill number index
        $oldbill_nr_records = $db->getrows("SELECT max(substring(bill_nr FROM ".(strlen($prefix)+1).")+0) as bill_nr FROM billing_bill WHERE bill_nr LIKE '$prefix%'");
        $oldbill_nr_index = (int) $oldbill_nr_records[0]["bill_nr"];

        // Set the bill_nr to the newly generated number
        $new_bill_no = $prefix . sprintf("%02d", $oldbill_nr_index + 1);
      }

      return $new_bill_no;
    }




  }

?>