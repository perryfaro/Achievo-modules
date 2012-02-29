<?php

  $setup->alterColumn("project", "expensereg_limit",     "billing_expensereg_limit", "int(1)",  false, "0");
  $setup->alterColumn("project", "type",                 "billing_type",             "int(1)", true,  "0");

  $setup->alterColumn("person",  "target_declarability", "billing_declarability",    "int(10)");

?>