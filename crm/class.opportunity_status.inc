<?php

class opportunity_status extends atkNode 
{
  
  function opportunity_status()
  {
    $this->atkNode("opportunity_status",NF_MRA|NF_ADD_LINK);
    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("status_name",AF_OBLIGATORY));
    
    $this->setSecurityAlias("crm.account");
    $this->setOrder("status_name");
    $this->setTable("crm_opportunity_status");
  }
  
  function descriptor_def()
  {
    return "[status_name]";
  }
}



?>