<?php

  $setup->renameTable("weekplanning", "weekplanning_planning");
  $setup->renameTable("weekplanningconflictlog", "weekplanning_conflict");
  $setup->alterColumn("weekplanning_planning", "date", "monday", "date", false);
  $setup->alterColumn("weekplanning_planning", "hours", "minutes", "int(10)", false);

?>