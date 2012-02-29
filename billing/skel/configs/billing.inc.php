<?php

  // Bill document folder location (defaults to the billing/bills folder)
  $config['bill_billdir'] = "";

  // Block billing when the same week for timereg is blocked
  $config['block_with_timereg'] = false;
  
  // E-mail address from where the alert e-mails will be send
  $config['frommail'] = "automailer@achievo.org";

  // Current year used for generating bills
  // if not set or set to 0, current year will used (based on date)
  $config['financial_year'] = 0;
  
  // billnumber template - is used for building the billnumber.
  // Available tags:
  // [year]       current year    (YYYY)
  // [month]      current month   (mm)
  // [day]        current day     (dd)
  // [fin_year]   financial year  (YYYY) (as configured above)
  //
  // All billnumbers are postfixed by an auto-incrementing number ## (or more digits if needed)
  //
  // For example:
  // $config['billnumber_template'] = "[year][month][day]";
  $config['billnumber_template'] = "[fin_year][month][day]";
  
  // templates in de 'templates' dir of the billing module that should
  // be used for generating reminder documents.
  $config['reminderdoc_templates'] = array("reminder","second_reminder");

?>
