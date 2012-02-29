<?php

  class mod_projectplan extends achievoModule
  {
    function getNodes()
    {
      registerNode("projectplan.projectplan", array("plan", "edit"));
    }
    
    function getMenuItems()
    {
      $selectedproject = $_REQUEST["selectedproject"];
      if ($selectedproject == 0)
      {        
        $target_planning = "dispatch.php?atknodetype=projectplan.projectplan&atkaction=plan&selectedprojectid=[id]";        
        menuitem("projectplan",  dispatch_url("project.project", "select", array("atktarget" => atkurlencode($target_planning))), "projectmanagement", array("projectplan.projectplan", "plan"));
        menuitem("-","","projectmanagement");        
      }
      else
      {        
        menuitem("projectplan",  dispatch_url("projectplan.projectplan", "plan", array("selectedprojectid" => $selectedproject)), "projectmanagement", array("projectplan.projectplan", "plan"));
        menuitem("-","","projectmanagement");        
      }      
    }
  }

?>
