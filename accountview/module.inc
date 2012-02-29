<?php

  $modifiers[] = "project.project";
  $modifiers[] = "timereg.hours";

  atkimport("atk.atklanguage");
  $al = &atkLanguage::getInstance();
  $al->addOverrideModule("accountview");

  class mod_accountview extends atkModule
  {
    function project_project_modifier(&$node)
    {
      // Make the abbreviation visible in list views and searchable
      $abbreviationattr = &$node->getAttribute("abbreviation");
      $abbreviationattr->addFlag(AF_SEARCHABLE);
      $abbreviationattr->removeFlag(AF_HIDE_LIST);
    }

    function timereg_hours_modifier(&$node)
    {
      // Add the listener for timereg additions/updates
      atkimport("module.accountview.timeregactionlistener");
      $node->addListener(new timeregActionListener(array("save", "update")));
    }
  }

?>