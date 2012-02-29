<?php

  /**
   * Extension of the project node, with added capabilities of filtering
   * projects. (members only, etc.)
   * TODO: generalize this; this works only for hardcoded rights/filter
   * settings. (in fact this is a copy of the one from the project module)
   */

  include_once(moduleDir("project")."class.project.inc");

  class billing_projectselector extends project
  {
    function billing_projectselector()
    {
      global $g_securityManager;
      $this->project("projectselector", NF_READONLY);
      $this->setSecurityAlias("project.project");

      // Add filter.
      if (!$g_securityManager->allowed("billing.expense", "any_project"))
      {
        $this->add(new billing_projectFilter());
      }
    }

  }

  class billing_projectFilter extends atkAttribute
  {
    function billing_projectFilter()
    {
      $this->atkAttribute("projectfilter", AF_HIDE|AF_FORCE_LOAD);

    }

    function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level, $mode)
    {
      if ($level==1) // not if we're nested inside something elses query
      {
        $user = getUser();
        $query->addJoin("project_person", "members", "members.projectid = project.id", true);
        $query->addCondition("(members.personid = ".$user["id"]." OR project.billing_expensereg_limit=".PRJ_EXPENSEREG_ALL_USERS.")");
      }
    }
  }

?>