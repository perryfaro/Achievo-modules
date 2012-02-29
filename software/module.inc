<?php
  $modifiers[] = "project.project";

  userelation("atkonetomanyrelation");

  class mod_software extends achievoModule   
  {

    function getNodes()
    {
      registerNode("software.software", array("admin","add", "edit", "delete"));      
    }

    function getMenuItems()
    {
      /* add menuitems */
			menuitem("assets","","main");
      menuitem("at_software", dispatch_url("software.software_search","admin","search","view","add", "edit", "delete"), "assets");
    }

    function getDescription()
    {
      return "Software license Tracking";
    }
    
    function project_modifier(&$node)
    {
      userelation("atkonetomanyrelation");
      $node->add(new atkOneToManyRelation("software_assets","software.software", "project_id", AF_HIDE_LIST),"assets");
    }
    
  }   
?>