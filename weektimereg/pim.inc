<?php

  function pimWeekTimeReg()
  {
    $items = array();

    $weekhoursnode = &atkGetNode("weektimereg.weekhours");
    $result = $weekhoursnode->getweekview(time(), false, true);
    $result .= href(dispatch_url("weektimereg.weekhours", "weekregistration"), atktext('pim_gototimeregistration'), SESSION_NEW);

    return $result;
  }

?>