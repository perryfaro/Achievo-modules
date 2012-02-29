<?php

class lead_source extends atkNode 
{
  
  function lead_source()
  {
    $this->atkNode("lead_source",NF_MRA|NF_ADD_LINK);
    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("lead_source",AF_OBLIGATORY));
    
    $this->setSecurityAlias("crm.account");
    $this->setOrder("lead_source");
    $this->setTable("crm_lead_source");
  }
  
  function descriptor_def()
  {
    return "[lead_source]";
  }
}



?>