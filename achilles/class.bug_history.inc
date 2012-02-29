<?php
useattrib("atkDateTimeAttribute");
userelation("atkManyToOneRelation");

class bug_history extends atkNode 
{
  
  function bug_history()
  {
    $this->atkNode("bug_history",NF_NO_EDIT,NF_NO_DELETE,NF_NO_VIEW);
    $this->add(new atkAttribute("bug_id",AF_HIDE));
    $this->add(new atkAttribute("changed_field"));
    $this->add(new atkAttribute("old_value"));
    $this->add(new atkAttribute("new_value"));
    $this->add(new atkManyToOneRelation("created_by","employee.employee",AF_READONLY));
    $this->add(new atkDateTimeAttribute("created_on","","",AF_READONLY));
    $this->setSecurityAlias("achilles.bug");
    $this->setOrder("created_on DESC");
    $this->setTable("achilles_bug_history");  
  }
  
}
?>