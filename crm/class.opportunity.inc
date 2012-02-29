<?php

useattrib("atkdateattribute");
useattrib("atktextattribute");
useattrib("atkListAttribute");
useattrib("atkCurrencyAttribute");
userelation("atkonetomanyrelation");
useattrib("atkUpdateStampAttribute");
userelation("atkmanytoonerelation");
useattrib("crm.lastModifiedByAttribute");

class opportunity extends atkNode 
{
  
  function opportunity()
  {
    $this->atkNode("opportunity",NF_MRA|NF_ADD_LINK);
    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("title",AF_OBLIGATORY));
    $this->add(new atkManyToOneRelation("account","crm.account",AF_OBLIGATORY|AF_LARGE));
    $preFilter = new atkManyToOneRelation("contact","crm.contact",AF_RELATION_AUTOLINK);
    $preFilter->m_destinationFilter = "account = '[account.id]'";
    $this->add($preFilter);
    $this->add(new atkManyToOneRelation("campaign","crm.campaign",AF_HIDE_LIST));
    $this->add(new atkManyToOneRelation("lead","crm.lead_source",AF_HIDE_LIST));
    $this->add(new atkManyToOneRelation("status","crm.opportunity_status",AF_OBLIGATORY));
    $this->add(new atkCurrencyAttribute("amount"));
    $this->add(new atkListAttribute("probability",array("0%","10%","20%","30%","40%","50%","60%","70%","80%","90%","100%"),array(0,10,20,30,40,50,60,70,80,90,100),AF_HIDE_LIST));
    $this->add(new atkDateAttribute("close_date","","",0,0,AF_OBLIGATORY));
    $this->add(new atkManyToOneRelation("assigned_to","employee.employee"));
    $this->add(new atkTextAttribute("description",AF_HIDE_LIST));
    $this->add(new atkOneToManyRelation("activity","crm.opportunity_activity","parent_id",AF_HIDE_LIST),"activity");
    
    $this->add(new atkManyToOneRelation("created_by","employee.employee",AF_HIDE_LIST|AF_READONLY),"history");
    $this->add(new atkDateTimeAttribute("created_on","","",AF_HIDE_LIST|AF_READONLY),"history");
    $this->add(new lastModifiedByAttribute("last_modified_by","employee.employee"),"history");
    $this->add(new atkUpdateStampAttribute("last_modified_on",AF_HIDE_LIST|AF_READONLY),"history");
    
    $this->setSecurityAlias("crm.opportunity");
    $this->setOrder("title");
    $this->setTable("crm_opportunity");
  }
  
  function descriptor_def()
  {
    return "[title]";
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
  
  function title_display($record)
  {
    global $ATK_VARS;
    $action = $ATK_VARS["atkaction"];
    $title = $record["title"];
    $viewUrl = session_url($_SERVER["PHP_SELF"].'?atknodetype='.$this->atkNodeType().
                              '&atkaction=view&atkselector='.rawurlencode($this->primaryKey($record)),
                               SESSION_NESTED);
    if (strlen($title)>0 && ($action!="view" && $action!="select")) return '<a href="'.$viewUrl.'">'.$title.'</a>';
    return $title;
  }
  
  function account_display($record)
  {
    global $ATK_VARS;
    atk_var_dump($record,"RECORD");
    $action = $ATK_VARS["atkaction"];
    $accountName = $record["account"]["name"].($record["account"]["name"]!=""?", ":"").$record["account"]["visit_city"];
    $viewUrl = session_url($_SERVER["PHP_SELF"].'?atknodetype=crm.account&atkaction=view&atkselector='.rawurlencode($record["account"]["id"]),SESSION_NESTED);
    if (strlen($accountName)>0 && ($action!="view" && $action!="select")) return '<a href="'.$viewUrl.'">'.$accountName.'</a>';
    return $accountName;
    
  }
}



?>