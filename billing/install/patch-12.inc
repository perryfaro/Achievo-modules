<?php

  $setup->dropColumn("projecttype", "rate");
  $setup->renameTable("projecttype", "billing_projecttype");
  $setup->addColumn("billing_rate", "projecttype_id", "int(10)", true);

?>