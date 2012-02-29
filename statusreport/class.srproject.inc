<?php

atkimport('module.statusreport.utils.statusreportutil');
userelation("atkonetomanyrelation");

class srproject extends project
{
  public function srproject()
  {
    $this->project("project");
    $sr = &getNode("statusreport.statusreport");
    if($sr->allowed("pm") || $sr->allowed("mt"))
    {
      $this->add(new atkOneToManyRelation("statusreport","statusreport.statusreport","project_id",AF_HIDE_LIST|AF_HIDE_ADD),"status_report");
    }
    
    $this->addAllowedAction("status_update");
    $this->addAllowedAction("planning");
    $this->addAllowedAction("view_last_statusreport");
  }
  
  public function rowColor($rec)
  {
//    $res = "";
//    global $ATK_VARS;
//    if($ATK_VARS['atkaction'] == "pim")
//    {
      $sr = &getNode("statusreport.statusreport");
      list($rec) = $sr->selectDb("statusreport.project_id='".$rec['id']."'","statusreport.id DESC",1,"",array("pm_traffic_light","achievo_traffic_light"));
      $res = $sr->m_traffic_color[$sr->m_traffic_light[$rec['pm_traffic_light']][$rec['achievo_traffic_light']]];
//    }
    return $res;
  }
  
  public function adminFooter()
  {
    $flagged = $this->getView("flagged");
    $res = parent::adminFooter();

    if ($flagged=="flagged")
    {
      $res .= atktext("show_all_flagged_project","statusreport")." ".href(dispatch_url($this->atknodetype(),$this->m_action,array("flagged"=>"nonflagged")),
                                                    atktext('clicktoviewall',"statusreport"))."<br>";
    }
    elseif($flagged=="nonflagged")
    {
      $res .= atktext("show_all_project","statusreport")." ".href(dispatch_url($this->atknodetype(),$this->m_action,array("flagged"=>"flagged")),
                                                    atktext('clicktoviewflagged',"statusreport"))."<br>";
    }
    return $res;
  }
  
  public function getView($varname)
  {
    if($varname <> "flagged") return parent::getView($varname);
    
    $sessionManager = &atkGetSessionManager();
    
    /**
     * "flagged" must be present in stack. If not - we don't come from PIM page,
     * so we haven't show "flagged porject" link.
     */
    $value = $sessionManager->stackVar($varname);
    
    return $value;
  }

  public function action_admin(&$handler, $record=null)
  {
    $flagged = $this->getView("flagged");
    
    if ($flagged == "flagged")
    {
      $filter = "1=0";
      $ids = statusreportutil::getFlaggedProjectid();

      if(!empty($ids))
      {
        $filter = "project.id IN(".implode(",",$ids).")";
      }
      $this->addFilter($filter);
    }
    return parent::action_admin($handler, $record);
  }
  
}