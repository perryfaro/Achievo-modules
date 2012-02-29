<?php

  class mod_profilesheet extends achievoModule
  {
    function getNodes()
    {
      /* register nodes */
      registerNode("profilesheet.certification", array("admin", "add", "edit", "delete"));
      registerNode("profilesheet.experience_category", array("admin", "add", "edit", "delete"));
      registerNode("profilesheet.experience", array("admin", "add", "edit", "delete"));
      registerNode("profilesheet.profilesheet_experience", array("admin", "add", "edit", "delete"));
      registerNode("profilesheet.profilesheet_certification", array("admin", "add", "edit", "delete"));
      registerNode("profilesheet.profilesheet", array("admin", "add", "edit", "delete","all_persons"));
    }

    function getMenuItems()
    {
      $this->menuitem("certification_admin", dispatch_url("profilesheet.certification", "admin"), "setup", array("profilesheet.certification", "admin"));
      $this->menuitem("experience_category_admin", dispatch_url("profilesheet.experience_category", "admin"), "setup", array("profilesheet.experience_category", "admin"));
      $this->menuitem("experience_admin", dispatch_url("profilesheet.experience", "admin"), "setup", array("profilesheet.experience", "admin"));
      $this->menuitem("profilesheet_admin", dispatch_url("profilesheet.profilesheet", "admin"),"main", array("profilesheet.profilesheet", "admin"),"600");
      
      $node = atkGetNode("profilesheet.profilesheet");
      
      if($node->isAllowed('all_persons'))
      {
        $this->menuitem("profilesheet_admin", dispatch_url("profilesheet.profilesheet", "admin"),"main", array("profilesheet.profilesheet", "admin"),"600");
      } else {
        $this->menuitem("profilesheet_admin", dispatch_url("profilesheet.profilesheet", "edit","employee.id = ". atkGetUserId()),"main", array("profilesheet.profilesheet", "admin"),"600");
      }
      
    }

    function getDependencies($mode)
    {
      return array("employee");
    }
  }

?>