<?php

  /** 
   * Project extension with a custom action to remove the project from a deposit.
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @version $Revision: 3107 $
   * 
   * $Id: class.deposit_project.inc 3107 2003-05-26 21:40:12Z ivo $
   */
   
  include_once(moduleDir("project")."class.project.inc"); // include original baseclass.
  
  useattrib("project.timecountattribute");

  class deposit_project extends project
  {
    function deposit_project()
    {
      $this->project();
      $this->m_flags |= NF_NO_EDIT|NF_NO_ADD;
      $this->m_type = "deposit_project";
      
      // Anyone who may edit deposits, may remove a project from the
      // deposit. 
      $this->m_securityMap["remove"] = "deposit.edit";
      
      // The rest of the actions has equal privileges to 
      // the original node. 
      $this->setSecurityAlias("project.project");
      
      $this->add(new timeCountAttribute(AF_HIDE_VIEW|AF_TOTAL));
    }
    
    /**
     * Remove a project from a deposit. 
     */
    function action_remove()
    {
      global $g_db;
      if ($this->m_postvars["project_id"]!="")
      {
        $g_db->query("UPDATE project SET hourdeposit_deposit_id = NULL WHERE id = '".$this->m_postvars["project_id"]."'");
      }
      $this->redirect();
    }
    
    function recordActions($record, &$actions, &$mraactions)
    {
      $actions = array(); // Remove all default actions. 
      $actions["remove"] = dispatch_url($this->atkNodeType(), "remove", array("project_id"=>$record["id"]));
      $actions["view"] = dispatch_url("project.project", "view", array("atkselector"=>$this->primaryKey($record)));
    }
  }

?>