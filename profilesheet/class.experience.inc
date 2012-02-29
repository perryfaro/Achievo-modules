<?php

userelation("atkmanytoonerelation");
useattrib("atknumberattribute");

class experience extends atkNode
{

  function experience()
  {
    $this->atkNode("experience");
    
    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkManyToOneRelation("category_id","profilesheet.experience_category", AF_OBLIGATORY|AF_SEARCHABLE));
    $this->add(new atkNumberAttribute("displayorder",AF_OBLIGATORY));
    $this->add(new atkAttribute("description",AF_OBLIGATORY|AF_SEARCHABLE));
    
    //$this->add(new atkOneToManyRelation("experiences","profilesheet.profilesheet_experience","experience",AF_HIDE|AF_CASCADE_DELETE));
    
    $this->setOrder("category_id, profilesheet_experience.displayorder");
    $this->setTable("profilesheet_experience");
  }

  function descriptor_def()
  {
    return "[category_id.description] - [description]";
  }
  
}
?>