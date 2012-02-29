<?php

useattrib("atkTextAttribute");
useattrib("atkDateAttribute");
useattrib("atkDateTimeAttribute");
useattrib("atkBoolAttribute");
useattrib("atkRadioAttribute");
useattrib("atkUpdateStampAttribute");
userelation("atkmanytoonerelation");
useattrib("pinboard.updatedByAttribute");

class pinboard extends atkNode 
{
  var $m_archive = "";

  function pinboard()
  {
    $this->atkNode("pinboard",NF_MRA|NF_ADD_LINK);
    
    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("subject",AF_OBLIGATORY));
    $this->add(new atkTextAttribute("description",AF_HIDE_LIST|AF_OBLIGATORY));
    $this->add(new atkDateAttribute("startdate"));
    $this->add(new atkDateAttribute("enddate"));
    $this->add(new atkBoolAttribute("showonpim",AF_HIDE_LIST));
    $this->add(new atkManyToOneRelation("userid", "employee.employee", AF_READONLY),"history");
    $this->add(new atkDateTimeAttribute("created_on","","",AF_HIDE_LIST|AF_READONLY),"history");
    $this->add(new updatedByAttribute("updated_by", "employee.employee", AF_READONLY|AF_HIDE_LIST),"history");
    $this->add(new atkUpdateStampAttribute("updated_on",AF_HIDE_LIST),"history");
    
    $this->setOrder("startdate");
    $this->setTable("pinboard_pinboard");
  }
  
  function initial_values()
  {
    $user = getUser();
    $now = array("year"=>date("Y"),
                 "month"=>date("m"),
                 "day"=>date("d"),
                 "hours"=>date("H"),
                 "minutes"=>date("i"),
                 "seconds"=>date("s"));
                 
    $next_week_timestamp = strtotime("+1 week");
    $next_week = array("year"=>date("Y",$next_week_timestamp),
                 "month"=>date("m",$next_week_timestamp),
                 "day"=>date("d",$next_week_timestamp),
                 "hours"=>date("H",$next_week_timestamp),
                 "minutes"=>date("i",$next_week_timestamp),
                 "seconds"=>date("s",$next_week_timestamp));
    
   return array("created_on"=>$now,
                "startdate"=>$now,
                "enddate"=>$next_week,
                "showonpim"=>1,
                "userid"=>array("id"=>$user["id"]),
                "updated_by"=>array("id"=>$user["id"]));
  }
  

  function action_admin(&$handler)
  {          
    global $g_sessionManager;
    $this->m_archive = $g_sessionManager->pageVar("archive");

    if ($this->m_archive==1)
    {
      $this->addFilter("pinboard_pinboard.enddate < now()");        
    }
    else
    {
      $this->addFilter("startdate<=now() AND enddate>=now()");
    }
      
    return $handler->action_admin();
  }
  
  function adminHeader()
  {
    $res="";
    if ($this->m_archive!=1)
    {
      $res .= href(dispatch_url($this->atknodetype(), $this->m_action, array("archive"=>1)),
                  atkText("showarchive", "pinboard", "pinboard"));
    }
    else
    {
      $res .= href(dispatch_url($this->atknodetype(), $this->m_action, array("archive"=>0)),
                  atkText("showactive", "pinboard", "pinboard"));
    } 
    $res.="&nbsp;&nbsp;&nbsp;";
    return $res;
  }
}
?>
