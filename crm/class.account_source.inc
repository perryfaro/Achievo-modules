<?php

class account_source extends atkNode 
{
  
  function account_source()
  {
    $this->atkNode("account_source",NF_MRA|NF_ADD_LINK);
    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("source_name",AF_OBLIGATORY));
    
    $this->setSecurityAlias("crm.account");
    $this->setOrder("source_name");
    $this->setTable("crm_account_source");
  }
  
  function descriptor_def()
  {
    return "[source_name]";
  }
}



?>