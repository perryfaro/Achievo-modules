<?php

  $setup->dropTable("billing_bill");
  $setup->dropTable("billing_bill_line");
  $setup->dropTable("billing_currency");
  $setup->dropTable("billing_discount");
  $setup->dropTable("billing_expense");
  $setup->dropTable("billing_rate");
  $setup->dropTable("billing_expensetype");
  $setup->dropTable("billing_expense_lock");

  $setup->dropColumn("hours", "billing_marked");
  $setup->dropColumn("hours", "billing_rate");
  $setup->dropColumn("hours", "billing_bill_line_id");
  $setup->dropColumn("hours", "billing_billed");

  $setup->dropColumn("organization", "billing_currency");

  $setup->dropColumn("project", "billing_expensereg_limit");
  $setup->dropColumn("project", "billing_type");
  $setup->dropColumn("project", "billing_hoursdeclarable");

  $setup->dropColumn("person", "billing_declarability");

?>