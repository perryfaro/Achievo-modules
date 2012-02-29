<?php


class profilesheet_certification extends atkNode
{
  function profilesheet_certification()
  {
    $this->atkNode("profilesheet_certification");
    
    $this->add(new atkManyToOneRelation("profilesheet","profilesheet.profilesheet", AF_PRIMARY));
    $this->add(new atkManyToOneRelation("certification","profilesheet.certification", AF_PRIMARY));
    
    $this->setTable("profilesheet_profilesheet_certification");
    $this->setOrder("certification");
  }
}
?>