<?php

  $setup->addColumn("project_person", "legal_type", "varchar(50)", true, "");

  $db = &atkdb::getInstance();
  $db->query("UPDATE project_person SET legal_type = 'client';");

?>