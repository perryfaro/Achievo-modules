<?php

  // Add weekplanning_employee_budget column to project table
  $setup->addColumn("project", "weekplanning_employee_budget", "decimal(13,5)", true);

?>