<?php

useattrib("atkdateattribute");
userelation("atkmanytoonerelation");

class joblog extends atkNode
{
	function joblog()
	{
		$this->atkNode("joblog", NF_NO_ADD|NF_TRACK_CHANGES|NF_MRA);
		$this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkDateAttribute("jobdate", "", "", 0, 0, AF_SEARCHABLE|AF_READONLY));
		$this->add(new atkManyToOneRelation("userid", "employee.employee", AF_SEARCHABLE));
    $this->add(new atkManyToOneRelation("job_id", "autoduty.job", AF_SEARCHABLE|AF_READONLY));
    $this->add(new atkAttribute("appliedfactor", AF_HIDE_LIST|AF_READONLY));
		
		$this->setOrder("jobdate DESC, job_id");
		$this->setTable("autoduty_joblog");

	}
  
  function postDel($rec)
  {
    // If a record has been deleted, then that means that the job was not
    // done that day. We must decrease the jobcounter for the affected 
    // job and person.    
    $this->decrease($rec["userid"]["id"], $rec["job_id"]["id"], $rec["appliedfactor"]);
  }
  
  function postUpdate(&$rec)
  {
    if ($rec["atkorgrec"]["userid"]["id"]!=$rec["userid"]["id"])
    {
      atkdebug("userid changed, performing magic..");
      
      // We decrease the log of the original person, by the same amount we
      // originally added (appliedfactor).
      $orgfactor = $rec["atkorgrec"]["appliedfactor"];
      $this->decrease($rec["atkorgrec"]["userid"]["id"], 
                      $rec["atkorgrec"]["job_id"]["id"], 
                      $orgfactor);
                      
      // Then, we increase the log of the new person, calculating his
      // contractfactor on the said day. 
      $newfactor = $this->getContractFactor($rec["userid"]["id"], $rec["jobdate"]);      
      $this->increase($rec["userid"]["id"], 
                      $rec["job_id"]["id"],
                      $newfactor);
                      
      if ($orgfactor!=$newfactor)
      {
        // The factor of the new person is not the same as the old person.
        // so we must update the joblog record, so it reflects the new 
        // factor.        
        $db = &atkGetDb();
        $db->query("UPDATE autoduty_joblog SET appliedfactor = $newfactor 
                       WHERE id=".$rec["id"]);
                       
        $rec["appliedfactor"] = $newfactor; // update $rec variable so editform is updated.
      }
    }
  }
  
  function decrease($userid, $job_id, $amount=1)
  {
		$db = &atkGetDb();
    $db->query("UPDATE autoduty_employeejobs SET jobcount=jobcount-$amount WHERE job_id = ".$job_id." AND userid ='".$userid."'");
  }
  
  function increase($userid, $job_id, $amount=1)
  {
    $db = &atkGetDb();
    $db->query("UPDATE autoduty_employeejobs SET jobcount=jobcount+$amount WHERE job_id = ".$job_id." AND userid ='".$userid."'");
  }
  
  function getContractFactor($userid, $date)
  {
    $db = atkGetDb();
    $strdate = sprintf("%04d-%02d-%02d", $date["year"], $date["month"], $date["day"]);
    $recs = $db->getrows("SELECT uc_hours FROM usercontract WHERE userid='$userid' AND ('$strdate' BETWEEN startdate AND enddate OR ('$strdate'>=startdate AND enddate IS NULL))");
    if (count($recs))
    {
      return sprintf("%.02f", 40/($recs[0]["uc_hours"]));
    }
    return 1; // default factor
  }
  
  /* TODO: make nice per-week view, for reviewing previous weeks schedules. 
  function adminHeader()
  {
    global $g_sessionManager;
    $viewdate = $g_sessionManager->pageVar("viewdate");
    if ($viewdate=="")
    {
      $viewdate = date("Y-m-d");
      $viewtime = time();
    }
    else
    {
      $viewtime = mktime(12,12,0,substr($viewdate,5,2),substr($viewdate,8,2),substr($viewdate,0,4));
    }
    
    // determine weekdays of this week.
    $weekday = strftime("%w",$viewtime);
    if ($weekday==0) $weekday=7;

    for ($i=0; $i<=8;$i++)
    {
      $weekdates[$i]['date']=date("Y-m-d" ,$viewtime-(86400*($weekday-$i)));      
    }
    
    $prevnextlinks = '<br>'.href('dispatch.php?weekview=1&viewdate='.$weekdates[0]['date'], text('houradmin_previousweek'));
    if ($weekdates[8]['date'] <= date("Y-m-d"))
    {
      $prevnextlinks.='&nbsp;&nbsp;'.href('dispatch.php?weekview=1&viewdate='.$weekdates[8]['date'],text('houradmin_nextweek'));
    }
    
    $this->addFilter("jobdate BETWEEN ".$weekdates[1]['date']." AND ".$weekdates[7]['date']);

    return $prevnextlinks;    
  } */
}
?>
