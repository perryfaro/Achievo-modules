<?php

  $setup->dropColumn("project", "billing_type");
  $setup->installNode("billing.projecttype");
  $setup->addColumn("project", "billing_type", "int(10)", true, "0");

?>