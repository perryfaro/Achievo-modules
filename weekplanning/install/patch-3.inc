<?php

  $setup->alterColumn("weekplanning_conflict", "description", "type", "varchar(100)", false);
  $setup->addColumn("weekplanning_conflict", "status", "varchar(20)", false);

?>