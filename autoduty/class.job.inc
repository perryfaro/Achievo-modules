<?php

userelation("atkonetomanyrelation");
useattrib("atklistattribute");
useattrib("atknumberattribute");
useattrib("atktextattribute");
useattrib("atkmultiselectattribute");
//useattrib("utils.fuzzysearchattribute");
useattrib("atkfuzzysearchattribute");

class job extends atkNode
{
  function job()
  {
    $this->atkNode("job",NF_EDITAFTERADD);
    
    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("name",AF_UNIQUE|AF_OBLIGATORY|AF_SEARCHABLE));
    $this->add(new atkTextAttribute("description", AF_HIDE_ADD)); 
    $this->add(new atkMultiSelectAttribute("days",array("mon", "tue","wed","thu","fri","sat","sun"),array(1,2,3,4,5,6,0),AF_HIDE_LIST|AF_HIDE_ADD));
    
    $this->add(new atkListAttribute("schedulefreq",array("daily","weekly"),"",AF_HIDE_LIST|AF_HIDE_ADD));
    $this->add(new atkNumberAttribute("required",AF_HIDE_LIST|AF_HIDE_ADD)); 
    $this->add(new atkNumberAttribute("optimal",AF_HIDE_LIST|AF_HIDE_ADD)); 
    $this->add(new atkListAttribute("scheduletype",array("structured", "random"), array("s", "r"), AF_HIDE_LIST|AF_HIDE_ADD));
    
    $this->add(new atkOneToManyRelation("participants","autoduty.employeejobs","job_id",AF_HIDE_LIST|AF_CASCADE_DELETE));
    
    //$this->add(new fuzzySearchAttribute("participant_add", "employee.employee", "addparticipants", FSA_MULTISELECT, AF_HIDE_ADD|AF_HIDE_LIST));
    $this->add(new atkFuzzySearchAttribute("participant_add", "employee.employee", "addparticipants", FSA_MULTISELECT, AF_HIDE_ADD|AF_HIDE_LIST));
    
    $this->setTable("autoduty_job");
    $this->setIndex("name");
    $this->setOrder("name");
  }
  
  function optimal_validate(&$record)
  {
    if ($record["optimal"]<$record["required"])
    {
      triggerError($record, "optimal", "err_job_optimaltosmall");
    }
  }

  function initial_values()
  {
    return array("days"=>"1|2|3|4|5",
     "requires"=>1);
  }

  function descriptor_def()
  {
    return "[name]";
  }       
  
  function addparticipants($record, $matches)
  {    
    $db = &atkGetDb();
    
    $empjobnode = &getNode("autoduty.employeejobs");
    $job = $record["id"];
    $default = $empjobnode->getmaxjobcount($job); 
    if ($job!="")
    {
      for ($i=0, $_i=count($matches); $i<$_i; $i++)
      {
        $emp = $matches[$i]['id'];      
        if ($emp!="")
        {
          $db->query("INSERT INTO autoduty_employeejobs (job_id, userid, jobcount) VALUES ('$job', '$emp', '$default')");
        }    
      }
    }
  }
    
}
?>
