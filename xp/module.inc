<?php
  /**
   * mod_xp class file
   *
   * @package achievo_modules
   * @subpackage xp
   *
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2005 Ibuildings.nl BV
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision: 3867 $
   * $Id: module.inc 3867 2006-01-24 15:55:22Z guido $
   */

  // Register the modifier on the project node
  $modifiers[] = "project.project";

  /**
   * Module class defining the menuitems, nodes and modifiers for the xp module
   *
   * @author guido <guido@ibuildings.nl>
   * @package achievo_modules
   * @subpackage xp
   */
  class mod_xp extends achievoModule
  {

    /**
     * Adds the menuitems used to access the story node to the menu
     */
    function getMenuItems()
    {
      $this->menuitem("xp", "", "main", array("xp.story", "admin"));
      $this->menuitem("stories", dispatch_url("xp.story", "admin"), "xp", array("xp.story", "admin"));
    }

    /**
     * Registers the story node as being a part of this module
     */
    function getNodes()
    {
      registerNode("xp.story", array("admin", "add", "edit", "document"));
    }

    /**
     * This function implements the modifier on the project node
     *
     * @param atkNode &$node Project node to be modified
     */
    function project_project_modifier(&$node)
    {
      userelation("atkonetomanyrelation");
      $node->add(new atkOneToManyRelation("user_stories", "xp.story", "project", AF_HIDE_LIST), "user_stories");
    }
  }

?>