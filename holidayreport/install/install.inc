<?php

  $setup->addColumn("phase_activity", "holidayreport_isholiday", "int(1)", false, "0");
  $setup->installNode("holidayreport.correction");
  $setup->installNode("holidayreport.leave_type");
  $setup->installNode("holidayreport.rule");

  $setup->installNode("holidayreport.holidayrequest");
  $setup->installNode("holidayreport.holidayrequest_review");

  $setup->addColumn("holidayreport_rule","employer_id","INT(10)");

  $setup->setVersion(6);

?>