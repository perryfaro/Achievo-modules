<?php

  $setup->installNode("legal.casetype");
  $setup->addColumn("project_person", "legal_reference", "varchar(50)", true, "");
  $setup->addColumn("project", "legal_casenumber", "varchar(50)", true, "");
  $setup->addColumn("project", "legal_type", "int", true, "");
  $setup->addColumn("person", "legal_nextcasenumber", "int", true, "");

?>