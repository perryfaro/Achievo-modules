<?php
  /**
   * XP User Story class file
   *
   * @package achievo_modules
   * @subpackage xp
   *
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2005 Ibuildings.nl BV
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision: 3873 $
   * $Id: class.story.inc 3873 2007-02-05 22:30:58Z guido $
   */

  // Load the attribute classes to be used
  useattrib("atktextattribute");
  useattrib("atknumberattribute");
  useattrib("atkdateattribute");
  userelation("atkmanytoonerelation");
  useattrib("atkdocumentattribute");
  useattrib("atkdummyattribute");
  useattrib("xp.atkuserstorystatusattribute");

  /**
   * Story node class
   *
   * @author guido <guido@ibuildings.nl>
   * @package achievo_modules
   * @subpackage xp
   */
  class story extends atkNode
  {

    /**
     * Story class constructor
     */
    function story()
    {
      // Call base class constructor
      $this->atkNode("story", NF_NO_SECURITY|NF_ADD_LINK|NF_MRA);
      $this->add(new atkAttribute("id", AF_AUTOKEY));
      $this->add(new atkManyToOneRelation("project", "project.project", AF_SEARCHABLE));
      $this->add(new atkAttribute("title", AF_SEARCHABLE));
      $this->add(new atkManyToOneRelation("submitted_by", "employee.employee", AF_HIDE_LIST));
      $this->add(new atkTextAttribute("story", AF_HIDE_LIST));
      $this->add(new atkNumberAttribute("estimate", AF_TOTAL | AF_SEARCHABLE, 10, 0));
      $this->add(new atkDateAttribute("submitted_on", AF_HIDE_LIST | AF_FORCE_LOAD | AF_READONLY));
      $this->add(new atkDateAttribute("accepted_on", AF_HIDE_LIST | AF_FORCE_LOAD));
      $this->add(new atkDateAttribute("planned_on", AF_HIDE_LIST | AF_FORCE_LOAD));
      $this->add(new atkDateAttribute("implemented_on", AF_HIDE_LIST | AF_FORCE_LOAD));
      $this->add(new atkUserStoryStatusAttribute("status", AF_HIDE_VIEW));
      $this->add(new atkTextAttribute("comments", AF_HIDE_LIST));

      $this->add(new atkDocumentAttribute("documents"));

      $this->setOrder("xp_story.id DESC");
      $this->setIndex("title");
      $this->setTable("xp_story");
    }

    /**
     * Defines initial values to be used when setting up a new user story
     *
     * @return array Array containing key=>defaultvalue pairs for several attributes of the story node
     */
    function initial_values()
    {
      global $g_user;
      return array(
        "submitted_by" => array("id" => $g_user["id"]),
        "estimate" => 0,
        "submitted_on" => array("year"=>date("Y"), "month"=>date("m"), "day"=>date("d"))
      );
    }

    /**
     * Defines the window title template to be shown when editing a user story
     *
     * @return string Template string telling atkNode to display the projectname and user story title in the titlebar of an editscreen
     */
    function descriptor_def()
    {
      return "[project.name], [title]";
    }

    /**
     * RecordActions definitions container function
     *
     * This function contains the definitions of all custom actions to be offered on records by adding actionname=>actionurl pairs to the $actions array
     *
     * @param array $record The record for which the actions need to be determined.
     * @param array &$actions Reference to an array with the already defined actions. This is an associative array with the action identifier as key, and an url as value. Actions can be removed from it, or added to the array.
     * @param array &$mraactions List of multirecordactions that are supported forthe passed record.
     */
    function recordActions($rec, &$actions, &$mraactions)
    {
      $actions["implement"] = session_url("dispatch.php?atknodetype=" . $this->atkNodeType() . "&atkaction=implement&atkselector=[pk]", SESSION_NESTED);
      $actions["print"] = session_url("dispatch.php?atknodetype=" . $this->atkNodeType() . "&atkaction=print&atkselector=[pk]", SESSION_NESTED);

      //Inactive, but functional recordaction definition to call the document action handler without using the documentattribute. Uncomment if you want to use it.
      //$templatefile = "userstory.odt";
      //$actions["document"] = dispatch_url($this->atkNodeType(), "document", array("atkdoctpl"=>$templatefile, "atkselector"=>"[pk]"));
    }

    /**
     * Action handler for the implement action.
     *
     * This action handler implements the implement action (double isnt that
     * :D). When called it updates the selected user story (passed by the
     * atkselector parameter) by setting its implemented_on attribute to the
     * current date/time
     *
     * @param atkActionHandler &$handler The generic actionhandler used to handle this action
     */
    function action_implement(&$handler)
    {
      if ($this->m_postvars["atkselector"] != "")
      {
        $db = &atkGetDb();
        $db->query("UPDATE xp_story SET implemented_on=NOW() WHERE " . $this->m_postvars["atkselector"]);
      }
      $this->redirect();
    }

    /**
     * Formats a given date array to a localized string
     *
     * @param array $value Array containing the month, day and year element refering to its corresponding values
     * @return string Formatted date
     */
    function formatDate($value)
    {
      // Return empty string if invalid date array given
      if (empty($value["month"]) or empty($value["day"]) or empty( $value["year"])) return "";

      // Construct a date object
      $date = adodb_getdate(adodb_mktime(0, 0, 0, $value["month"], $value["day"], $value["year"]));

      // Try to get the local date format and use "F j Y" in case of failure
      $txt_date_format_view = atktext("date_format_view","atk","","","",true);
      $format = (!empty($txt_date_format_view)) ? $txt_date_format_view : "F j Y";

      // Make M (short month) and F (full month name) inrecognisable to the adodb_data format function
      // so later on they can be replaced by localized strings
      $format = str_replace("M", "%-%",   $format);
      $format = str_replace("F", "%=%",   $format);

      // Get the formatted date string
      $str_date = adodb_date($format, $date[0]);

      // Invoke a static array to cache the localized month and shortmonth strings
      static $langcache = array();

      // Determine strings that identify the month and shortmonth localized strings
      $month = $date['month'];
      $shortmonth = substr(strtolower($date["month"]), 0, 3);

      // Check for cached localized month and load it if not present
      if(!isset($langcache[$month]))
      {
        $langcache[$month]= text(strtolower($month),"","atk");
      }

      // Check for cached localized shortmonth and load it if not present
      if(!isset($langcache[$shortmonth]))
      {
        $langcache[$shortmonth] = text($shortmonth,"","atk");
      }

      // Replace the self-made month and shortmonth values by the localized strings
      $str_date = str_replace("%-%", $langcache[$shortmonth], $str_date);
      $str_date = str_replace("%=%", $langcache[$month], $str_date);

      // Return the formatted date string
      return $str_date;
    }

    /**
     * Action handler for the print action
     *
     * This function implementes the print action. It retrieves the data for
     * the selected user story (passed as atkselector parameter) and uses\
     * the atkUI to merge it into a custom template.
     *
     * @param atkActionHandler &$handler The generic actionhandler used to handle this action
     */
    function action_print(&$handler)
    {
      // Retrieve record from database
      $record = $this->selectDb($this->m_postvars["atkselector"]);

      // Insert all values from the first record into the templatevars values array
      $templatevars = array();
      $templatevars["values"] = $record[0];

      // Convert all text attributes to html
      $textattributes = array("story", "comments");
      foreach($textattributes as $textattribute) {
        $templatevars["values"][$textattribute] = nl2br(htmlspecialchars($templatevars["values"][$textattribute]));
      }

      // Convert all date datatypes into tekst
      $dateattributes = array("submitted_on", "accepted_on", "planned_on", "implemented_on");
      foreach($dateattributes as $dateattribute) {
        $templatevars["values"][$dateattribute] = $this->formatDate($templatevars["values"][$dateattribute]);
      }

      // Include printtime in template vars
      $currenttime = time();
      $currentdate = array("month" => date("m"), "day" => date("d"), "year" => date("Y"));
      $templatevars["printtime"] = $this->formatDate($currentdate) . adodb_date(" G:i", $currenttime);

      // Include backlink in template vars (thnx to Boy)
      $templatevars["backlink"] = href(dispatch_url($this->m_module.".".$this->m_type,"admin"), atktext("clickheretogoback", "xp"), SESSION_BACK);

      // Get localized labels
      $languagevars = array("user_story", "title", "submitted_by", "story", "estimate", "submitted_on", "accepted_on", "planned_on", "implemented_on", "released_on", "status", "comments", "not_processed");
      foreach ($languagevars as $languagevar) {
        $templatevars["labels"][$languagevar] = atktext($languagevar, "xp");
      }
      $templatevars["labels"]["project"] = atktext("project", "atk");

      // Call user interface render engine and parse the template using the templatevars
      $ui = &$this->getUI();
      $output = $ui->render(moduleDir("xp") . "templates/print.tpl", $templatevars);

      // Configure the page using the output generated by the template parser and let it use the print-sepecific style-sheet
      $page = &$this->getPage();
      $page->register_style(moduleDir("xp") . "templates/print.css");
      $page->register_scriptcode("
        function printframe() {
          window.parent.main.focus();
          window.print();
        }
      ");
      $page->register_loadscript("printframe();");
      $page->addContent($output);
    }

    /**
     * Determines the current view to be used (active or archive)
     *
     * @return string Current view mode ("active" or "archive")
     */
    function getView()
    {
      // We use the session manager to retrieve the view.
      // this way, when paging, or when returning from an
      // edit screen etc., the system will have remembered
      // if we were viewing active records or the archive.
      global $g_sessionManager;
      $view = $g_sessionManager->pageVar("view");
      if ($view=="") return "active"; // view only active records by default
      return $view;
    }

    /**
     * Admin action handler override to implement archive and active view modes
     *
     * This function overrides the default adminHandlers action_admin, in
     * order to add our filtering logic to the admin action. This is done to
     * implement the archive and active view modes
     *
     * @param atkActionHandler &$handler Default admin actionhandler
     * @return atkAdminHandler Default admin action handler
     */
    function action_admin(&$handler)
    {
      // Add the filtering logic to the admin screen
      $this->addFilter("implemented_on is " . (($this->getView() == "active") ? "null" : "not null"));

      // Call the default adminHandler action_admin so the default action will be undertaken.
      return $handler->action_admin();
    }

    /**
     * Add html interface for switching to archive and active in user interface footer
     *
     * In this function we provide the user with a way to switch the view by
     * adding a link to the view the user is not in.
     *
     * @return string HTML Code fore the link to the other view
     */
    function adminFooter()
    {    	
      if ($this->getView()=="active")
      {
        $link = "dispatch.php?view=archive";
        return "Only active records are displayed. Click " . href($link, "here") . " to view the archive";
      }
      else
      {
        $link = "dispatch.php?view=active";
        return "You ar viewing the archive. Click " . href($link, "here") . " to go back to normal view";
      }
    }
  }
?>