<?php

class campaign_type extends atkNode 
{
  
  function campaign_type()
  {
    $this->atkNode("campaign_type",NF_MRA|NF_ADD_LINK);
    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("type_name",AF_OBLIGATORY));
    
    $this->setSecurityAlias("crm.account");
    $this->setOrder("type_name");
    $this->setTable("crm_campaign_type");
  }
  
  function descriptor_def()
  {
    return "[type_name]";
  }
}



?>