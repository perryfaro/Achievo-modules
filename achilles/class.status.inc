<?php
useattrib("atkNumberAttribute");
class status extends atkNode 
{
  
  function status()
  {
    $this->atkNode("status");
    $this->add(new atkAttribute("status_id",AF_AUTOKEY));
    $this->add(new atkAttribute("name",AF_OBLIGATORY));
    $this->add(new atkAttribute("description"));
    $this->add(new atkNumberAttribute("sort"));
    
    $this->setOrder("achilles_status.sort,achilles_status.name"); 
    $this->setTable("achilles_status");  
  }
  
  function descriptor_def()
  {
    return "[name]"; 
  }
}
?>