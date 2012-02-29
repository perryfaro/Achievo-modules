<?php

  $setup->installNode("billing.bill");
  $setup->installNode("billing.bill_reminder_number");
  $setup->installNode("billing.bill_line");
  $setup->installNode("billing.currency");
  $setup->installNode("billing.discount");
  $setup->installNode("billing.expense");
  $setup->installNode("billing.billing_rate");
  $setup->installNode("billing.expensetype");
  $setup->installNode("billing.expense_lock");
  $setup->installNode("billing.income");
  $setup->installNode("billing.fund");
  $setup->installNode("billing.projecttype");
  $setup->installNode("billing.taxrate");

  $setup->addColumn("hoursbase", "billing_marked", "int(1)", false, 0);
  $setup->addColumn("hoursbase", "billing_billed", "int(1)", false, 0); // was once 'onbill'
  $setup->addColumn("hoursbase", "billing_rate", "decimal(10,5)", true); // was 'hour_rate'
  $setup->addColumn("hoursbase", "billing_bill_line_id", "int(10)", true);

  $setup->addColumn("organization", "billing_currency", "varchar(10)", true);

  $setup->addColumn("project", "billing_expensereg_limit", "int(1)", false, "0");
  $setup->addColumn("project", "billing_type", "int(10)", true, "0");
  $setup->addColumn("project", "billing_hoursdeclarable", "int(1)", false, "0");

  $setup->addColumn("person", "billing_declarability", "int(10)");

  $setup->installNode("billing.expense_category");

  $setup->addColumn("billing_expense","period","INT(11)"); // investments require an additional field.

  // Create the hours view again
  $setup->dropView("hours");
  $setup->createView("hours","SELECT * FROM hoursbase WHERE virtual_time=0 OR virtual_time IS NULL", false);


  $setup->setVersion(25);

?>
