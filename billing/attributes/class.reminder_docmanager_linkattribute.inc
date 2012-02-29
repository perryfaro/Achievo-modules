<?php

  useattrib("atkdummyattribute");

  class reminder_docmanager_linkattribute extends atkDummyAttribute 
  {
    function reminder_docmanager_linkattribute($name,$flags=0)
    {
      $this->atkDummyAttribute($name,'',$flags);
    }
    
    function display($record,$mode="")
    {
      // get project belonging to our parent node
      $bill_id = $record['bill_id']['id'];
      
      if (!$bill_id || !is_numeric($bill_id)) return '';
      
      $billNode = &getNode("billing.bill");
      $result = array_pop($billNode->selectDb($billNode->m_table . ".id = '$bill_id'","","","",array("project_id")));
      $projectid = $result['project_id']['projectid']['id'];
      
      if (!$projectid || !is_numeric($projectid)) return '';

      $url = session_url(dispatch_url('project.project','edit',array("atkselector"=>"project.id=$projectid","atktab"=>"documents")),SESSION_NESTED);
      return "<a href='$url'>".atktext('billing_reminder_document_in_projects_docmanager')."</a>";
    }
  }

?>
