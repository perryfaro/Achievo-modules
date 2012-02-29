<?php
	atkimport("atk.atklanguage");
	$al = &atkLanguage::getInstance();
	$al->addOverrideModule("carlease");

  class mod_carlease extends atkModule 
  {
   function getNodes()
    {
      registerNode("carlease.carlease", array("admin", "add", "edit", "delete"));
      registerNode("carlease.carlease_maintenance", array("admin", "add", "edit", "delete"));
      registerNode("carlease.carlease_employee", array("admin", "add", "edit", "delete"));
      registerNode("carlease.carlease_damage", array("admin", "add", "edit", "delete"));
      registerNode("carlease.carlease_owner", array("admin", "add", "edit", "delete"));
      registerNode("carlease.carlease_insurance", array("admin", "add", "edit", "delete"));
    }
    
    function getMenuItems()
    {
      $this->menuitem("carlease", dispatch_url("carlease.carlease", "admin"),"main",array("carlease.carlease","admin"));
    }
  }
  
?>