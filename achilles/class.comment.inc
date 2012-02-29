<?php
useattrib("atkTextAttribute");
useattrib("atkBoolAttribute");
useattrib("atkDateTimeAttribute");
userelation("atkManyToOneRelation");

class comment extends atkNode 
{
  
  function comment()
  {
    $this->atkNode("comment",NF_NO_DELETE|NF_NO_EDIT|NF_NO_VIEW);
    $this->add(new atkAttribute("comment_id",AF_AUTOKEY));
    $this->add(new atkAttribute("bug_id"));
    $this->add(new atkTextAttribute("comment_text"));
    $this->add(new atkBoolAttribute("is_private","","",AF_HIDE_LIST));
    $this->add(new atkManyToOneRelation("created_by","employee.employee",AF_READONLY));
    $this->add(new atkDateTimeAttribute("created_on","","",AF_READONLY));
    $this->setSecurityAlias("achilles.bug");

    $this->setOrder("created_on");
    $this->setTable("achilles_comment");  
  }
  
  function initial_values()
  {
    $user = getUser();
    $now = array("year"=>date("Y"),
                 "month"=>date("m"),
                 "day"=>date("d"),
                 "hours"=>date("H"),
                 "minutes"=>date("i"),
                 "seconds"=>date("s"));
                 
   return array("created_on"=>$now,
                "created_by"=>array("id"=>$user["id"]));
  }
}
?>