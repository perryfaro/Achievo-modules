<?php

	// ----------------------------------------------------------
  //            EXTERNAL MODULE CARLEASE CONFIGURATION
  // ----------------------------------------------------------

  // This variable configures the moments when a mail should be send to the carlease coordinator, indicating
  // that a leasecontract expires within the specified amount of time. More then one entry is allowed.
  // 
  // Example :
  // $config["carlease_contracts_expiration_values"] = array('day'=>0,'month'=>4,'year'=>0);
  //
  // Explanation:
  // An email will be sent in 4 monts to the e-mailadres what is specified @ carlease_contracts_expiration_sendmail   
  //  
   $config["carlease_contracts_expiration_values"] = array('day'=>0,'month'=>4,'year'=>0);

    
  // This variable configures that a warning mail will be send to each of these mail-addresses 
  // indicating that a specified usercontract expires.  
  // Example:
  // $config["carlease_contracts_expiration_sendmail"] = array("someone@somewhere.nl", "anyone@otherdomain.com");
?>
