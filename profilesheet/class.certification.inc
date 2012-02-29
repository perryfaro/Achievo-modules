<?php

useattrib("atkfileattribute");

class certification extends atkNode
{

  function certification()
  {
    $this->atkNode("certification");
    
    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("description",AF_OBLIGATORY|AF_SEARCHABLE));
    $this->add(new atkFileAttribute("logo",'./photos/certification'));
    
   // $this->add(new atkOneToManyRelation("certifications","profilesheet.profilesheet_certification","certification",AF_HIDE|AF_CASCADE_DELETE));

    $this->setOrder("profilesheet_certification.description");
    $this->setTable("profilesheet_certification");
  }

  function descriptor_def()
  {
    return "[description]";
  }
  
}
?>