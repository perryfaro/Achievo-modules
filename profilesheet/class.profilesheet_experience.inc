<?php


class profilesheet_experience extends atkNode
{

  function profilesheet_experience()
  {
    $this->atkNode("profilesheet_experience");
    
    $this->add(new atkManyToOneRelation("profilesheet","profilesheet.profilesheet",AF_PRIMARY));
    $this->add(new atkManyToOneRelation("experience","profilesheet.experience",AF_PRIMARY));
    
    $this->setTable("profilesheet_profilesheet_experience");
    $this->setOrder("experience");
  }
}
?>