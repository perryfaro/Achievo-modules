<?php

  $setup->dropTable("holidayreport_leave_type");
  $setup->dropTable("holidayreport_correction");
  $setup->dropTable("holidayreport_rule");
  $setup->dropTable("holidayreport_request");
  $setup->dropColumn("phase_activity", "holidayreport_isholiday");

?>