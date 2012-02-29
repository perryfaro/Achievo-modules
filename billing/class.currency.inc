<?php
  
  useattrib("atknumberattribute");
  
  class currency extends atkNode
  {
    function currency()
    {
      $this->atkNode("currency");
  
      $this->add(new atkAttribute("symbol",AF_PRIMARY|AF_OBLIGATORY, 10));
      $this->add(new atkAttribute("name" ,AF_UNIQUE, 50));
      $this->add(new atkNumberAttribute("value",AF_OBLIGATORY, 10, 5));
      
  
      $this->setTable("billing_currency");
      $this->setOrder("name");
    }
  
    function descriptor_def()
    {
      return "[name] ([symbol])";
    }
    
    function value_edit($record, $mode)
    {
      $org = $this->m_attribList["value"]->edit($record, $mode);
      return $org." ".atkconfig("default_currency", "EUR");
    }
    
    function value_display($record, $mode)
    {
      $org = $this->m_attribList["value"]->display($record, $mode);
      return $org." ".atkconfig("default_currency", "EUR");
    }
  
  }

?>
