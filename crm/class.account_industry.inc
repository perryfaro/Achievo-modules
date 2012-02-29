<?php

class account_industry extends atkNode 
{
  
  function account_industry()
  {
    $this->atkNode("account_industry",NF_MRA|NF_ADD_LINK);
    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("industry_name",AF_OBLIGATORY));
    
    $this->setSecurityAlias("crm.account");
    $this->setOrder("industry_name");
    $this->setTable("crm_account_industry");
  }
  
  function descriptor_def()
  {
    return "[industry_name]";
  }
}



?>