<?php
  // Add the calculationtype column
  $setup->addColumn("billing_rate", "calculationtype", "int(10)", false);

  // Set the calculationtype for every billing_rate to RATECALCULATION_NORMAL
  $db = &atkGetDb();
  $db->query(sprintf("UPDATE billing_rate SET calculationtype='%d'", RATECALCULATION_NORMAL));
?>