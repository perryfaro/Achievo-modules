<?php

class activity_type extends atkNode 
{
  
  function activity_type()
  {
    $this->atkNode("activity_type",NF_MRA|NF_ADD_LINK);
    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("type_name",AF_OBLIGATORY));
    
    $this->setSecurityAlias("crm.account");
    $this->setOrder("type_name");
    $this->setTable("crm_activity_type");
  }
  
  function descriptor_def()
  {
    return "[type_name]";
  }
}



?>