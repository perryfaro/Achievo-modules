<?php

useattrib("atkFileAttribute");
useattrib("atkTextAttribute");
useattrib("atkBoolAttribute");
useattrib("atkDateTimeAttribute");
userelation("atkManyToOneRelation");

class attachment extends atkNode 
{
  
  function attachment()
  {
    $this->atkNode("attachment");
    $this->add(new atkAttribute("attachment_id",AF_AUTOKEY));
    $this->add(new atkAttribute("bug_id",AF_OBLIGATORY));
    $file_attrib=&$this->add(new atkFileAttribute("filename",atkconfig("documentpath", "documents/"),AF_FILE_NO_AUTOPREVIEW|AF_FILE_PHYSICAL_DELETE|AF_FILE_NO_SELECT));
    $file_attrib->setAutonumbering(true); 
    $this->add(new atkTextAttribute("description"));
    $this->add(new atkAttribute("file_size",AF_HIDE_ADD|AF_READONLY));
    $this->add(new atkAttribute("mime_type",AF_HIDE_ADD|AF_READONLY));
    $this->add(new atkBoolAttribute("is_private"));
    $this->add(new atkManyToOneRelation("created_by","employee.employee",AF_READONLY));
    $this->add(new atkDateTimeAttribute("created_on","","",AF_READONLY));
    $this->setSecurityAlias("achilles.bug");

    $this->setOrder("created_on");
    $this->setTable("achilles_attachment");
  }
  
  function descriptor_def()
  {
    return "[filename]";
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
  
  function preAdd(&$rec)
  {
	$rec["file_size"]=$rec["filename"]["filesize"]/1000;
	
  }
   function preUpdate(&$rec)
  {
	$rec["file_size"]=$rec["filename"]["filesize"]/1000;
	
  }
  function file_size_display($record)
  {
	  $kbytes=$record["file_size"];
	  if($kbytes>=0) return $kbytes." Kb.";
	  return "";
  }
}




?>