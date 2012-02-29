<?php

class salutation extends atkNode 
{
  
  function salutation()
  {
    $this->atkNode("salutation",NF_MRA|NF_ADD_LINK);
    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("salutation",AF_OBLIGATORY));
    
    $this->setSecurityAlias("crm.account");
    $this->setOrder("salutation");
    $this->setTable("crm_salutation");
  }
  
  function descriptor_def()
  {
    return "[salutation]";
  }
}



?>