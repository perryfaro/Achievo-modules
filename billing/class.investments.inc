<?php

  atkimport("module.billing.expense");
  
  useattrib("atknumberattribute");

  class investments extends expense 
  {
    function investments()
    {
      $this->expense("investments");
      
      $this->add(new atkNumberAttribute("period"),null,750);
      
      $p_attr = &$this->getAttribute("expensetype_id");
      $p_attr->addDestinationFilter("km_specification<>1");
      
      $this->addFilter("billing_expense.type='i'");
      
      $this->setSecurityAlias("billing.expense");
    }

    function initial_values()
    {
      $result = parent::initial_values();
      $result["type"] = 'i';
      return $result;      
    }
    
  }

?>