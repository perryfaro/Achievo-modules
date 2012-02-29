<?php

  // Make the billing_taxrate.percentage column non-obligatory
  $setup->alterColumn("billing_taxrate", "percentage", "percentage", "decimal(10,1)", true);
?>