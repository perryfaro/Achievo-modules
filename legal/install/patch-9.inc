<?php

  // Multiply all points by 3*60 to store points as minutes.
  $db = &atkGetDb();
  $db->query("UPDATE legal_points SET points=(points*180)");

  // Convert the points column to an integer format
  $setup->alterColumn("legal_points","points","points","int(10)",true);

?>