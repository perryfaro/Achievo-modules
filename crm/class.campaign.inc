<?php

useattrib("atkdateattribute");
useattrib("atktextattribute");
useattrib("atkcurrencyattribute");
useattrib("atkUpdateStampAttribute");
userelation("atkmanytoonerelation");
useattrib("crm.lastModifiedByAttribute");

class campaign extends atkNode 
{
  
  function campaign()
  {
    $this->atkNode("campaign",NF_MRA|NF_ADD_LINK);
    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("title",AF_OBLIGATORY|AF_SEARCHABLE));
    $this->add(new atkManyToOneRelation("type","crm.campaign_type",AF_SEARCHABLE));
    $this->add(new atkManyToOneRelation("status","crm.campaign_status",AF_SEARCHABLE));
    $this->add(new atkDateAttribute("start_date"));
    $this->add(new atkDateAttribute("end_date"));
    $this->add(new atkCurrencyAttribute("budget",AF_HIDE_LIST));
    $this->add(new atkCurrencyAttribute("actual_cost",AF_HIDE_LIST));
    $this->add(new atkCurrencyAttribute("expected_revenue",AF_HIDE_LIST));
    $this->add(new atkCurrencyAttribute("expected_cost",AF_HIDE_LIST));
    $this->add(new atkTextAttribute("objective",AF_HIDE_LIST));
    $this->add(new atkTextAttribute("description",AF_HIDE_LIST));
    
    
    $this->add(new atkManyToOneRelation("created_by","employee.employee",AF_HIDE_LIST|AF_READONLY),"history");
    $this->add(new atkDateTimeAttribute("created_on","","",AF_HIDE_LIST|AF_READONLY),"history");
    $this->add(new lastModifiedByAttribute("last_modified_by","employee.employee"),"history");
    $this->add(new atkUpdateStampAttribute("last_modified_on",AF_HIDE_LIST|AF_READONLY),"history");
     
    
    
    $this->setSecurityAlias("crm.account");
    $this->setOrder("title");
    $this->setTable("crm_campaign");
  }
  
  function initial_values()
  {
    $user = getUser();
    return array("created_by"=>$user["id"],
                 "last_modified_by"=>$user["id"],
                 "created_on"=>array("year"=>date("Y"),
                                     "month"=>date("m"),
                                     "day"=>date("d"),
                                     "hours"=>date("H"),
                                     "minutes"=>date("i"),
                                     "seconds"=>date("s")));
    
    
  }
  
  
  
  
  function descriptor_def()
  {
    return "[title]";
  }
}



?>