<?php

  //1-N relation carlease - carlease_maintenance
  userelation("atkonetomanyrelation");
  //N-1 relation carlease - organization
  userelation("atkmanytoonerelation");
  
  useattrib("atkdateattribute");
  useattrib("atknumberattribute");
  useattrib("atkcurrencyattribute");
  useattrib("atklistattribute");
  useattrib("carlease.carlease_currentOwnerAttribute");
  atkimport("module.utils.dateutil");

  class carlease extends atkNode
  {
    function carlease()
    {
      //create new node carlease
      $this->atkNode("carlease", NF_EDITAFTERADD|NF_ADD_LINK);
      //set id field as primary key
      $this->add(new atkAttribute("id", AF_AUTOKEY));
      $this->add(new atkAttribute("licensenumber", AF_OBLIGATORY|AF_UNIQUE|AF_SEARCHABLE));
      $this->add(new atkAttribute("carbrand", AF_SEARCHABLE));
      $this->add(new atkAttribute("cartype", AF_SEARCHABLE));
      $this->add(new carlease_currentOwnerAttribute("current_owner"));
      $this->add(new atkManyToOneRelation("leasecompany","organization.organization", AF_HIDE_LIST|AF_RELATION_AUTOLINK|AF_RELATION_AUTOCOMPLETE));
      $this->add(new atkAttribute("leasecontractnumber", AF_HIDE_LIST));
      $this->add(new atkCurrencyAttribute("leasesum", 0, 10, "&euro;"));
      $this->add(new atkCurrencyAttribute("employeecontribution", AF_HIDE_LIST, 10, "&euro;"));
      $this->add(new atkNumberAttribute("leaseperiod", AF_HIDE_LIST));
      $this->add(new atkCurrencyAttribute("cataloguedprice", AF_HIDE_LIST, 10, "&euro;"));
      $this->add(new atkCurrencyAttribute("taxcontribution", AF_HIDE_LIST, 10, "&euro;"));
      $this->add(new atkCurrencyAttribute("fueladvance", AF_HIDE_LIST, 10, "&euro;"));
      $this->add(new atkListAttribute("fuel",array("diesel","gasoline","lpg"),array(1,2,3),AF_HIDE_LIST|AF_OBLIGATORY));
      $this->add(new atkNumberAttribute("kmcontract", AF_HIDE_LIST));
      $this->add(new atkDateAttribute("startdate", AF_HIDE_LIST));
      $this->add(new atkDateAttribute("enddate"));
      $this->add(new atkOneToManyRelation("owner","carlease.carlease_owner","car_id", AF_HIDE_LIST));
      $this->add(new atkOneToManyRelation("maintenance","carlease.carlease_maintenance","car_id", AF_HIDE_LIST));
      
      $this->add(new atkOneToManyRelation("damage","carlease.carlease_damage","car_id", AF_HIDE_LIST));
      $this->add(new atkOneToManyRelation("insurance","carlease.carlease_insurance","car_id", AF_HIDE_LIST));
      
/*      $this->add(new atkManyToOneRelation("insurancecompany","organization.organization", AF_HIDE_LIST|AF_RELATION_AUTOLINK|AF_RELATION_AUTOCOMPLETE));
      $this->add(new atkNumberAttribute("insuranceownrisk", AF_HIDE_LIST));*/
      $this->add(new atkManyToOneRelation("tankcardcompany","organization.organization", AF_HIDE_LIST|AF_RELATION_AUTOLINK|AF_RELATION_AUTOCOMPLETE));
      $this->add(new atkAttribute("tankcardnr", AF_HIDE_LIST|AF_SEARCHABLE));
      $this->add(new atkNumberAttribute("tankcardcode", AF_HIDE_LIST|AF_SEARCHABLE));
      $this->add(new atkListAttribute("status", array("active","nonactive"), array(), AF_HIDE_LIST|AF_HIDE_ADD|AF_OBLIGATORY|AF_LIST_NO_NULL_ITEM|AF_FORCE_LOAD));
      if ((atkArrayNvl($ATK_VARS, "atknodetype", "") == "carlease.carlease") && in_array(atkArrayNvl($ATK_VARS, "atkaction", ""), array("admin", "add", "update", "edit")))
      {
        $statusfilter = ($this->getView() == "active") ? "active" : "nonactive";
        $this->addFilter("carlease.status",$statusfilter);
      }
      
      $this->setOrder("licensenumber");
      
      $this->setIndex("carbrand");
      
      $this->setTable("carlease");
    }
    
    function rowcolor($record)
    {
      $car_id = $record["id"];
      $enddate = $record["enddate"];
      $startdate = $record["startdate"];
      
      if ($enddate!="")
      {
        $currentdate = date("Ymd");
        $enddate = dateutil::arr2str($enddate);
                
        //make row red if enddate < currentdate
        if($enddate <= $currentdate) return "#FF0000";// color red
        
        // make date 4 months away from current date
         $fourmonthcheck = mktime(12,0,0,date("m")+4, date("d"), date("Y"));
         $fourmonthcheck = date("Ymd",$fourmonthcheck);
        //make row orange if current date 4 moths or less away from enddate
        if($enddate <= $fourmonthcheck) return "#FF9900"; 
           
      }
    }
    
    function descriptor_def()
    {
      return "[licensenumber]";
    }
    
    function preAdd(&$record, $mode="")
    {
      $record["licensenumber"] = strtoupper($record["licensenumber"]);
      return true;
    }
    
  function initial_values()
  {
    $initial = array();
    $initial["status"] = "active";
    return $initial;
  }

  /**
   * Are we in 'active' emps or 'archive' mode?
   */
  function getView()
  {
    $sessionManager = &atkGetSessionManager();
    $view = $sessionManager->stackVar("view");
    if ($view=="")
    {
      $view = "active";
    }
    return $view;
  }

  function action_admin($handler)
  {

    $view = $this->getView();
    if ($view=="active")
    {
      $this->addFilter("carlease.status","active");
    }
    else
    {
      $this->addFilter("carlease.status","nonactive");
    }
    return $handler->action_admin();
  }

  function adminFooter()
  {
    $view = $this->getView();

    if ($view=="active")
    {
      return atktext("onlyactiverecordsareshown")." ".href('dispatch.php?atknodetype='.$this->atkNodeType().'&atkaction='.$this->m_action.
                  '&view=nonactive', atktext('clicktoviewarchive', $this->m_module, $this->m_type))."<br>";
    }
    else
    {
      return atktext("youareinthearchive")." ".href('dispatch.php?atknodetype='.$this->atkNodeType().'&atkaction='.$this->m_action.
                  '&view=active', atktext('clicktoviewactiverecords', $this->m_module, $this->m_type))."<br>";
    }
  }
    
  }

?>