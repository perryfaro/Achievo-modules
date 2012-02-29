<?php
  // this patch adds the fromdate/todate. The installnode call
  // will see the difference and perform a diff.
  $setup->installNode("holidayreport.holidayrequest");

?>