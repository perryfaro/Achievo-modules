<?php

  $setup->installNode("billing.expensetype");
  $setup->installNode("billing.expense_lock");

  $setup->addColumn("billing_expense", "expensetype_id", "int(10)", false, 0);
  $setup->addColumn("billing_expense", "billable", "int(1)", false, 0); 
  $setup->addColumn("billing_expense", "entrydate", "date", false, 0); 
  $setup->addColumn("billing_expense", "remark", "text", false, 0); 

  //$setup->dropColumn("billing_expense", "shortdescription");
  //$setup->dropColumn("billing_expense", "description");
?>
