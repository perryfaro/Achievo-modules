<?php

function pimBug()
{
    $userid = atkGetUserId();
    $rl = &atknew("atk.recordlist.atkrecordlist");

    $qnode = &atkGetNode("achilles.bug");
    $qnode->m_flags |= NF_NO_SEARCH;
    $qnode->m_action="admin";

    $actions = array();
    //$actions["edit"] = dispatch_url("achilles.bug","edit",array("atkselector"=>"[pk]"));
    $actions["view"] = dispatch_url("achilles.bug","view",array("atkselector"=>"[pk]"));

    //03/04/08 filter in pim for ticket fixed Id= 5
	//$where = "achilles_bug.assigned_to='".$userid."'";
	$where = "achilles_bug.assigned_to='".$userid."' and achilles_bug.status_id NOT IN('5','7')";

    $recs = $qnode->selectDb($where,"","","",array("bug_id","title","severity_id","priority","status_id","completed","resolution_id","qa_contact","project_id","milestone_id"));
    
    $ret = atktext("pim_tickets_assigned_to_you").":<br>";
    if (count($recs)>0)
    {
      $ret.= $rl->render($qnode,$recs,$actions,RL_NO_EXTENDED_SEARCH|RL_NO_SEARCH|RL_NO_SORT, array("assigned_to"),"userleads",array(),"userleads");
      //$ret.= $rl->render($qnode,$recs,$actions,RL_NO_EXTENDED_SEARCH|RL_NO_SEARCH, array("assigned_to"),"userleads",array(),"userleads");

    }
    else
    {
      $ret.= atktext("pim_no_tickets_assigned_to_you")."<br>";
    }

    $where = "achilles_bug.created_by='".$userid."' and achilles_bug.status_id NOT IN('5')";

    $recs = $qnode->selectDb($where,"","","",array("bug_id","title","severity_id","priority","status_id","completed","resolution_id","qa_contact","project_id","milestone_id"));
    $ret .= "<br><br><br>". atktext("pim_tickets_created_by_you").":<br>";
    if (count($recs)>0)
    {
      $ret.= $rl->render($qnode,$recs,$actions,RL_NO_EXTENDED_SEARCH|RL_NO_SEARCH|RL_NO_SORT, array("assigned_to"),"userleads",array(),"userleads");
      //$ret.= $rl->render($qnode,$recs,$actions,RL_NO_EXTENDED_SEARCH|RL_NO_SEARCH, array("assigned_to"),"userleads",array(),"userleads");

    }
    else
    {
      $ret.= atktext("pim_no_tickets_created_by_you")."<br>";
    }

    return $ret;

}
?>