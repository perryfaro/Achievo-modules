<?php

  $setup->addColumn("docmanager_document", "legal_projectdoctype", "int", true, "0");
  $db = &atkdb::getInstance();
  $db->query("UPDATE docmanager_document SET legal_projectdoctype = 0 WHERE master='project.project';");

?>