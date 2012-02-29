<?php
  $modifiers[] = "project.project";
  $modifiers[] = "organization.organization";
  
  class mod_hourdeposit extends achievoModule
  {
    function getMenuItems()
    {
      menuitem("deposits", dispatch_url("hourdeposit.deposit", "admin"), "organizationmanagement", array("hourdeposit.deposit", "admin"));
    }
    
    function getNodes()
    {
      registerNode("hourdeposit.deposit", array("admin", "add", "edit", "delete", "status")); 
    }
    
    function project_modifier(&$node)
    {
      $node->add(new atkManyToOneRelation("hourdeposit_deposit_id", "hourdeposit.deposit", AF_HIDE_LIST|AF_HIDE_ADD));
    }
    
    function organization_modifier(&$node)
    {
      $node->add(new atkOneToManyRelation("deposits", "hourdeposit.deposit", "customer", AF_HIDE_LIST));
    }
    
    function getDescription()
    {
      return "Customer deposits of hours";
    }
    
  }
?>
