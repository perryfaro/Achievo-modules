<?php

  class mod_weektimereg extends achievoModule
  {
    function getMenuItems()
    {
      // Overwrite the time registration submenu to open weekhours admin by default instead of hours admin
      $this->menuitem("time_registration", dispatch_url("weektimereg.weekhours", "weekregistration"), "main", true, 100);

      // Add the weektimereg menuitem in the time_registration menu, eventually overwriting the
      // default hours_admin link if specified by config
      $weekhoursadminname = atkconfig("weektimereg_replacedefaulttimereg", false) ? "hours_admin" : "weekhours_admin";
      $this->menuitem($weekhoursadminname, dispatch_url("weektimereg.weekhours", "weekregistration"),"time_registration",array("timereg.hours", "admin"), 710);
    }

    function getPimItems()
    {
      return array("weekhours"=>"getWeekTimeReg");
    }

    function getWeekTimeReg()
    {
      include_once(moduleDir("weektimereg")."pim.inc");
      return pimWeekTimeReg();
    }

    function getDescription()
    {
      return "Module for easy weekly time registration.";
    }
  }
?>