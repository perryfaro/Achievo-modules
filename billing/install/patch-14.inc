<?php

  $setup->installNode("billing.taxrate");
  $setup->addColumn("billing_bill_line", "taxrate", "int(10)", false);

?>