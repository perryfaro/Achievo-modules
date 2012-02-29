<?php
  class advanced_projectFilter extends atkAttribute
  {
    function advanced_projectFilter()
    {
      $this->atkAttribute("projectfilter", AF_HIDE|AF_FORCE_LOAD);
    }

    function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level, $mode)
    {
      if ($mode != 'select') return;

      global $g_securityManager;
      if (!$g_securityManager->allowed("timereg.hours", "any_project") || moduleExists("advancedsecurity"))
      {
        $sessionmanager = &atkGetSessionManager();
        $this->m_userid = $sessionmanager->stackVar("viewuser");
        if($this->m_userid!="")
        {
          $userid = $this->m_userid;
        }
        else
        {
          $userid = atkGetUserId();
        }

        $query->addJoin("project_person", "members", "members.projectid = $tablename.id", true);
        $query->addGroupBy("project.id");
        $query->addCondition("(members.personid = ".$userid." OR $tablename.timereg_limit=".PRJ_TIMEREG_ALL_USERS.")");
      }
      $query->addCondition("$tablename.status='active'");
    }
  }
?>