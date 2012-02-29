<?php
  /**
   * Docmanager module file
   *
   * @package achievo_modules
   * @subpackage docmanager
   *
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2005 Ibuildings.nl BV
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision: 3014 $
   * $Id: module.inc 3014 2006-12-05 15:53:48Z guido $
   */

  // Add the docmanager to any modules specified in the $config_docmanagernodes array
  $modifiers = array_merge($modifiers, atkconfig("docmanagernodes", array()));
  $modifiers[] = "project.tpl_project";
  $modifiers[] = "project.project_personcontact";

  /**
   * docmanager module class
   *
   * @author guido <guido@ibuildings.nl>
   * @package achievo_modules
   * @subpackage docmanager
   */
  class mod_docmanager extends achievoModule
  {

    /**
     * Sets the nodes contained within this module and the actions a user could perform on them
     */
    function getNodes()
    {
      registerNode("docmanager.document", array("add","edit","delete","generate"));
      registerNode("docmanager.documenttype", array("admin","add","edit","delete"));
    }

    /**
     * Adds menu items to the menu when loading the menu
     */
    function getMenuItems()
    {
      $this->menuitem("document_types", dispatch_url("docmanager.documenttype", "admin"), "setup", array("docmanager.documenttype", "admin"));
    }

    /**
     * Modifier on project.project node
     *
     * @param atkNode &$node Node
     * @todo Replace with generic modifier
     */
    function project_project_modifier(&$node)
    {
      useattrib("docmanager.projectdocumentmanager");
      $node->add(new projectDocumentManager("documents", AF_HIDE_LIST|AF_HIDE_SEARCH), "documents");
    }

    /**
     * Modifier on project.project node
     *
     * @param atkNode &$node Node
     */
    function project_tpl_project_modifier(&$node)
    {
      userelation("atkmanyboolrelation");
      $rel = &$node->add(new atkManyBoolRelation("documenttypes", "docmanager.projecttpl_doctype", "docmanager.documenttype", AF_HIDE_LIST|AF_HIDE_ADD));
      $rel->setLocalKey("projecttpl_id");
      $rel->setRemoteKey("documenttype_id");
      $rel->setDestinationFilter("master='project.project'");
    }

    function project_project_personcontact_modifier(&$node)
    {
      useattrib("docmanager.relateddocument");
      $node->add(new relatedDocument("document", "projectid"));
    }

    /**
     * Generic modifier for nodes that want to include the documentManager attribute
     *
     * @param atkNode &$node Node
     */
    function addDocumentManager(&$node)
    {
      useattrib("docmanager.documentmanager");
      $node->add(new documentManager("documents", AF_HIDE_LIST|AF_HIDE_SEARCH), "documents");
    }

    function getModifierFunctions(&$node)
    {
      $modifierfunctions = parent::getModifierFunctions($node);
      $docmanagernodes = atkconfig("docmanagernodes", array());
      if (in_array($node->atknodetype(), $docmanagernodes) && ($node->atknodetype() != "project.project"))
        $modifierfunctions[] = "addDocumentManager";
      return $modifierfunctions;
    }
  }
?>