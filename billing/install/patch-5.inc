<?php

  $setup->addColumn("billing_rate", "starttime", "time", false);
  $setup->addColumn("billing_rate", "endtime", "time", false);
  
  $setup->addColumn("billing_rate", "report", "int(10)", false);
  $setup->addColumn("billing_rate", "ratetype", "int(1)", false);  
  
  $setup->addColumn("billing_rate", "phaseid", "int(10)", false);  

?>