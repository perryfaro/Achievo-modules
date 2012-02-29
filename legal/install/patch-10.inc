<?php

  // Add the openingswords column to the person table
  $setup->addColumn("person", "openingswords", "varchar(50)", true, "");

  // Copy openingswords as far as possible to the person
  $db = &atkGetDb();
  $projectcontactswithopeningswords = $db->getRows("SELECT personid, openingswords FROM project_person WHERE openingswords IS NOT NULL");
  foreach ($projectcontactswithopeningswords as $projectcontact)
  {
    $db->query(sprintf("UPDATE person SET openingswords='%s' WHERE id='%d'", $db->escapeSQL($projectcontact["openingswords"]), $projectcontact["personid"]));
  }

  // Drop the project_person openingswords column
  $setup->dropColumn("project_person", "openingswords");

?>