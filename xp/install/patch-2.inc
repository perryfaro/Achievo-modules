<?php

  $setup->addColumn("xp_story", "submitted_by", "int(10)");
  $setup->addColumn("xp_story", "submitted_on", "date");
  $setup->addColumn("xp_story", "accepted_on","date");
  $setup->addColumn("xp_story", "planned_on","date");
  $setup->addColumn("xp_story", "implemented_on","date");
  $setup->addColumn("xp_story", "comments","text");
  $setup->addColumn("xp_story", "project","int(10)");

?>