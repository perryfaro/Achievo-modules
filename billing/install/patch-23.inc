<?php

$setup->addColumn("billing_expense","vat","INT(11)");
$setup->addColumn("billing_expense","type","CHAR(1)");

$db = &atkGetDb();
$db->query("UPDATE billing_expense SET type='r'");

$setup->addColumn("billing_expense","period","INT(11)");
$setup->installNode("billing.expense_category");
$setup->addColumn("billing_expense","category","INT(11)");
$setup->addColumn("billing_expense","subcategory","INT(11)");

?>