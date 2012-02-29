<?php

  useattrib("utils.fuzzysearchattribute");
  useattrib("atkdateattribute");
  useattrib("atkparserattribute");
  useattrib("atkdummyattribute");
  useattrib("atktextattribute");
  useattrib("atknumberattribute");
  userelation("atkmanytoonerelation");
  userelation("atkonetomanyrelation");
  useattrib("graph.datagraphattribute");
  useattrib("atklistattribute");
  
  useattrib("hourdeposit.depositcount");
  
  include_once("achievotools.inc"); // used for time_format function.

  class deposit extends atkNode
  {
    var $m_showStatus; 
    var $m_archive;
  
    function deposit()
    {
      $this->atkNode("deposit", NF_ADD_LINK|NF_EDITAFTERADD);
      
      $this->add(new atkAttribute("id", AF_AUTOKEY));
      $this->add(new atkAttribute("name", AF_SEARCHABLE|AF_OBLIGATORY));
      $this->add(new atkTextAttribute("description", AF_SEARCHABLE));
      $this->add(new atkManyToOneRelation("customer", "organization.organization", AF_RELATION_AUTOLINK));
      $this->add(new atkNumberAttribute("hours"));   
      $this->add(new atkDateAttribute("start_date"));
      $this->add(new atkDateAttribute("end_date"));
      $this->add(new atkListAttribute("status", array("active", "archived"), "", AF_HIDE_ADD|AF_HIDE_LIST));
      $this->add(new atkOneToManyRelation("projects", "hourdeposit.deposit_project", "hourdeposit_deposit_id", AF_HIDE_LIST|AF_READONLY));            
      $this->add(new atkDummyAttribute("projectaddhint", text("fuzzyprojectaddhint", "deposit", "hourdeposit"), AF_HIDE_LIST|AF_HIDE_SEARCH|AF_HIDE_VIEW));
      $this->add(new fuzzySearchAttribute("projectadd", "project.project", "addProjects", FSA_MULTISELECT));          
      
      $this->setOrder("name");
      $this->setTable("hourdeposit_deposit");
    }
    
    function action_admin()
    {          
      global $g_sessionManager;
      $this->m_showStatus = $g_sessionManager->pageVar("showstatus");
      $this->m_archive = $g_sessionManager->pageVar("archive");
      if ($this->m_showStatus==1)
      {
        $this->addStatusFields();
      }
      
      if ($this->m_archive==1)
      {
        $this->addFilter("hourdeposit_deposit.status='archived'");        
      }
      else
      {
        $this->addFilter("hourdeposit_deposit.status='active'");
      }
      
      return parent::action_admin();
    }
    
    function initial_values()
    {
      return array("status"=>"active",
                   "start_date"=>array("year"=>date("Y"), "month"=>date("m"), "day"=>date("d")),
                   "end_date"=>array("year"=>date("Y")+1, "month"=>date("m"), "day"=>date("d")));
    }
    
    function hours_edit($record, $mode)
    {
      return $this->m_attribList["hours"]->edit($record, $mode)." ".text("hours");      
    }
    
    // In view and edit mode, we always show status.
    function action_view()
    {
      $this->addStatusFields();
      return parent::action_view();
    }
    
    function action_edit()
    {
      $this->addStatusFields();
      return parent::action_edit();
    }
    
    function action_update()
    {
      $this->addStatusFields();
      return parent::action_update();
    }
    
    function addStatusFields()
    {
      $this->add(new depositCount("usedtime"));
      $this->add(new atkParserAttribute("hoursleft", "", AF_READONLY));
      
      $this->add(new dataGraphAttribute("history", "history", array("deposit"=>"[id]",
                                                                    "hours"=>"[hours]"), "line", AF_HIDE_ADD|AF_HIDE_LIST));
    }
    
    function adminFooter()
    {
      $res = "" ;
      if (!$this->m_showStatus)
      {
        $res .= href(dispatch_url($this->atknodetype(), $this->m_action, array("showstatus"=>1)),
                    text("showstatus", "deposit", "hourdeposit"))."<br><br>";
      }      
      if ($this->m_archive!=1)
      {
        $res .= href(dispatch_url($this->atknodetype(), $this->m_action, array("archive"=>1)),
                    text("showarchive", "deposit", "hourdeposit"));
      }
      else
      {
        $res .= href(dispatch_url($this->atknodetype(), $this->m_action, array("archive"=>0)),
                    text("showactive", "deposit", "hourdeposit"));
      }
      return $res."<br>";
    }
    
    function addProjects($rec, $projects)
    { 
      global $g_db;
      
      // Add the first found match as project.
      for ($i=0, $_i=count($projects); $i<$_i; $i++)
      {
        // Only add projects that are not already linked to another deposit.
        $g_db->query("UPDATE project SET hourdeposit_deposit_id = ".$rec["id"]." 
                       WHERE 
                         project.id = '".$projects[$i]["id"]."' 
                         AND 
                           (hourdeposit_deposit_id IS NULL OR hourdeposit_deposit_id = '')");
      }         
    }
    
    function descriptor_def()
    {
      return "[name]";
    }
    
    /** 
     * As a workaround for ATK bug #1780, we have to implement our own countDb method.
     */
    function countDb()
    {
      global $g_db;
      $cnt = $g_db->getrows("SELECT count(*) as cnt FROM hourdeposit_deposit");
      return $cnt[0]["cnt"];
    }
    
    function rowColor($record, $num)
    {
      if ($this->m_showStatus)
      {
        $depletionwarning = atkconfig("hourdeposit_depletionwarning", 10);        
      
        $usedtime = $record["usedtime"];
        $deposittime = $record["hours"]*60;
        if ($deposittime<$usedtime) return "#ff0000";
        $minutesleft = $deposittime-$usedtime;
        if ($minutesleft <= (60*$depletionwarning)) return "#ff9900"; // orange if 10 hours or less left
      }
              
    }
          
    function hours_display($record, $mode)
    {      
      return time_format($record["hours"]*60, true);
    }  
    
    //function hoursleft_edit($record, $mode)
    //{
//      return $this->hoursleft_display
    //}
    
    function hoursleft_display($record, $mode)
    {
      $saldo = $record["hours"]*60;
      $used  = $record["usedtime"];
      $left = $saldo-$used;
      $result = time_format($left, true);
      if (($saldo-$used)<=0)
      {
        $result = '<b>'.$result.'</b>';
      }
      return $result." (".sprintf("%.01f",$left/60/8)."d)";
    }
    
    function usedtime_display($record, $mode)
    {
      return $this->m_attribList["usedtime"]->display($record, $mode)." (".sprintf("%.01f",$record["usedtime"]/60/8)."d)";
    }
    
    function graph_history($params)
    {
      if ($params["deposit"]!="")
      {
        $result = array();                        
        
        global $g_db;
        $recs = $g_db->getrows("SELECT 
                                  DATE_FORMAT(hours.activitydate,'%Y%V') as stamp, 
                                  SUM(hours.time) as total
                                FROM
                                  hours, phase, project, hourdeposit_deposit
                                WHERE
                                  hours.phaseid = phase.id
                                  and hourdeposit_deposit.id = project.hourdeposit_deposit_id
                                  and project.id = phase.projectid
                                  and hourdeposit_deposit.id = '".$params["deposit"]."'
                                  and hours.activitydate BETWEEN hourdeposit_deposit.start_date AND hourdeposit_deposit.end_date
                                GROUP BY
                                  hourdeposit_deposit.id,
                                  DATE_FORMAT(hours.activitydate,'%Y%V')
                                ORDER BY 
                                  DATE_FORMAT(hours.activitydate,'%Y%V')");
        $totalused = 0;
        if (count($recs)==0)
        {
          // Add one dummy datapoint for the plot with the full deposit.
          $result[date("YW")]=$params["hours"];
          $result[date("YW")-1]=$params["hours"];
        }
        else
        {
          for ($i=0, $_i=count($recs); $i<$_i; $i++)
          {
            if ($i==0)
            {
              // Add initial stamp with full deposit
              $result[($recs[$i]['stamp']-1)] = $params["hours"];
            }
            $totalused+=$recs[$i]["total"];
            $result[$recs[$i]['stamp']] = ((($params["hours"]*60)-$totalused)/60);
          }         
        }
                          
        return array("historyperweek"=>$result);
      }
    }
        
  }

?>
