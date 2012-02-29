<?php

  include_once("achievotools.inc");
  useattrib("atkboolattribute");
  useattrib("atknumberattribute");
  useattrib("atktextattribute");
  useattrib("atkdateattribute");
  useattrib("atkdummyattribute");
  useattrib("billing.calccurrencyattribute");
  userelation("atkmanytoonerelation");


  class expense extends atkNode
  {

    var $m_lock = "";
    var $m_viewdate = "";
    var $m_weekview = "";
    var $m_user = "";

    function expense($name="expense", $flags=0)
    {
      // Call parent constructor with readonly flag if registration is blocked
      $this->atkNode($name, $this->hasBlock() ? NF_READONLY : 0);

      // Read pagevars
      $this->readPageVars();

      // Add attributes
      $this->add(new atkAttribute("id", AF_AUTOKEY));
      $this->add(new atkDateAttribute("expensedate","","", 0, date("Ymd"),AF_OBLIGATORY));
      $this->add(new atkDateAttribute("entrydate", AF_FORCE_LOAD|AF_HIDE|AF_OBLIGATORY));
      $this->add(new atkAttribute("registrationnumber",AF_SEARCHABLE,20));
      $expensetype = &$this->add(new atkManyToOneRelation("expensetype_id", "billing.expensetype", AF_OBLIGATORY|AF_SEARCHABLE));
      $expensetype->addOnChangeHandler("maybe_display_kmfield()");
      
      $p_attr = &$this->add(new atkManyToOneRelation("category","billing.expense_category",AF_SEARCHABLE));
      $p_attr->addDestinationFilter("pid IS NULL");
      $p_attr->addDependee("subcategory");

      $p_attr = &$this->add(new atkManyToOneRelation("subcategory","billing.expense_category",AF_SEARCHABLE));
      $p_attr->addDestinationFilter("pid IS NOT NULL");

      $p_attr = &$this->add(new atkNumberAttribute("km", AF_TOTAL, 5, 0));
      $p_attr->addDependee("kmamount");
      
      $expression = "is_null('[expensetype_id.km_price]') ? 0 : '[km]'*'[expensetype_id.km_price]'";
      $this->add(new calcCurrencyAttribute("kmamount",$expression,AF_TOTAL|AF_HIDE_LIST,10, atkconfig("default_currency", "EUR")));
      
      $p_attr = &$this->add(new atkCurrencyAttribute("amount", AF_TOTAL, 10, atkconfig("default_currency", "EUR")));
      $p_attr->addDependee("amount_with_vat");
      
      $p_attr = &$this->add(new atkManyToOneRelation("vat","billing.taxrate",AF_HIDE_LIST|AF_FORCE_LOAD));
      $p_attr->addDependee("amount_with_vat");

      $expression = "[amount]*( 1+(is_null('[vat.percentage]') ? 0 : '[vat.percentage]')/100);";
      $this->add(new calcCurrencyAttribute("amount_with_vat",$expression,AF_TOTAL,10, atkconfig("default_currency", "EUR")));

      $this->add(new atkAttribute("type",AF_HIDE|AF_FORCE_LOAD));
      $this->add(new atkTextAttribute("remark", 1, AF_HIDE_LIST));
      $this->add(new atkManyToOneRelation("bill_line_id", "billing.bill_line", AF_HIDE));
      $this->add(new atkManyToOneRelation("project_id", "billing.billing_projectselector", AF_SEARCHABLE));
      $this->add(new atkBoolAttribute("billable"));
      $this->add(new atkManyToOneRelation("user_id", "employee.employee", AF_HIDE_LIST));

      // Set other parameters
      $this->setTable("billing_expense");
      $this->setOrder("expensedate ASC");
      $this->setDescriptorTemplate("[remark]");
    }

    function readPageVars()
    {
      // Read session pagevars
      $sessionmanager = &atkGetSessionManager();
      $this->m_viewdate = $sessionmanager->pageVar("viewdate");
      $this->m_weekview = $sessionmanager->pageVar("weekview");
      $this->m_user = $sessionmanager->pageVar("viewuser");

      // if set by the date jumper, viewdate is an array. It may also be empty, if so use today
      if (is_array($this->m_viewdate))
        $this->m_viewdate = sprintf("%04d-%02d-%02d", $this->m_viewdate["year"], $this->m_viewdate["month"],$this->m_viewdate["day"]);
      elseif ($this->m_viewdate=="")
        $this->m_viewdate = strftime("%Y-%m-%d");

      // Use the currently logged in user
      if ($this->m_user == "")
        $this->m_user = atkArrayNvl(getUser(), "id");
    }

    function postAdd($record=NULL)
    {
      if (!$this->allowed("any_user"))
      {
        $this->m_attribList["user_id"]->m_flags |= AF_HIDE;
        $this->addFilter("billing_expense.user_id = ".atkArrayNvl(getUser(), "id"));
      }
      return parent::postAdd($record);
    }

    function edit_values($record, $locked=FALSE)
    {
      if (!$this->allowed("any_user"))
      {
        $this->m_attribList["user_id"]->m_flags |= AF_HIDE;
        $this->addFilter("billing_expense.user_id = ".atkArrayNvl(getUser(), "id"));
      }
      return $record;
    }

    function expensedate_validate(&$record)
    {
      if ($this->getLockType($this->m_user,sprintf("%04d-%02d-%02d",$record["expensedate"]["year"],$record["expensedate"]["month"],$record["expensedate"]["day"]))!="")
      {
        triggerError($record, "expensedate", "weeklocked");
      }
    }

    function validate(&$record, $mode)
    {

      $obj = getNode("billing.expensetype");
      $act = $obj->selectDb("id=".$record['expensetype_id']['id']);
      if (count($act)==1)
      {
        if ($act[0]["remarkrequired"]==1)
        {
          // Specification is required. So we set the obligatory flag.
          // The validation of this flag is done later on by the default
          // ::validate() method of the base class.
          $this->m_attribList["remark"]->m_flags |= AF_OBLIGATORY;
        }
      }
      else
      {
        triggerError($record, "expensetype_id", "error_obligatoryfield");
      }

      // validate km/amount fields

      if ($mode == "add" || $mode == "update")
      {
        $isKmField = false;
        $expenseTypes = $this->getKmExpenseTypes();

        foreach($expenseTypes as $row)
        {
          if ($record['expensetype_id']['id'] == $row['id'])
          {
            $isKmField = true;
            continue;
          }
        }

        $amountAttrib = &$this->getAttribute("amount");
        $kmAttrib = &$this->getAttribute("km");

        if ($isKmField)
        {
          $amountAttrib->removeFlag(AF_OBLIGATORY);
          $kmAttrib->addFlag(AF_OBLIGATORY);
        }
        else
        {
          $amountAttrib->addFlag(AF_OBLIGATORY);
          $kmAttrib->removeFlag(AF_OBLIGATORY);
        }
      }

      atkNode::validate($record, $mode);
    }

    function setLock()
    {
      $this->m_lock = $this->getLockType(strtolower($this->m_user),$this->m_viewdate);

      if ($this->m_lock != "") {
        // This week is locked..
        $this->addFlag(NF_NO_ADD|NF_NO_DELETE|NF_NO_EDIT);
      }
    }

    function action_admin(&$handler)
    {

      $this->setLock();

      $this->addFilter("billing_expense.user_id",$this->m_user);

      if ($this->m_weekview!=1)
      {
      	if ($this->m_lock=="") $this->postAdd();
      	$this->addFilter("expensedate",$this->m_viewdate);
      }
      else
      {
      	$this->m_startofweek=startOfWeek($this->m_viewdate);
      	$this->m_endofweek=endOfWeek($this->m_viewdate);
      	$this->addFilter("expensedate >= '".$this->m_startofweek."' AND expensedate <= '".$this->m_endofweek."'");
      }

      return $handler->action_admin();

    }


    function action_edit(&$handler)
    {
      $recs = $this->selectDb($this->m_postvars["atkselector"], "billing_expense.id", "", "", array("user_id","expensedate","entrydate"));

      $this->m_lock = $this->getLockType($recs[0]["user_id"]["id"],
                        sprintf("%02d-%02d-%02d", $recs[0]["expensedate"]["year"],
                                                  $recs[0]["expensedate"]["month"],
                                                  $recs[0]["expensedate"]["day"]));

      if ($this->m_lock=="")
      {
        $this->addFilter("billing_expense.user_id",strtolower($this->m_user));
        //return parent::action_edit();
        return $handler->action_edit();
      }
      else {
        // Week is locked. We may not edit, only view.
        return $handler->action_view();
      }

    }

    // Check if this week may be booked..
    // returns: "" - not locked
    //          "individual" - locked for the user only
    //          "all"       - locked for everybody (can only be unlocked by administrator)
    function getLockType($userid, $date)
    {
      static $s_locks = array();

      $week = weekstamp(mktime(0,0,0,substr($date,5,2),
                                       substr($date,8,2),
                                       substr($date,0,4)));

      if (!isset($s_locks[$userid][$week])) {
        $query = "SELECT
                    week, userid
                  FROM
                    billing_expense_lock
                  WHERE
                    (userid = '$userid' OR userid IS NULL OR userid='')
                  AND
                    week = '$week'";
        $db = &$this->getDb();
        $res = $db->getrows($query);

        if (count($res)) {
          $all = false;
          for ($i=0;$i<count($res);$i++) {
            if (empty($res[$i]["userid"])) {
              $s_locks[$userid][$week] = "all";
              $all = true;
            }
          }

          // apparently not locked for everybody.
          if (!$all) {
            $s_locks[$userid][$week] = "individual";
          }
        }
        else {
          $s_locks[$userid][$week]="none";
        }
      }
      return ($s_locks[$userid][$week]=="none"?"":$s_locks[$userid][$week]);
    }

    /**
     * This method is called by the framework to determine the allowed actions on
     * the billing_expense node.
     * In this case, the edit and delete actions are disabled if the record does
     * not belong to the current user.
     */
    function recordActions($record, &$actions, &$mraactions)
    {
      $user = getUser();
      $userid = atkArrayNvl($user, "id");
      $locktype = $this->getLockType($record["userid"]["id"],
                                     sprintf("%02d-%02d-%02d", $record["expensedate"]["year"],
                                                               $record["expensedate"]["month"],
                                                               $record["expensedate"]["day"]));
      if ($userid==""	|| $record["user_id"]["id"]!=$userid || $locktype!="")
      {
        if (!$this->allowed("any_user"))
        {
          unset($actions["edit"]);
          unset($actions["delete"]);
          $idx = array_search("delete", $mraactions);
          if ($idx!==false)
          {
            unset($mraactions[$idx]);
          }
        }
        else
        {
          $actionbase = $_SERVER["PHP_SELF"].'?atknodetype='.$this->atknodetype().'&atkselector=[pk]&viewuser='.$this->m_user;
          $actions = array();
          $actions["view"] = $actionbase."&atkaction=view";
          $actions["edit"] = $actionbase."&atkaction=edit";
          $actions["delete"] = $actionbase."&atkaction=delete";
        }
      }
    }


    function getLockIcon()
    {
      if ($this->m_lock!="")
      {
        // Week is locked
        return ' <img src="'.atkconfig("atkroot").'atk/images/lock.gif" border="0" alt="'.$this->text("locked").'">';
      }
      else
      {
        return '';
      }
    }

    function adminHeader()
    {
      $res='<form name="dayview" method="post" action="dispatch.php">';
      $res.='<table border=0 cellpadding=0 cellspacing=0 width="100%" class="table">';
      $res.='<tr><td align="left" valign="top">';

      $viewdatestamp = mktime(0,0,0,substr($this->m_viewdate,5,2),substr($this->m_viewdate,8,2),substr($this->m_viewdate,0,4));

      // we substract 3601 seconds to account for Daylight Savings Time
      $yesterday = date('Y-m-d',$viewdatestamp-(86400-3601));

      // we add 3601 seconds to account for Daylight Savings Time
      $tomorrow = date('Y-m-d',$viewdatestamp+86400+3601);

      $today = date('Y-m-d');

      $res.= href('dispatch.php?viewuser='.$this->m_user.'&viewdate='.$yesterday.'&weekview=0', atktext("previousday"));
      if ($this->m_viewdate<date('Y-m-d'))
      {
        // If current is not today we can also move forward
        $res.='&nbsp;&nbsp;'.href('dispatch.php?viewuser='.$this->m_user.'&viewdate='.$tomorrow.'&weekview=0', atktext("nextday"));
        $res.='&nbsp;&nbsp;'.href('dispatch.php?viewuser='.$this->m_user.'&viewdate='.$today.'&weekview=0', atktext("today"));
      }
      $res.= '&nbsp;&nbsp;'.href('dispatch.php?viewuser='.$this->m_user.'&viewdate='.$this->m_viewdate.'&weekview=1', atktext("gotoweekview"));
      $res.='</td><td align="right" valign="top">';
      $res.=session_form();
      $res.=$this->getUserSelect().'&nbsp;';
      $res.=$this->getDateJumper();
      $res.='&nbsp;<input type="submit" value="'.atktext("goto").'">';
      $res.='</td></tr></table></form>';

      if ($this->m_weekview!=1 ) {
      	$res.='<b>'.atktext(strtolower(date("l", $viewdatestamp)))." ".atktext(strtolower(date("F", $viewdatestamp)))." ".date("d, Y", $viewdatestamp).'</b>';
      }
      else {
      	$startdatestamp = mktime(0,0,0,substr($this->m_startofweek,5,2),substr($this->m_startofweek,8,2),substr($this->m_startofweek,0,4));
      	$enddatestamp = mktime(0,0,0,substr($this->m_endofweek,5,2),substr($this->m_endofweek,8,2),substr($this->m_endofweek,0,4));
      	$res.='<b>'.atktext(strtolower(date("l", $startdatestamp)))." ".atktext(strtolower(date("F", $startdatestamp)))." ".date("d", $startdatestamp).'</b>';
      	$res.='<b> - '.atktext(strtolower(date("l", $enddatestamp)))." ".atktext(strtolower(date("F", $enddatestamp)))." ".date("d, Y", $enddatestamp).'</b>';
      }

      $res.=$this->getLockIcon();
      return $res;
    }

    function adminFooter()
    {
      $db = &$this->getDb();

      $filter = '';
      if(is_a($this,"investments")) $filter = "AND type='i'";
      elseif (is_a($this,"regulars")) $filter = "AND type='r'";
      
      if ($this->m_weekview!=1)
      {
        $date = "AND expensedate='$this->m_viewdate'";
      }
      else
      {
        $date = "AND expensedate BETWEEN '$this->m_startofweek' AND '$this->m_endofweek'";
      }

      $rec = $db->getRows("SELECT 
    	                       SUM(amount) AS total,
    	                       SUM(amount*(1+IFNULL(percentage,0)/100)) as total_with_vat
    	                     FROM billing_expense
    	                       LEFT JOIN billing_taxrate
    	                       ON billing_expense.vat=billing_taxrate.id
                           WHERE user_id='$this->m_user' $filter $date");
      
      $t['total'] = $this->format_currency($rec[0]['total']);
      $t['total_with_vat'] = $this->format_currency($rec[0]['total_with_vat']);

      atkimport("atk.utils.atkstringparser");
      $str = &new atkStringParser(atktext("expenses_total", "billing"));
      $res = '<b>'.$str->parse($t).'</b>';
      
      return $res;
    }

    function getDateJumper()
    {
      $dummy_rec = array("viewdate"=>$this->getInitialDate());
      $datebox = new atkDateAttribute("viewdate","","",0, date("Ymd"),AF_OBLIGATORY);
      return $datebox->edit($dummy_rec);
    }

    function getUserSelect()
    {
      $db = &$this->getDb();
      if ($this->allowed("any_user")) {
        $users = $db->getRows("SELECT id, lastname, firstname FROM person WHERE role = 'employee' AND status = 'active'");
        $res = '<select name="viewuser">';
        for ($i=0;$i<count($users);$i++)
        {
          $res .= '<option value="'.$users[$i]["id"].'" '.($this->m_user == $users[$i]["id"] ? 'selected' : '').'>'.$users[$i]["lastname"].', '.$users[$i]["firstname"];
        }
        $res .= '</select>';
      }
      return $res;
    }

    function getInitialDate()
    {
      if ($this->m_viewdate!="") {
         $initial_date = Array("year"=>substr($this->m_viewdate,0,4),
                               "month"=>substr($this->m_viewdate,5,2),
                               "day"=>substr($this->m_viewdate,8,2));
      }
      else {
        $initial_date = Array("year"=>date("Y"),
                              "month"=>date("m"),
                              "day"=>date("d"));
      }
      return $initial_date;

    }

    function initial_values()
    {
       return array("user_id"=>array("id"=>$this->m_user),
                    "entrydate"=>Array("year"=>date("Y"),
                               "month"=>date("m"),
                               "day"=>date("d")),
                    "expensedate"=>$this->getInitialDate()
                    );
    }

    function editPage(&$handler,$record,$locked=false)
    {
      $this->registerKmFields();
      return $handler->editPage($record,$locked);
    }

    function addPage(&$handler,$record=null)
    {
      $this->registerKmFields();
      return $handler->addPage($record);
    }

    function registerKmFields()
    {
      $results = $this->getKmExpenseTypes();
      $atkPage = &atkPage::getInstance();

      foreach($results as $row)
      {
        $selectboxvalue = str_replace("'","\\'",$row['atkprimkey']);
        $atkPage->register_loadscript("register_km_option('$selectboxvalue');");
      }

      $atkPage->register_loadscript("maybe_display_kmfield();");
    }

    /**
     * Get all KM expensetype records
     *
     * @return Array KM expensetype records
     */
    function getKmExpenseTypes()
    {
      /** @todo Move the javascript registration to an interface related function. */
      // Register the km_displayfield javascript
      $page = &atkpage::getInstance();
      $page->register_script(moduleDir("billing") . "javascript/km_displayfield.js");

      // Get all KM expense types
      $expenseTypeNode = &getNode("billing.expensetype");
      $results = $expenseTypeNode->selectDb("`billing_expensetype`.`km_specification`='1'","","","",array('id'));

      // Return the KM expensetypes
      return $results;
    }

    function _getKmType($id)
    {
      $node = &getNode("billing.expensetype");
      return  $node->selectDb("km_specification='1' AND id='$id'","","","",array('id','km_price'));
    }
    
    function _calculateAmount(&$record)
    {
      $row = $this->_getKmType($record['expensetype_id']['id']);
      if(count($row))
      {
        $record['amount'] = $record['km'] * $row[0]['km_price'];
        $record['vat'] = '';
      }
      else 
      {
        $record['km'] = '';
        
      }
    }
    
    function preAdd(&$record)
    {
      $this->_calculateAmount($record);
      return parent::preAdd($record);
    }

    function preUpdate(&$record)
    {
      $this->_calculateAmount($record);
      return parent::preUpdate($record);
    }

    function getFormButtons($mode,$record)
    {
       $btns = parent::getFormButtons($mode,$record);
       if (atkLevel()>0)
           $btns[] = atkButton(atktext('back'),"",SESSION_BACK,true,"btn_cancel");
       return $btns;
    }

    /**
     * Checks if the current view should be blocked.
     *
     * @return boolean  true when node should be blocked, false when not.
     */
    function hasBlock()
    {
      $db = &atkGetDb();

      $user = &getUser();
      $userid = $user['id'];

      if (!$userid || $userid < 1) return false; // don't block for administrator

      $weeknumber = date("YW",strtotime($this->m_viewdate));

      $result = $db->getRows("SELECT COUNT(*) AS c FROM hours_lock WHERE period='$weeknumber' AND userid='$userid'");
      return (is_array($result) && sizeof($result) > 0 && $result[0]['c'] > 0);
    }

    function allowed($action,$record="")
    {
      if ($action != "view" && $action != "admin" && $this->hasFlag(NF_READONLY))
        return false;
      else
        return parent::allowed($action,$record);
    }
    
    function format_currency($value)
    {
      $p_attr = &$this->getAttribute("amount");
      return $p_attr->display(array("amount"=>$value));
    }
    
    function subcategory_selection($record, $mode='select')
    {
      $attr = &$this->getAttribute('subcategory');
    
      if ($mode == 'select')
      {
        $filter = $attr->getDestinationFilter();
        $attr->setDestinationFilter("pid='[category.id]'");
        $result = $attr->getSelectableRecords($record, $mode);
        $attr->setDestinationFilter($filter);
        return $result;
      }
      else
      {
        return $attr->getSelectableRecords($record, $mode);
      }
    } 
    
  }
?>
