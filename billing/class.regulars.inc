<?php

  atkimport("module.billing.expense");
  
  useattrib("atknumberattribute");

  class regulars extends expense 
  {
    function regulars()
    {
      $this->expense("regulars");
      
      $this->addFilter("billing_expense.type='r'");
      
      $this->setSecurityAlias("billing.expense");
    }
    
    function initial_values()
    {
      $result = parent::initial_values();
      $result["type"] = 'r';
      return $result;      
    }

  }

?>