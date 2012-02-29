<?php

  $db = &atkGetDb();
  $db->query("UPDATE accessright SET node='billing.billing_rate' WHERE node='billing.rate'");

?>