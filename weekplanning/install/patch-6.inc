<?php

  if (moduleExists("billing"))
  {
    $setup->addColumn("billing_projecttype", "weekplanning_avaragerate", "decimal(10,2)", true);
  }

?>