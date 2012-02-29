<?php
  $setup->dropTable("weekplanning_planning");
  $setup->dropTable("weekplanning_conflicts");

  if (moduleExists("billing") && $setup->columnExists("billing_projecttype", "weekplanning_avaragerate"))
  {
    $setup->dropColumn("billing_projecttype", "weekplanning_avaragerate");
  }

?>