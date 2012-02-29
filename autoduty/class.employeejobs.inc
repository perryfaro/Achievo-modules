<?php

userelation("atkmanytoonerelation");
useattrib("atknumberattribute");

class employeejobs extends atkNode
{
	function employeejobs()
	{
		$this->atkNode("employeejobs");

		$this->add(new atkManyToOneRelation("job_id","autoduty.job",AF_PRIMARY|AF_HIDE));
		$this->add(new atkManyToOneRelation("userid","employee.employee",AF_PRIMARY));
    $this->add(new atkNumberAttribute("jobcount"));

		$this->setTable("autoduty_employeejobs");
		$this->setSecurityAlias("autoduty.job");
	}

  /** 
   * We override the edit function for the jobcount field, so we can
   * automatically determine the initial value of the jobcount field
   * (the minimum of the jobcounts of the other people assigned to this
   * task).
   * Note: I couldn't use initial_values for this, because I need the job_id 
   * field, and I don't have that in initial_values(). In edit() I do have it, 
   * because it's set when clicking the 'add' button.
   */
	function jobcount_edit($rec, $prefix, $mode)
  {
    if ($mode=="add")
    {      
      $rec["jobcount"] = $this->getmaxjobcount($rec["job_id"]["id"]);
    }
    return $this->m_attribList["jobcount"]->edit($rec, $prefix, $mode);
  }
  
  function getmaxjobcount($job_id)
  {
    $db = &atkGetDb();
		
    $query = "SELECT min(jobcount) as mincount 
              FROM 
                autoduty_employeejobs,
                usercontract
             WHERE 
                autoduty_employeejobs.userid=usercontract.userid
                AND (".$db->func_now()." BETWEEN usercontract.startdate AND usercontract.enddate
                     OR (usercontract.startdate<=".$db->func_now()." AND usercontract.enddate IS NULL))
                AND job_id = ".$job_id;
        
    $recs = $db->getrows($query);
    
    if (count($recs))
    {
      return $recs[0]['mincount'];
    }
    return 0;
  }
  
}

?>