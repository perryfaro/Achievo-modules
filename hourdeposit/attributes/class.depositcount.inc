<?php
  
  class depositCount extends atkAttribute
  {
    function depositCount($name, $flags=0)
    {
      $this->atkAttribute($name, $flags|AF_READONLY|AF_HIDE_ADD);
    }
    
    function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level, $mode)
    {
      if ($mode!="add"&&$mode!="update")
      {
        $query->addField("sum(hours.time)");
        $query->addJoin("project","project","project.hourdeposit_deposit_id = hourdeposit_deposit.id",true);
        $query->addJoin("phase","phase","phase.projectid = project.id",true);
        $query->addJoin("hours","hours","hours.phaseid = phase.id 
                                         AND hours.activitydate BETWEEN hourdeposit_deposit.start_date and hourdeposit_deposit.end_date",true);
        
        $query->addGroupBy("hourdeposit_deposit.id");
      }
    }
    
    function db2value($record)
    {
      return $record["sum(hours.time)"];
    }
    
    function display($record, $mode)
    {
      include_once("achievotools.inc");
      return time_format($record[$this->fieldName()], true);
    }
    
  }

?>