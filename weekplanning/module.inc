<?php
  /**
   * weekplanning class file
   *
   * @package achievo_modules
   * @subpackage weekplannings
   *
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2005 Ibuildings.nl BV
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision: 3668 $
   * $Id: module.inc 3668 2006-05-25 01:58:36Z guido $
   */

  /**
   * @internal modifiers
   */
  $modifiers[] = "timereg.hours";
  $modifiers[] = "project.project";
  $modifiers[] = "project.phase";
  $modifiers[] = "employee.usercontracts";
  if (moduleExists("billing"))
  {
    $modifiers[] = "billing.projecttype";
  }

  atkimport("atk.atklanguage");
  $al = &atkLanguage::getInstance();
  $al->addOverrideModule("weekplanning");

  /**
   * weekplanning module class
   *
   * @author guido <guido@ibuildings.nl>
   * @package achievo_modules
   * @subpackage weekplan
   */
  class mod_weekplanning extends atkModule
  {

    /**
     * Defines the nodes and the available rights you can grant to users on these nodes
     */
    function getNodes()
    {
      registerNode("weekplanning.weekplanning", array("admin", "edit", "any_user", "any_project"));
      registerNode("weekplanning.weekplanningconflictlog", array("admin", "view"));
    }

    /**
     * Gives a description of this module
     *
     * @return String Description of this module
     */
    function getDescription()
    {
      return "Module for making week based schedules";
    }

    /**
     * Defines the menuitems to give access to this module's nodes
     */
    function getMenuItems()
    {

      // Add a weekplanning submenu if the user has either admin access to the weekplanning or weekplanningconflictlog node
      $this->menuitem("weekplanning", "", "main", array("weekplanning.weekplanning", "admin", "weekplanning.weekplanningconflictlog", "admin"));

      // Add links to the 2 weekplanning views if the user has admin access to the weekplanning node
      $this->menuitem("weekplanning_peremployee", dispatch_url("weekplanning.weekplanning_peremployee", "admin"), "weekplanning", array("weekplanning.weekplanning", "admin"));
      $this->menuitem("weekplanning_perproject", dispatch_url("weekplanning.weekplanning_perproject", "admin"), "weekplanning", array("weekplanning.weekplanning", "admin"));

      // Add a link to the weekplanningconflictlog node if the user has admin access on this node
      $this->menuitem("weekplanningconflicts", dispatch_url("weekplanning.weekplanningconflictlog", "admin"), "weekplanning", array("weekplanning.weekplanningconflictlog", "admin"));

      // Add a seperator after the last item, to create a spacing between the menu items and the "Back to the main menu" link
      $this->menuitem("-", "", "weekplanning", array("weekplanning.weekplanning", "admin", "weekplanning.weekplanningconflictlog", "admin"), 100000);

      // Overwrite the mastergantt graph link in the project menu
      $this->menuitem("mastergantt", dispatch_url("weekplanning.weekmastergantt", "planning"), "projectmanagement");
    }

    function billing_projecttype_modifier(&$node)
    {
      $node->add(new atkNumberAttribute("weekplanning_avaragerate", 0, 10, 2));
    }

    function timereg_hours_modifier(&$node)
    {
      $this->addConflictListener($node, array("save", "update"));
    }

    function project_project_modifier(&$node)
    {
      $fixed_price_order = $node->m_attribIndexList[$node->m_attribList["fixed_price"]->m_index]["order"];
      $node->add(new atkNumberAttribute("weekplanning_employee_budget",AF_HIDE_LIST|AF_HIDE_ADD, 13, 5), "finance", $fixed_price_order + 10);
    }

    function project_phase_modifier(&$node)
    {
      $this->addConflictListener($node, array("update"));

      // Add the listener for phaseplanning shiftings when updating a phase
      atkimport("module.weekplanning.shiftphaselistener");
      $node->addListener(new shiftPhaseListener(array("update")));

      // Remove the phaseplanning relation and it's fuzzysearch attribute
      $node->remove("phaseplanning");
      $node->remove("phaseplanning_add");
    }

    function employee_usercontracts_modifier(&$node)
    {
      $this->addConflictListener($node, array("save", "update"));
    }

    function addConflictListener(&$node, $actionfilter)
    {
      atkimport("module.weekplanning.conflictlistener");
      $node->addListener(new conflictListener($actionfilter));
    }

  }
?>