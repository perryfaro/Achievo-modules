<?php

  class mod_moduleinfo extends achievoModule 
  {
    function getMenuItems()
    {
      $this->menuitem("moduleadmin", dispatch_url("moduleinfo.module", "admin"), "setup", array("moduleinfo.module", "admin"));
    }
    
    function getNodes()
    {
      registerNode("moduleinfo.module", array("admin"));
    }
    
    function getDescription()
    {
      return "Administration of Achievo extension modules.";
    }
  }

?>
