<?php

  $modifiers[] = "organization.organization";
  $modifiers[] = "project.project";

  class mod_quotation extends achievoModule
  {

    function getNodes()
    {
      registerNode("quotation.quotation",array("admin","add","edit","delete"));
    }

    function getMenuItems()
    {
      $this->menuitem("quotation", dispatch_url("quotation.quotation", "admin"), "main",1,685);
    }

    function getDescription()
    {
      return "Administration of quotations for customers.";
    }

    function organization_modifier(&$node)
    {
      userelation("atkonetomanyrelation");
      $node->add(new atkOneToManyRelation("quotations","quotation.quotation","organization_id",AF_HIDE_LIST));
    }

    function project_project_modifier(&$node)
    {
      useattrib("atkAttribute");
      $node->add(new atkAttribute("quotation_number", AF_HIDE_LIST));
    }
  }

?>
