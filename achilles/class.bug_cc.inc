<?php
userelation("atkManyToOneRelation");

class bug_cc extends atkNode 
{
  
  function bug_cc()
  {
    $this->atkNode("bug_cc",NF_NO_EDIT|NF_NO_VIEW);
    $this->add(new atkManyToOneRelation("bug_id","achilles.bug",AF_PRIMARY|AF_LARGE));
    $this->add(new atkManyToOneRelation("user_id","employee.employee",AF_PRIMARY|AF_LARGE));
    $this->setSecurityAlias("achilles.bug");
    $this->setOrder("user_id");
    $this->setTable("achilles_bug_cc");  
  }
  
  function descriptor_def()
  {
    return "[user_id]"; 
  }
}
?>