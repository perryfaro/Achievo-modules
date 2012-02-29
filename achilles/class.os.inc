<?php

useattrib("atkNumberAttribute");

class os extends atkNode 
{
  
  function os()
  {
    $this->atkNode("os");
    $this->add(new atkAttribute("os_id",AF_AUTOKEY));
    $this->add(new atkAttribute("name",AF_OBLIGATORY));
    $this->add(new atkNumberAttribute("sort"));
    
    $this->setOrder("sort,name"); 
    $this->setTable("achilles_os");  
  }
  
  function descriptor_def()
  {
    return "[name]"; 
  }
}
?>