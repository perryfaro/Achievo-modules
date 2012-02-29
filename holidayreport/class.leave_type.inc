<?php

class leave_type extends atkNode
{
  function leave_type()
  {
    $this->atkNode("leave_type", NF_NO_DELETE);

    $this->add(new atkAttribute("id"                 , AF_AUTOKEY));
    $this->add(new atkAttribute("description"        , AF_OBLIGATORY, 50));
    $this->setTable("holidayreport_leave_type");
    $this->setOrder("description");
    
    $this->setSecurityAlias("holidayreport.rule");
  }

  function descriptor_def()
  {
    return "[description]";
  }
}
?>