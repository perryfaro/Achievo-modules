<?php

  $setup->addColumn("holidayreport_rule","weekhours","DECIMAL(6,2)");
  //set default week work hours to 40
  $db->query("UPDATE holidayreport_rule SET weekhours='40'");
?>