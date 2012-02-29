<?php

  $modifiers[] = "project.project";

  class mod_projectsoftware extends achievoModule
  {

    function getNodes()
    {
      registerNode("projectsoftware.software", array("admin","add","edit","delete"));
      registerNode("projectsoftware.installation", array("admin","add","edit","delete"));
    }

    function getMenuItems()
    {
      $this->menuitem("-", "", "projectmanagement");
      $this->menuitem("installations", dispatch_url("projectsoftware.installation", "admin"), "projectmanagement", array("projectsoftware.installation", "admin"));
      $this->menuitem("software", dispatch_url("projectsoftware.software", "admin"), "setup", array("projectsoftware.software", "admin"));
    }

    function getDescription()
    {
      return "Administration of software used in projects.";
    }

    function project_modifier(&$node)
    {
      userelation("atkonetomanyrelation");
      $node->add(new atkOneToManyRelation("software","projectsoftware.installation","project_id",AF_HIDE_LIST),"software");
    }
  }

?>
