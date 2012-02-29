<?php

  $setup->installNode("hourdeposit.deposit");  
  $setup->addColumn("project", "hourdeposit_deposit_id", "int(10)");

  $setup->setVersion(2);

?>