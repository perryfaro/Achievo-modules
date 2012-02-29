<?php
useattrib("atkBoolAttribute");
useattrib("atkDateTimeAttribute");
useattrib("atkUpdateStampAttribute");
useRelation("atkManyToOneRelation");
useattrib("achilles.lastModifiedByAttribute");

class milestone extends atkNode 
{
  
  function milestone()
  {
    global $g_sessionManager;
    $this->atkNode("milestone");
    $this->add(new atkAttribute("milestone_id",AF_AUTOKEY));
    $this->add(new atkManyToOneRelation("project_id","project.project",AF_OBLIGATORY));
    $this->add(new atkAttribute("name",AF_OBLIGATORY));
    $this->add(new atkAttribute("description"));
    $this->add(new atkDateAttribute("deadline"));
    $this->add(new atkBoolAttribute("is_active"));
    $this->add(new atkManyToOneRelation("created_by","employee.employee",AF_HIDE_LIST|AF_READONLY),"history");
    $this->add(new atkDateTimeAttribute("created_on","","",AF_HIDE_LIST|AF_READONLY),"history");
    $this->add(new lastModifiedByAttribute("last_modified_by","employee.employee"),"history");
    $this->add(new atkUpdateStampAttribute("last_modified_on",AF_HIDE_LIST|AF_READONLY),"history");

    $prjid = $g_sessionManager->pageVar("selectedprojectid");
    if($prjid!="")
    {
      $this->addFilter("achilles_milestone.project_id=$prjid"); 
    }
    
    $this->setOrder("name");  
    $this->setTable("achilles_milestone");  
  }
  
  function descriptor_def()
  {
    return "[name]"; 
  }
  
  function initial_values()
  {
    global $g_sessionManager;
    $user = getUser();
    $now = array("year"=>date("Y"),
                 "month"=>date("m"),
                 "day"=>date("d"),
                 "hours"=>date("H"),
                 "minutes"=>date("i"),
                 "seconds"=>date("s"));
                 
    return array("project_id"=>array("id"=>$g_sessionManager->pageVar("selectedprojectid")),
                 "created_on"=>$now,
                 "created_by"=>array("id"=>$user["id"]),
                 "last_modified_by"=>array("id"=>$user["id"],
                 "is_active"=>true));
  }
}
?>