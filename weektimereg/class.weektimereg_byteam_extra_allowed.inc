<?php

  /**
   * Pure used for install; no functionality whatsoever
   */

  useattrib("atknumberattribute");

  class weektimereg_byteam_extra_allowed extends atkNode
  {
    function weektimereg_byteam_extra_allowed()
    {
      $this->atkNode("weektimereg_byteam_extra_allowed");
      $this->add(new atkAttribute("id",AF_AUTOKEY));
      $this->add(new atkNumberAttribute("userid"));
      $this->add(new atkNumberAttribute("projectid"));
      $this->add(new atkNumberAttribute("phaseid"));
      $this->add(new atkNumberAttribute("activityid"));
      $this->add(new atkNumberAttribute("week",0,6,0));
      $this->setTable("weektimereg_byteam_extra_allowed");
    }
  }

?>