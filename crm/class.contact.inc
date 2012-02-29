<?php
useattrib("atklistattribute");
useattrib("atkemailattribute");
useattrib("atktextattribute");
useattrib("atkdateattribute");
useattrib("atkboolattribute");
useattrib("atkUpdateStampAttribute");
useattrib("atkaggregatedcolumn");
userelation("atkonetomanyrelation");
userelation("atkmanytoonerelation");
useattrib("crm.lastModifiedByAttribute");

class contact extends atkNode
{
  function contact()
  {
    $this->atkNode("contact", NF_ADD_LINK|NF_IMPORT|NF_EXPORT);    
    $this->add(new atkAttribute("id" ,AF_AUTOKEY));
    
    $this->add(new atkManyToOneRelation("salutation","crm.salutation",AF_HIDE_LIST));
    $this->add(new atkAttribute("lastname",AF_SEARCHABLE|AF_OBLIGATORY|AF_HIDE_LIST, 50));
    $this->add(new atkAttribute("firstname",AF_FORCE_LOAD|AF_HIDE_LIST, 50));
    $this->add(new atkAggregatedColumn("name","[firstname] [lastname]",AF_SEARCHABLE,array("firstname","lastname")));   
    
    $this->add(new atkListAttribute("gender",array("male","female","unknown"),array("m","f","u"),AF_HIDE_LIST));
    $this->add(new atkAttribute("title", 20));
    $this->add(new atkManyToOneRelation("lead","crm.lead_source",AF_HIDE_LIST));
    $this->add(new atkManyToOneRelation("account","crm.account",AF_LARGE|AF_SEARCHABLE));
    $this->add(new atkAttribute("division",AF_HIDE_LIST));
    
    $preFilter = new atkManyToOneRelation("reports_to","crm.contact",AF_RELATION_AUTOLINK|AF_HIDE_LIST);
    $preFilter->m_destinationFilter = "account = '[account.id]'";
    $this->add($preFilter);
    
    $this->add(new atkDateAttribute("birthday","","",0,0,AF_HIDE_LIST));
    $this->add(new atkAttribute("address",AF_HIDE_LIST, 100));
    $this->add(new atkAttribute("zipcode",AF_HIDE_LIST, 20));
    $this->add(new atkAttribute("city",AF_HIDE_LIST, 100));
    $this->add(new atkAttribute("state",AF_HIDE_LIST, 100));
    $this->add(new atkAttribute("country",AF_HIDE_LIST, 100));
    $this->add(new atkAttribute("workphone", 0, 20));
    $this->add(new atkAttribute("cellphone", AF_HIDE_LIST|AF_FORCE_LOAD, 20));
    $this->add(new atkAttribute("homephone", AF_HIDE_LIST|AF_FORCE_LOAD, 20));
    $this->add(new atkAttribute("otherphone", AF_HIDE_LIST|AF_FORCE_LOAD, 20));
    $this->add(new atkAttribute("fax", AF_HIDE_LIST|AF_FORCE_LOAD, 20));
    $this->add(new atkEmailAttribute("email", false,AF_SEARCHABLE, 50));
    $this->add(new atkEmailAttribute("otheremail", false, AF_HIDE_LIST, 50));
    $this->add(new atkBoolAttribute("donotcall",AF_HIDE_LIST));
    $this->add(new atkManyToOneRelation("assigned_to","employee.employee",AF_SEARCHABLE));
    $this->add(new atkTextAttribute("remark", TEXT_LARGE, AF_HIDE_LIST));
    $this->add(new atkOneToManyRelation("activity","crm.activity","parent_id",AF_HIDE_LIST),"activity");

    $this->add(new atkManyToOneRelation("created_by","employee.employee",AF_HIDE_LIST|AF_READONLY),"history");
    $this->add(new atkDateTimeAttribute("created_on","","",AF_HIDE_LIST|AF_READONLY),"history");
    $this->add(new lastModifiedByAttribute("last_modified_by","employee.employee"),"history");
    $this->add(new atkUpdateStampAttribute("last_modified_on",AF_HIDE_LIST|AF_READONLY),"history");
    
    $this->setTable("crm_contact", "crm_contact");
    $this->setOrder("crm_contact.lastname");
    $this->setIndex("lastname");
    
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
  
  function name_display($record)
  {
    global $ATK_VARS;
    atk_var_dump($record);
    $action = $ATK_VARS["atkaction"];
    $name = $record["firstname"].($record["firstname"]!=""?" ":"").$record["lastname"];
    $viewUrl = session_url($_SERVER["PHP_SELF"].'?atknodetype='.$this->atkNodeType().
                              '&atkaction=view&atkselector='.rawurlencode($this->primaryKey($record)),
                               SESSION_NESTED);
    if (strlen($name)>0 && ($action!="view" && $action!="select")) return '<a href="'.$viewUrl.'">'.$name.'</a>';
    return $name;
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
  
  function workphone_display($record)
  {
    if($record["workphone"]=="")
    {
      return $record["account"]["phone"];
    }
    return $record["workphone"];
    
  }
  
  
  function descriptor_def()
  {
    return "[lastname], [firstname]";
  }

}

?>