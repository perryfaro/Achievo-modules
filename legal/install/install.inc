<?php

  $setup->installNode("employee.employee");

  $setup->alterColumn("title","title","title","varchar(50)",true);

  $setup->addColumn("project_person", "legal_type", "varchar(50)", true, "");
  $setup->addColumn("project_person", "legal_reference", "varchar(50)", true, "");
  $setup->addColumn("person", "openingswords", "varchar(50)", true, "");

  $setup->addColumn("project", "legal_casenumber", "varchar(50)", true, "");
  $setup->addColumn("project", "legal_type", "int", true, "");

  $setup->addColumn("docmanager_document", "legal_projectdoctype", "int", true, "0");
  $setup->installNode("docmanager.documenttype");

  $setup->installNode("legal.casetype");
  $setup->installNode("legal.points");

  $db = &atkdb::getInstance();
  $db->query("UPDATE project_person SET legal_type = 'client';");

  $setup->setVersion(11);

?>