<?PHP
useattrib("atkTextAttribute");
useattrib("atkDateTimeAttribute");
useattrib("atkEmailAttribute");
useattrib("atkBoolAttribute");
useattrib("atkDurationAttribute");
useattrib("atkUpdateStampAttribute");
userelation("atkmanytoonerelation");
useattrib("crm.lastModifiedByAttribute");

class activity extends atkNode 
{
  
  function activity()
  {
    $this->atkNode("activity",NF_MRA|NF_ADD_LINK);

    $this->add(new atkAttribute("activity_id",AF_AUTOKEY));
    $this->add(new atkAttribute("parent_id",AF_HIDE));
    $this->add(new atkAttribute("parent_node",AF_HIDE));
    $this->add(new atkManyToOneRelation("type","crm.activity_type",AF_OBLIGATORY));
    $this->add(new atkAttribute("title",AF_OBLIGATORY));
    
    $this->add(new atkManyToOneRelation("account","crm.account",AF_OBLIGATORY|AF_LARGE));
    
    $preFilter = new atkManyToOneRelation("contact","crm.contact",AF_RELATION_AUTOLINK);
    $preFilter->m_destinationFilter = "account = '[account.id]'";
    $this->add($preFilter);

    $this->add(new atkDateTimeAttribute("start_date"));
    $this->add(new atkDateAttribute("end_date"));
    $this->add(new atkDurationAttribute("duration",AF_HIDE_LIST));
    
    $this->add(new atkManyToOneRelation("status","crm.activity_status",AF_OBLIGATORY|AF_HIDE_LIST));
    $this->add(new atkManyToOneRelation("assigned_to","employee.employee"));
    
    $this->add(new atkTextAttribute("description",AF_HIDE_LIST));
    $this->add(new atkBoolAttribute("completed",AF_HIDE_LIST));
    
    $this->add(new atkManyToOneRelation("created_by","employee.employee",AF_HIDE_LIST|AF_READONLY),"history");
    $this->add(new atkDateTimeAttribute("created_on","","",AF_HIDE_LIST|AF_READONLY),"history");
    $this->add(new lastModifiedByAttribute("last_modified_by","employee.employee"),"history");
    $this->add(new atkUpdateStampAttribute("last_modified_on",AF_HIDE_LIST|AF_READONLY),"history");
    
    $this->setSecurityAlias("crm.account");
    $this->setOrder("title");
    $this->setTable("crm_activity");
  }
  
  function descriptor_def()
  {
    return "[title]";
  }
  
  function initial_values()
  {
    global $ATK_VARS;
    return array("created_by"=>$user["id"],
                 "last_modified_by"=>$user["id"],
                 "created_on"=>array("year"=>date("Y"),
                                     "month"=>date("m"),
                                     "day"=>date("d"),
                                     "hours"=>date("H"),
                                     "minutes"=>date("i"),
                                     "seconds"=>date("s")));
  }
}

?>