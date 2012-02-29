<?php
  /*@var $setup setup*/
  $setup->addColumn("carlease","leasecontractnumber","VARCHAR(30)");  
  $setup->addColumn("carlease","fuel","INT(11)");  
  $setup->addColumn("carlease","insurancecompany","INT(11)");  
  $setup->addColumn("carlease","insuranceownrisk","INT(11)");  

  $setup->setVersion(2);

?>