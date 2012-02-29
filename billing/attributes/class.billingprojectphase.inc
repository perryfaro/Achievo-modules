<?php
useattrib("project.projectphaseattrib");

class billingprojectphase extends atkProjectPhaseAttribute 
{
  function billingprojectphase($name, $flags=0)
  {
    parent::atkProjectPhaseAttribute($name, $flags);
  }
  
  function addToQuery(&$query, $tablename="", $fieldaliasprefix="", $rec="", $level=0, $mode="")
  {
    return atkAttribute::addToQuery($query,$tablename,$fieldaliasprefix,$rec,$level,$mode);
  } 
  
  function db2value($rec)
  {
    $phasenode = &getNode("project.phase");
    $excludelist = $this->_getExcludeListForNode($phasenode, array("projectid","name"));
    $phasedata = $phasenode->selectDb("phase.id = '{$rec["phaseid"]}'",null,null,$excludelist);
    return Array("projname"=>$phasedata[0]["projectid"]["name"],
                 "phaseid"=>$rec["phaseid"],
                 "phasename"=>$phasedata[0]["name"]);
  }  
  
  function _getExcludeListForNode($node, $include)
  {
    $excludelist = array_keys($node->m_attribList);
    foreach ($excludelist as $key=>$value)
    {
      if (in_array($value, $include)) unset($excludelist[$key]);
    }
    sort($excludelist);
    return $excludelist;
  }
}

?>