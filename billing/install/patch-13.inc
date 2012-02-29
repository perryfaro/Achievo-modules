<?php

  // The contactperson relation will replace the addressing field. Although we don't
  // use the addressing field anymore, we don't drop this data as it can mean significant
  // data loss to users upgrading from previous versions.
  //$setup->dropColumn("billing_bill", "addressing");
  $setup->addColumn("billing_bill", "contactperson", "int(10)", true);
  $setup->addColumn("billing_bill", "billtype", "int(10)", true);

?>