<?php
  $setup->dropColumn("functionlevel", "name");
  $setup->addColumn("functionlevel", "name", "int(10)");
    
  $setup->setVersion(1);
?>