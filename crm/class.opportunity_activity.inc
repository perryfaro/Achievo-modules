<?PHP
useattrib("atkTextAttribute");
useattrib("atkDateTimeAttribute");
useattrib("atkEmailAttribute");
useattrib("atkBoolAttribute");
useattrib("atkUpdateStampAttribute");
userelation("atkmanytoonerelation");
useattrib("crm.lastModifiedByAttribute");

class opportunity_activity extends atkNode 
{
  
  function opportunity_activity()
  {
    
    $this->atkNode("opportunity_activity",NF_MRA|NF_ADD_LINK);
    atk_var_dump($this->m_owner,"OWNER");
    $this->add(new atkAttribute("activity_id",AF_AUTOKEY));
    $this->add(new atkAttribute("parent_id",AF_HIDE));
    $this->add(new atkAttribute("parent_node",AF_HIDE));
    
    $this->add(new atkManyToOneRelation("account","crm.account",AF_OBLIGATORY|AF_LARGE));
    $preFilter = new atkManyToOneRelation("account","crm.account",AF_RELATION_AUTOLINK);
    $preFilter->m_destinationFilter = "account = '[account.id]'";
    $this->add($preFilter);
    
    $this->add(new atkManyToOneRelation("assigned_to","employee.employee"));
    
    $this->add(new atkManyToOneRelation("status","crm.opportunity_activity_status",AF_OBLIGATORY));
    
    $this->add(new atkManyToOneRelation("type","crm.activity_type",AF_OBLIGATORY));
    
    $this->add(new atkAttribute("probability"));
    $this->add(new atkAttribute("title"));
    $this->add(new atkTextAttribute("activity_notes"));
    $this->add(new atkTextAttribute("opportunity_notes"));
    $this->add(new atkDateTimeAttribute("start_date"));
    $this->add(new atkDateTimeAttribute("end_date"));
    $this->add(new atkEmailAttribute("mail_to"));
    $this->add(new atkBoolAttribute("completed"));
    
    $this->add(new atkManyToOneRelation("created_by","employee.employee",AF_HIDE_LIST|AF_READONLY),"history");
    $this->add(new atkDateTimeAttribute("created_on","","",AF_HIDE_LIST|AF_READONLY),"history");
    $this->add(new lastModifiedByAttribute("last_modified_by","employee.employee"),"history");
    $this->add(new atkUpdateStampAttribute("last_modified_on",AF_HIDE_LIST|AF_READONLY),"history");
    
    $this->setSecurityAlias("crm.account");
    $this->setOrder("title");
    $this->setTable("crm_opportunity_activity");
  }
  
  function descriptor_def()
  {
    return "[title]";
  }
  function initial_values()
  {
    global $ATK_VARS;
    return array("parent_node"=>$ATK_VARS["atksource"]);
  }
}

?>