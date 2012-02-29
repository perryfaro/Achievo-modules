<?PHP
userelation("atkmanytoonerelation");
class account_relation extends atkNode 
{
  function account_relation()
  {
    $this->atkNode("account_relation");

    $this->add(new atkAttribute("relation_id",AF_AUTOKEY));
    $this->add(new atkManyToOneRelation("account","crm.account",AF_HIDE));
    
    $this->add(new atkManyToOneRelation("relation_with","crm.account",AF_LARGE|AF_OBLIGATORY));
    $this->add(new atkManyToOneRelation("kind_of","crm.relation_type",AF_OBLIGATORY));

    $this->setSecurityAlias("crm.account");
    $this->setTable("crm_account_relation", "crm_account_relation");

  }
  
  
  
}

?>