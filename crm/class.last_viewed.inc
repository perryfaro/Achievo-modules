<?PHP

class last_viewed extends atkNode
{
  function last_viewed()
  {
    
    $this->atkNode("last_viewed",NF_MRA|NF_ADD_LINK);
    
    $this->setSecurityAlias("crm.account");
    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("atknodetype"));
    $this->add(new atkAttribute("atkaction"));
    $this->add(new atkDateTimeAttribute("inserted"));
        
    $this->setOrder("inserted");
    $this->setTable("crm_last_viewed");
  }
}

?>