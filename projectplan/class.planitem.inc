<?php

  userelation("atkmanytoonerelation");
  useattrib("atknumberattribute");
  useattrib("atkdurationattribute");

  class planitem extends atkNode 
  {
    function planitem()
    {
      $this->atkNode("planitem");
      
      $this->add(new atkManyToOneRelation("userid", "employee.employee", AF_PRIMARY));
      $this->add(new atkManyToOneRelation("phaseid", "project.phase", AF_PRIMARY));
      $this->add(new atkAttribute("month", AF_PRIMARY, 6));
      $this->add(new atkDurationAttribute("planned"));
      
      $this->_setRights();      
      
      $this->setTable("projectplan_planitem");
    }
    
    function _setRights()
    {
      // A planitem may be added, updated or deleted if
      // the user has the 'edit' right on the projectplan.
      $this->setSecurityAlias("projectplan.projectplan");
      $this->m_securityMap["add"] = "edit";      
      $this->m_securityMap["delete"] = "edit";
      $this->m_securityMap["save"] = "edit";
      $this->m_securityMap["update"] = "edit";
    }
    
    function storeItems($items)
    {      
      foreach ($items as $phaseid=>$users)
      {
        foreach ($users as $userid=>$months)
        {
          foreach ($months as $month=>$planning)
          {
            if ($phaseid!=""&&$userid!=""&&$month!="")
            {            
              // first delete existing rec.
              $this->deleteDb("projectplan_planitem.phaseid=$phaseid and projectplan_planitem.userid=$userid and projectplan_planitem.month='$month'");               
              if (strlen($planning)>0)
              {                
                $record = array("phaseid"=>$phaseid, "userid"=>$userid, "month"=>$month);                                
                $record["planned"] = $this->m_attribList["planned"]->_string2minutes($planning);
                $record["phaseid"] = $this->m_attribList["phaseid"]->db2value($record);
                $record["userid"] = $this->m_attribList["userid"]->db2value($record);                
                              
                $this->addDb($record);
                
              }
            }
          }
        }
      }
    }
  }

?>