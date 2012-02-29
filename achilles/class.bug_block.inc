<?php
userelation("atkManyToOneRelation");

class bug_block extends atkNode 
{
  
  function bug_block()
  {
    $this->atkNode("bug_block",NF_NO_EDIT|NF_NO_VIEW);
    $this->add(new atkManyToOneRelation("bug_id","achilles.bug",AF_PRIMARY|AF_LARGE));
    $this->add(new atkManyToOneRelation("depends_on","achilles.bug",AF_PRIMARY|AF_LARGE));
    $this->setSecurityAlias("achilles.bug");    
    $this->setOrder("depends_on");
    $this->setTable("achilles_bug_dependency");  
  }
  
  function descriptor_def()
  {
    return "[depends_on]"; 
  }
}
?>