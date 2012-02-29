<?php

useattrib("atknumberattribute");

class experience_category extends atkNode
{

  function experience_category()
  {
    $this->atkNode("experience_category");
    
    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkNumberAttribute("displayorder",AF_OBLIGATORY));
    $this->add(new atkAttribute("description",AF_OBLIGATORY|AF_SEARCHABLE));

    $this->setOrder("displayorder");
    $this->setTable("profilesheet_experience_category");
  }

  function descriptor_def()
  {
    return "[description]";
  }
  
}
?>