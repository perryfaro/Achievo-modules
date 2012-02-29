<?php
  if (!$setup->columnExists("billing_bill", "role_id"))
  {
    $setup->addColumn("billing_bill", "role_id", "int(10)", true);
  }
?>
