<?php

class lead_status extends atkNode 
{
  
  function lead_status()
  {
    $this->atkNode("lead_status",NF_MRA|NF_ADD_LINK);
    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("lead_status",AF_OBLIGATORY));
    
    $this->setSecurityAlias("crm.account");
    $this->setOrder("lead_status");
    $this->setTable("crm_lead_status");
  }
  
  function descriptor_def()
  {
    return "[lead_status]";
  }
}



?>