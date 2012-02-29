<?php
useattrib("atkListAttribute");
useattrib("atkTextAttribute");
useattrib("atkDateAttribute");
useattrib("atkEmailAttribute");
useattrib("atkaggregatedcolumn");
useattrib("atkUpdateStampAttribute");
userelation("atkManyToOneRelation");
useattrib("crm.lastModifiedByAttribute");

class lead extends atkNode
{
  function lead()
  {
    $this->atkNode("lead",NF_ADD_LINK);
    $this->add(new atkAttribute("id" ,AF_AUTOKEY));
    
    $this->add(new atkManyToOneRelation("lead","crm.lead_source",AF_HIDE_LIST));
    $this->add(new atkTextAttribute("lead_description",0,AF_HIDE_LIST));
    $this->add(new atkManyToOneRelation("status","crm.lead_status",AF_HIDE_LIST));
    $this->add(new atkTextAttribute("status_description",0,AF_HIDE_LIST));
    $this->add(new atkManyToOneRelation("salutation","crm.salutation",AF_HIDE_LIST));
    $this->add(new atkAttribute("lastname",AF_SEARCHABLE|AF_OBLIGATORY|AF_HIDE_LIST, 50));
    $this->add(new atkAttribute("firstname",AF_FORCE_LOAD|AF_HIDE_LIST, 50));
    $this->add(new atkAggregatedColumn("name","[firstname] [lastname]",AF_SEARCHABLE,array("firstname","lastname")));  
    $this->add(new atkListAttribute("gender",array("male","female","unknown"),array("m","f","u"),AF_HIDE_LIST));
    $this->add(new atkAttribute("title", 20));
    //$this->add(new atkManyToOneRelation("account","crm.account",AF_LARGE));
    $this->add(new atkAttribute("company",AF_SEARCHABLE));
    $this->add(new atkAttribute("division",AF_HIDE_LIST));    
    $this->add(new atkDateAttribute("birthday","","",0,0,AF_HIDE_LIST));
    $this->add(new atkAttribute("address",AF_HIDE_LIST, 100));
    $this->add(new atkAttribute("zipcode",AF_HIDE_LIST, 20));
    $this->add(new atkAttribute("city",AF_HIDE_LIST, 100));
    $this->add(new atkAttribute("state",AF_HIDE_LIST, 100));
    $this->add(new atkAttribute("country",AF_HIDE_LIST, 100));
    $this->add(new atkAttribute("workphone", AF_SEARCHABLE, 20));
    $this->add(new atkAttribute("cellphone", AF_HIDE_LIST|AF_FORCE_LOAD, 20));
    $this->add(new atkAttribute("homephone", AF_HIDE_LIST|AF_FORCE_LOAD, 20));
    $this->add(new atkAttribute("otherphone", AF_HIDE_LIST|AF_FORCE_LOAD, 20));
    $this->add(new atkAttribute("fax", AF_HIDE_LIST|AF_FORCE_LOAD, 20));
    $this->add(new atkEmailAttribute("email", false, AF_SEARCHABLE, 50));
    $this->add(new atkEmailAttribute("otheremail", false, AF_HIDE_LIST, 50));
    $this->add(new atkAttribute("website",AF_SEARCHABLE, 100));
    $this->add(new atkManyToOneRelation("assigned_to","employee.employee",AF_SEARCHABLE));
    $this->add(new atkTextAttribute("remark", TEXT_LARGE, AF_HIDE_LIST));
    $this->add(new atkManyToOneRelation("created_by","employee.employee",AF_HIDE_LIST|AF_READONLY),"history");
    $this->add(new atkDateTimeAttribute("created_on","","",AF_HIDE_LIST|AF_READONLY),"history");
    $this->add(new lastModifiedByAttribute("last_modified_by","employee.employee"),"history");
    $this->add(new atkUpdateStampAttribute("last_modified_on",AF_HIDE_LIST|AF_READONLY),"history");
    
    $this->setSecurityAlias("crm.account");
    $this->setOrder("lastname");
    $this->setTable("crm_lead");
  }
  
  function descriptor_def()
  {
    return "[lastname]";
  }
   
  function recordActions($rec, &$actions, &$mraactions)
  {
    $actions["convert"] = $_SERVER["PHP_SELF"].'?atknodetype='.$this->atknodetype()."&atkselector=[pk]&atkaction=convert";
  }
  
  function website_display($record)
  {
    $website = $record["website"];
    if (strlen($website)>0) return '<a href="http://'.$website.'" target="_blank">'.$website.'</a>';
    return "";
  }
  
  function website_edit($record, $mode)
  {
     $attr = &$this->getAttribute("website");
     $org = $attr->edit($record, $mode);
     return "http://".$org;
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
  
  function action_convert()
  {
    // Convert lead to contact/company
  }
}

?>