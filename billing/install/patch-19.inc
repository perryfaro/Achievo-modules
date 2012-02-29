<?php

$setup->addColumn("billing_expensetype", "km_specification", "int(1)", true, "0");
$setup->addColumn("billing_expensetype", "km_price", "decimal(10,2)", true, "0");

$setup->addColumn("billing_expense", "km", "int(5)", true, "0");

?>