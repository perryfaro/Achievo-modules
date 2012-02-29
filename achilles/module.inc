<?php
  
  class mod_achilles extends achievoModule 
  {
    function getMenuItems()
    {
      
      // TODO: After Bug ADD, it needs to jump to the admin screen of the chosen project
     

      $this->menuitem("achillesmanagement", "", "main",true,600);
      $selectedproject = $_REQUEST["selectedproject"];
      if ($selectedproject == 0)
      {
        $target_bug = "dispatch.php?atknodetype=achilles.bug&atkaction=admin&selectedprojectid=[id]";
        $this->menuitem("achilles_bug_admin",  dispatch_url("project.project", "select", array("atktarget" => atkurlencode($target_bug))), "achillesmanagement", array("achilles.bug", "admin"));
        $target_bug_add = "dispatch.php?atknodetype=achilles.bug&atkaction=add&selectedprojectid=[id]";
        $this->menuitem("achilles_bug_add",  dispatch_url("project.project", "select", array("atktarget" => atkurlencode($target_bug_add))), "achillesmanagement", array("achilles.bug", "admin"));
        $target_component = "dispatch.php?atknodetype=achilles.component&atkaction=admin&selectedprojectid=[id]";
        $this->menuitem("achilles_component",dispatch_url("project.project", "select", array("atktarget" => atkurlencode($target_component))), "achillesmanagement", array("achilles.component", "admin"));
        $target_version = "dispatch.php?atknodetype=achilles.version&atkaction=admin&selectedprojectid=[id]";
        $this->menuitem("achilles_version", dispatch_url("project.project", "select", array("atktarget" => atkurlencode($target_version))), "achillesmanagement", array("achilles.version", "admin"));
        $target_milestone = "dispatch.php?atknodetype=achilles.milestone&atkaction=admin&selectedprojectid=[id]";
        $this->menuitem("achilles_milestone", dispatch_url("project.project", "select", array("atktarget" => atkurlencode($target_milestone))), "achillesmanagement", array("achilles.milestone", "admin"));
      }
      else
      {
        $this->menuitem("achilles_bug_admin", dispatch_url("achilles.bug", "admin",array("atkselector" => "project.id='".$selectedproject."'")), "achillesmanagement", array("achilles.bug", "admin"));
        $this->menuitem("achilles_bug_add", dispatch_url("achilles.bug", "add",array("atkselector" => "project.id='".$selectedproject."'")), "achillesmanagement", array("achilles.bug", "admin"));
        $this->menuitem("achilles_component", dispatch_url("achilles.component", "admin",array("atkselector" => "project.id='".$selectedproject."'")), "achillesmanagement", array("achilles.component", "admin"));
        $this->menuitem("achilles_version", dispatch_url("achilles.version", "admin",array("atkselector" => "project.id='".$selectedproject."'")), "achillesmanagement", array("achilles.version", "admin"));
        $this->menuitem("achilles_milestone", dispatch_url("achilles.milestone", "admin",array("atkselector" => "project.id='".$selectedproject."'")), "achillesmanagement", array("achilles.milestone", "admin"));
      }
      
       
      $this->menuitem("os_admin", dispatch_url("achilles.os", "admin"), "setup", array("achilles.os", "admin"));
      $this->menuitem("resolution_admin", dispatch_url("achilles.resolution", "admin"), "setup", array("achilles.resolution", "admin"));
      $this->menuitem("severity_admin", dispatch_url("achilles.severity", "admin"), "setup", array("achilles.severity", "admin"));
      $this->menuitem("status_admin", dispatch_url("achilles.status", "admin"), "setup", array("achilles.status", "admin"));
    }
    
    
    function getNodes()
    {
      registerNode("achilles.bug", array("admin","add","edit","delete"));
      registerNode("achilles.component",array("admin","add","delete","edit"));
      registerNode("achilles.version",array("admin","add","delete","edit"));
      registerNode("achilles.milestone",array("admin","add","delete","edit"));
      registerNode("achilles.os",array("admin","add","delete","edit"));
      registerNode("achilles.resolution",array("admin","add","delete","edit"));
      registerNode("achilles.severity",array("admin","add","delete","edit"));
      registerNode("achilles.status",array("admin","add","delete","edit"));
    }
    
    
    function getDependencies($mode)
    {
      return array("employee", "organization","project");
    }
    
    function getDescription()
    {
      return "Achilles Module.";
    }
    
    function search($expression)
    {
      $res = array();
      $custnode = &getNode("achilles.bug");
      $recs = $custnode->searchDb($expression);
            
      $res["achilles"] = $this->recLinks($recs, "achilles.bug");

      return $res;
    }
    
    
    function getPimItems()
    {
      return array("bugs"=>"getBugs");
    }

    function getBugs()
    {
      include_once(moduleDir("achilles")."pim.inc");
      return pimBug();
    }
  }
?>
