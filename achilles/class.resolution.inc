<?php
useattrib("atkNumberAttribute");

class resolution extends atkNode 
{
  
  function resolution()
  {
    $this->atkNode("resolution");
    $this->add(new atkAttribute("resolution_id",AF_AUTOKEY));
    $this->add(new atkAttribute("name",AF_OBLIGATORY));
    $this->add(new atkAttribute("description"));
    $this->add(new atkNumberAttribute("sort"));
    $this->setOrder("achilles_resolution.sort,achilles_resolution.name");
    $this->setTable("achilles_resolution");  
  }
  
  function descriptor_def()
  {
    return "[name]"; 
  }
}
?>