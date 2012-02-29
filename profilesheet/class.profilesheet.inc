<?php

userelation("atkmanytoonerelation");
userelation("atkonetomanyrelation");

useattrib("atknumberattribute");
useattrib("atkdateattribute");
useattrib("atkfileattribute");
useattrib("atktextattribute");
useattrib("atkupdatestampattribute");

class profilesheet extends atkNode
{

  function profilesheet()
  {
    $this->atkNode("profilesheet",NF_ADD_LINK|NF_EDITAFTERADD);
    
    $this->add(new atkManyToOneRelation("employee_id","employee.employee", AF_OBLIGATORY|AF_SEARCHABLE|AF_PRIMARY));
    $this->add(new atkAttribute("office",AF_OBLIGATORY|AF_SEARCHABLE));
    $this->add(new atkAttribute("education",AF_OBLIGATORY,50));
    $this->add(new atkAttribute("position",AF_OBLIGATORY,50));
    $this->add(new atkTextAttribute("projects", AF_HIDE_LIST));
    $this->add(new atkTextAttribute("endorsements", AF_HIDE_LIST));
    $this->add(new atkFileAttribute("photo",'./photos/profilesheet',AF_HIDE_LIST));
    $this->add(new atkOneToManyRelation("experiences","profilesheet.profilesheet_experience","profilesheet",AF_HIDE_LIST|AF_CASCADE_DELETE),"experiences");
    $this->add(new atkOneToManyRelation("certifications","profilesheet.profilesheet_certification","profilesheet",AF_HIDE_LIST|AF_CASCADE_DELETE),"certifications");
    $this->add(new atkUpdateStampAttribute("last_modified_on",AF_READONLY));
    
    $this->setOrder("employee_id");
    $this->setTable("profilesheet_profilesheet");
  }

  function descriptor_def()
  {
    return "[office]";
  }
  
}
?>