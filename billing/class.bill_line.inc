<?php

  userelation("atkmanytoonerelation");
  userelation("atkonetomanyrelation");
  useattrib("atktextattribute");
  useattrib("atkdummyattribute");
  useattrib("atkradioattribute");
  useattrib("atknumberattribute");
  useattrib("atkCurrencyAttribute");

  include_once("achievotools.inc");

  class bill_line extends atkNode
  {
    var $m_view;

    function bill_line()
    {
      $this->atkNode("bill_line");

      $this->add(new atkAttribute("id", AF_AUTOKEY));
      $this->add(new atkAttribute("shortdescription",AF_OBLIGATORY, 100));
      $this->add(new atkTextAttribute("description", TEXT_SMALL, AF_HIDE_LIST));
      $this->add(new atkTextAttribute("bill_description", TEXT_SMALL, AF_HIDE|AF_READONLY));
      $this->add(new atkDummyAttribute("fixed","",AF_HIDE_LIST));
      $this->add(new atkRadioAttribute("calcoption",array("fixedcalc", "aftercalc", "discount", "expenses"),array("fixed","calc","discount","expenses"), AF_OBLIGATORY|AF_READONLY_EDIT));
      $this->add(new atkDummyAttribute("fixedamounthint", "<b>".atktext("fixedamount", "billing")."</b>", AF_HIDE_LIST));
      $this->add(new atkCurrencyAttribute("fixed_amount"));
      $this->add(new atkDummyAttribute("currency","",AF_HIDE_LIST));
      $hoursrel = &$this->add(new atkOneToManyRelation("hours", "timereg.hours", "billing_bill_line_id", AF_READONLY));
      $hoursrel->setDestinationFilter("hours.billing_billed=1");
      $this->add(new atkDummyAttribute("remaininghoursamount", "", AF_DUMMY_SHOW_LABEL));
      $this->add(new atkManyToOneRelation("bill_id","billing.bill",AF_HIDE));
      $this->add(new atkManyToOneRelation("taxrate", "billing.taxrate", AF_OBLIGATORY));
      $this->add(new atkDummyAttribute("taxamount","",AF_DUMMY_SHOW_LABEL|AF_HIDE_LIST|AF_HIDE_ADD|AF_HIDE_EDIT));
      $this->add(new atkDummyAttribute("amountexcltax","",AF_DUMMY_SHOW_LABEL|AF_HIDE_LIST|AF_HIDE_ADD|AF_HIDE_EDIT));
      $this->add(new atkDummyAttribute("amountincltax","",AF_DUMMY_SHOW_LABEL|AF_HIDE_LIST|AF_HIDE_ADD|AF_HIDE_EDIT));

      $this->setTable("billing_bill_line");

      $this->m_securityMap["save_hours"] = "add";
      $this->m_securityMap["nohours"] = "add";
      $this->m_securityMap["save_expenses"] = "add";
      $this->m_securityMap["mark_expenses"] = "add";
      $this->m_securityMap["mark_hours"] = "add";

      $this->setSecurityAlias("billing.bill");
    }

    function calculate_tax($record)
    {
      $taxrate = $record["taxrate"]["percentage"];
      $amount = $record["fixed_amount"];
      if (atkconfig("tax_included", DEFAULT_TAX_INCLUDED))
        return round($amount / (100 + $taxrate) * $taxrate, 2);
      else
        return round($amount / 100 * $taxrate, 2);
    }

    function taxamount_display($record, $mode)
    {
      if (isset($record["taxrate"]["percentage"]) && $record["taxrate"]["percentage"]!=="")
      {
        $curattr = new atkCurrencyAttribute("dummy");
        $displayvalue = $curattr->display(array("dummy"=>$this->calculate_tax($record)));
      }
      else
      {
        $displayvalue = $this->text("notax");
      }
      return $displayvalue;
    }

    function amountexcltax_display($record, $mode)
    {
      $amount = round($record["fixed_amount"], 2);
      if (atkconfig("tax_included", DEFAULT_TAX_INCLUDED))
        $amount -= $this->calculate_tax($record);
      $curattr = new atkCurrencyAttribute("dummy");
      return $curattr->display(array("dummy"=>$amount));
    }

    function amountincltax_display($record, $mode)
    {
      $amount = round($record["fixed_amount"], 2);
      if (!atkconfig("tax_included", DEFAULT_TAX_INCLUDED))
        $amount += $this->calculate_tax($record);
      $curattr = new atkCurrencyAttribute("dummy");
      return $curattr->display(array("dummy"=>$amount));
    }

    function descriptor_def()
    {
      return "[shortdescription]";
    }

    function remaininghoursamount_display(&$record)
    {
      // Get all hours from the database that are linked to this node
      $db = &atkGetDb();
      $hours = $db->getRows("SELECT `hours`.`time`,`hours`.`billing_rate` FROM `hours` WHERE `hours`.`billing_bill_line_id`='{$record["id"]}'");

      $currentamount = $record["fixed_amount"];
      foreach ($hours as $hour)
      {
        $currentamount -= $hour["billing_rate"] * $hour["time"]/ 60;
      }

      $curattr = new atkCurrencyAttribute("dummy");
      return $curattr->display(array("dummy"=>$currentamount));
    }

    function fixed_edit(&$rec)
    {
      atkimport("atk.layout");
      $layout = &layout::getInstance();

     $fixed_price = sessionLoad("fixed_price");
     if ($fixed_price == 0)
     {
       $text = "";
     }
     else
     {
       $db = &$this->getDb();
       $sql = "SELECT project_id FROM billing_bill WHERE id = " . $rec["bill_id"]["id"];
       $projectid=$db->getrows($sql);

       $sql = "SELECT id FROM billing_bill WHERE project_id = " . $projectid["0"]["project_id"];
       $bill_line_rec=$db->getrows($sql);

       for ($i=0;$i<count($bill_line_rec);$i++)
       {
         $bill_line_rec2[] = $bill_line_rec[$i]["id"];
       }

       $sql = "SELECT fixed_amount FROM billing_bill_line WHERE calcoption = 'fixed' AND bill_id IN (".implode(",",$bill_line_rec2).") ";
       $fixedrec=$db->getrows($sql);

       $sql = "SELECT fixed_amount FROM billing_bill_line WHERE calcoption = 'fixed' AND bill_id = ".$rec["bill_id"]["id"];
       $this_fixedrec=$db->getrows($sql);

       $billedamount = 0;
       for($i=0;$i<count($fixedrec);$i++)
       {
         $billedamount += $fixedrec[$i]["fixed_amount"];
       }

       $this_billedamount = 0;
       for($i=0;$i<count($this_fixedrec);$i++)
       {
         $this_billedamount += $this_fixedrec[$i]["fixed_amount"];
       }

       $text = '<input type="hidden" name="billedamount" value="' .$billedamount.'">';
       $text .= "<B>" . atktext("warning_fixedprice1", "billing") . " " . number_format($fixed_price, 2,",",".") . "! " . number_format($billedamount, 2,",",".") . " " . atktext("warning_fixedprice3", "billing") ." ". number_format($this_billedamount, 2,",",".")." ".atktext("warning_fixedprice4", "billing")."</B>";
     }
     return $text;
   }


    function currency_symbol_edit($rec)
    {
      atk_var_dump($rec);
      $text = $this->get_currency_symbol($rec["bill_id"]["id"], "bill");;
      return $text;
    }

    function fixed_amount_validate($rec)
    {
      $billedamount=$_REQUEST["billedamount"];

      $fixed_price = sessionLoad("fixed_price");
      if ($fixed_price != 0)
      {
        if ($rec["fixed_amount"] > ($fixed_price - $billedamount))
        {
          triggerError($rec, $this->m_attribList["fixed_amount"], 'error_billamount_tohigh');
          return false;
        }
      }
    }

    // Function to get the currency rate assigned to the
    // customer who will be billed with the bill belonging
    // to $bill_line_id
    function get_currency_rate($bill_line_id)
    {
      $db = &$this->getDb();
      $currency = $db->getrows("select billing_currency.value from billing_currency, organization, person, project_person, project, billing_bill, billing_bill_line
                                  where billing_currency.symbol=organization.billing_currency and organization.id=person.company and person.id=project_person.personid
                                              and project_person.projectid=project.id and project.id=billing_bill.project_id
                                              and billing_bill.id=billing_bill_line.bill_id and billing_bill_line.id='".$bill_line_id."'");
      $value = (count($currency) > 0) ? $currency[0]["value"] : 1;
      return $value;
    }

    // Function to get the currency symbol assigned to the
    // customer who will be billed with the bill belonging
    // to $bill_line_id
    function get_currency_symbol($bill_line_id, $bill=0)
    {
      $db = &$this->getDb();
      if ($bill == 0) $currency = $db->getrows("select
                                                    organization.billing_currency
                                                  from
                                                    organization,
                                                    person,
                                                    project_person,
                                                    project,
                                                    billing_bill,
                                                    billing_bill_line
                                                  where
                                                    organization.id=person.company
                                                    and person.id=project_person.personid
                                                    and project_person.projectid=project.id
                                                    and project.id=billing_bill.project_id
                                                    and billing_bill.id=billing_bill_line.bill_id
                                                    and billing_bill_line.id='".$bill_line_id."'");
      else $currency = $db->getrows("select
                                         organization.billing_currency
                                       from
                                         billing_currency,
                                         organization,
                                         person,
                                         project_person,
                                         project,
                                         bill
                                       where
                                         organization.id=person.company
                                         and person.id=project_person.personid
                                         and project_person.projectid=project.id
                                         and project.id=billing_bill.project_id
                                         and bill.id='".$bill_line_id."'");
      return $currency[0]["currency"];

    }

    function action_save(&$handler)
    {
      if (atkArrayNvl($this->m_postvars, "atkcancel", "") == "")
      {
        $record = $this->updateRecord();
        $this->validate($record, "add");
        if (count(atkArrayNvl($record, "atkerror")) > 0)
        {
          return $handler->action_save();
        }
        else
        {
          $this->addDb($record);
        }
      }
      else
      {
         // Cancel was pressed
         $this->redirect();
      }
    }

    function action_save_expenses(&$handler)
    {
    //this function adds the bill_id into the right hours records that were selected.

      $db = &$this->getDb();
      $bill_line_id = $_REQUEST["bill_line_id"];

      $marked_expenses = array();
      $marked = $_REQUEST["mark"];
      if (is_array($marked))
      {
        $marked_expenses = array_keys($marked);
      }

      $line = $this->getBillLine($bill_line_id);

      $currency_rate = $this->get_currency_rate($bill_line_id);

      $saveandclose = ($this->m_postvars["saveclose"]!=""?true:false);

      //Check if there are any hours selected.
      if (count($marked_expenses)==0 && $saveandclose)
      {
        // Setup the ui and page components
        $ui = &$this->getUI();
        $page = &$this->getPage();
        $this->addStyle("style.css");

        // Compose the box content and title
        $content = atktext("bill_noexpenses", "billing") . "<br><br>";
        $content.= '<form action="dispatch.php" method="post" name="generatebillform">';
        $content.= session_form();
        $content.= '<input type="hidden" name="atknodetype" value="billing.bill_line">';
        $content.= '<input type="hidden" name="atkaction" value="nohours">';
        $content.= '<input type="hidden" name="type" value="expenses">';
        $content.= '<input type="hidden" name="bill_line_id" value="'.$bill_line_id.'">';
        $content.= '<input type="submit" name="back" value="'.atktext("bill_goback", "billing").'"> <input type="submit" name="cancel" value="'.atktext("cancel", "billing").'">';
        $content.= "</form>";
        $title = atktext("title_noexpenses", "billing");

        // Render the box and add it to the page
        $page->addContent($ui->renderBox(array("title" => $title, "content" => $content)));
      }
      else
      {

        // Cleanup previous entries.
        $db->query("UPDATE billing_expense SET bill_line_id=NULL WHERE bill_line_id = ".$bill_line_id);

        if (count($marked_expenses) > 0)
        {
          $sql = " UPDATE billing_expense SET bill_line_id = ".$bill_line_id. " WHERE id IN (".implode(",",$marked_expenses).") ";
          $db->query($sql);
        }


        $sql = "SELECT amount FROM billing_expense WHERE bill_line_id = ".$bill_line_id;
        $recs = $db->getrows($sql);

        $total = 0;
        for ($i=0, $_i=count($recs);$i<$_i;$i++)
        {
          $total += $recs[$i]["amount"];
        }

        $sql = "UPDATE billing_bill_line SET fixed_amount = ".$total." WHERE id = ".$bill_line_id;
        $db->query($sql);

        if ($saveandclose)
        {
          $this->redirect();
        }
        else
        {
          return $this->action_mark_expenses($handler);
        }

      }
    }

    function action_save_hours(&$handler)
    {
       //this function adds the bill_id into the right hours records that were selected.
      global $db;
      $bill_line_id = $_REQUEST["bill_line_id"];

      $db = &$this->getDb();
      $marked_hours = array();
      $marked = $_REQUEST["mark"];
      if (is_array($marked))
      {
        $marked_hours = array_keys($marked);
      }
      $billed_hours = array();
      $billed = atkArrayNvl($_REQUEST, "bill");
      if (is_array($billed))
      {
        $billed_hours = array_keys($billed);
      }
      $rates = array();
      if (is_array($_REQUEST["rate"]))
      {
        $rates = $_REQUEST["rate"];
      }

      $line = $this->getBillLine($bill_line_id);

      $currency_rate = $this->get_currency_rate($bill_line_id);

      $saveandclose = ($this->m_postvars["saveclose"]!=""?true:false);

      //Check if there are any hours selected.
      if ($line["calcoption"]!="fixed" && count($marked_hours)==0 && $saveandclose)
      {
        // Setup the ui and page components
        $ui = &$this->getUI();
        $page = &$this->getPage();
        $this->addStyle("style.css");

        $content = atktext("bill_nohours", "billing");
        $content.= '<form action="dispatch.php" method="post" name="generatebillform">';
        $content.= session_form();
        $content.= '<input type="hidden" name="atknodetype" value="billing.bill_line">';
        $content.= '<input type="hidden" name="atkaction" value="nohours">';
        $content.= '<input type="hidden" name="type" value="hours">';
        $content.= '<input type="hidden" name="bill_line_id" value="'.$bill_line_id.'">';
        $content.= '<table border="0" cellspacing="0" cellpadding="2">';
        $content.= '<tr>';
        $content.= '<td><br><br><input type="submit" name="back" value="'.atktext("bill_goback", "billing").'"><input type="submit" name="cancel" value="'.atktext("cancel", "atk").'"></td>';
        $content.= '</tr>';
        $content.= '</table>';
        $content.= '</form>';
        $title = atktext("title_nohours", "billing");

        // Render the box and add it to the page
        $page->addContent($ui->renderBox(array("title" => $title, "content" => $content)));
      }
      else
      {

        // Cleanup previous entries.
        $db->query("UPDATE hours SET billing_rate=NULL, billing_billed=0, billing_marked=0 WHERE billing_bill_line_id = ".$bill_line_id);

        // Rate collector, to optimize the rate queries.
        $rate_reverse = array();
        foreach ($rates as $hoursid=>$rate)
        {
          if ($billed[$hoursid]!="")
          {
            $rate_reverse[$rate][] = $hoursid;
          }
        }

        foreach ($rate_reverse as $rate => $idArr)
        {
          $sql = " UPDATE hours SET billing_rate = ".$rate. " WHERE id IN (".implode(",",$idArr).") ";
          $db->query($sql);
        }

        //check which hours are selected to do a proper Query
        if (count($billed_hours) > 0)
        {
          $sql = " UPDATE hours SET billing_bill_line_id = ".$bill_line_id. ", billing_billed = 1 WHERE id IN (".implode(",",$billed_hours).") ";
          $db->query($sql);
        }

        if (count($marked_hours) > 0)
        {
          $sql = " UPDATE hours SET billing_bill_line_id = ".$bill_line_id. ", billing_marked = 1 WHERE id IN (".implode(",",$marked_hours).") ";
          $db->query($sql);
        }

        if ($line["calcoption"]=="calc")
        {
          $sql = "SELECT time, billing_rate FROM hours WHERE billing_bill_line_id = ".$bill_line_id." AND billing_billed=1";
          $recs = $db->getrows($sql);

          $total = 0;
          for ($i=0, $_i=count($recs);$i<$_i;$i++)
          {
            $total += (($recs[$i]["time"]/60) * $recs[$i]["billing_rate"]);
          }

          $sql = "UPDATE billing_bill_line SET fixed_amount = ".$total." WHERE id = ".$bill_line_id;
          $db->query($sql);

        }

        if ($saveandclose)
        {
          $this->redirect();
        }
        else
        {
          return $this->action_mark_hours($handler);
        }

      }
    }

    function action_nohours(&$handler)
    {
      $cancel = $this->m_postvars["cancel"];
      $type = $this->m_postvars["type"];
      $bill_line_id = $this->m_postvars["bill_line_id"];

      if ($cancel!="")
      {
        $db = &$this->getDb();
        $sql = "DELETE FROM billing_bill_line WHERE id = ".$bill_line_id;
        $db->query($sql);
        $this->redirect();
      }
      else
      {
        $funcname = "action_mark_".$type;
        return $this->$funcname($handler);
      }

    }

    function postAdd($rec)
    {
      if (atkArrayNvl($this->m_postvars, "atknodetype") == "billing.bill_line")
      {
        $bill_line_id = $rec["id"];
        $bill_id = $rec["bill_id"]["id"];

        $billnode = &getNode("billing.bill");
        $bills = $billnode->selectDb("`billing_bill`.`id`='$bill_id'");
        if ($rec["calcoption"] == "discount")
        {
          $this->redirect(session_url(dispatch_url("billing.discount", "add", array("bill_id"=>$bill_id, "bill_line_id"=>$bill_line_id)), SESSION_REPLACE));
        }
        else if ($rec["calcoption"] == "expenses")
        {
          $this->redirect(session_url(dispatch_url("billing.bill_line", "mark_expenses", array("bill_line_id"=>$bill_line_id)), SESSION_REPLACE));
        }
        else if ($bills[0]["billtype"] == BILL_NORMAL)
        {
          $this->redirect(session_url(dispatch_url("billing.bill_line", "mark_hours", array("bill_line_id"=>$bill_line_id)), SESSION_REPLACE));
        }
        else
        {
          $this->redirect();
        }
      }
      return true;
    }

    function get_activities($act_id)
    {
      $db = &$this->getDb();
      $activityid = $_REQUEST["activityid"];
      // Get the activities
      $sql = "SELECT id,name
              FROM activity
              ORDER BY name
             ";
      $records = $db->getrows($sql);
      if($act_id==-1) { $sel="SELECTED"; } else { $sel=""; }
      $activity_code='<OPTION VALUE="all" SELECTED>'.atktext("allactivities", "billing");
      for($i=0;$i<count($records);$i++)
      {
        if($act_id==$records[$i]["id"]) { $sel="SELECTED"; } else {$sel=""; }
        $activity_code.='<OPTION VALUE="'.$records[$i]["id"].'"'.$sel.'>'.$records[$i]["name"].'</OPTION>';
      }
      return $activity_code;
    }

    function get_employees($user_id)
    {
      $db = &$this->getDb();

      $sql = "SELECT id, lastname,userid,firstname
              FROM person
              WHERE status='active' AND role='employee'
              ORDER BY lastname
             ";

      $records = $db->getrows($sql);
      $employee_code='<OPTION VALUE="all">'.atktext("allusers");
      for($i=0;$i<count($records);$i++)
      {
        if($user_id==$records[$i]["id"]) { $sel="SELECTED"; } else {$sel=""; }
        $employee_code.='<OPTION VALUE="'.$records[$i]["id"].'"'.$sel.'>'.$records[$i]["lastname"].', '.$records[$i]["firstname"].'</OPTION>';
      }
      return $employee_code;
    }

    function getProjectId($bill_id)
    {
      $db = &$this->getDb();
      //Query to get projectid
      $sql = "SELECT project_id FROM billing_bill where id = '$bill_id'";
      $recs=$db->getrows($sql);
      return $recs[0]["project_id"];
    }

    function getBillLine($bill_line_id)
    {
      $line_data = $this->selectDb("billing_bill_line.id = '$bill_line_id'");
      $line = $line_data[0];
      return $line;
    }

    function action_mark_expenses(&$handler)
    {
      global $g_sessionManager;

      // Page vars
      $bill_line_id = $g_sessionManager->pageVar("bill_line_id");
      $userid = $g_sessionManager->pageVar("userid");
      $startdate = $g_sessionManager->pageVar("startdate");
      $enddate = $g_sessionManager->pageVar("enddate");
      $view = $g_sessionManager->pageVar("view");
      if ($view=="") $view="all";

      if (!is_array($startdate))
      {
        $startstamp = mktime(12,0,0,date("m")-1,date("d"),date("Y"));
        $startdate = array("year"=>date("Y", $startstamp),
                           "month"=>date("m", $startstamp),
                           "day"=>date("d", $startstamp));
      }
      if (!is_array($enddate))
      {
        $enddate = array("year"=>date("Y"),
                        "month"=>date("m"),
                        "day"=>date("d"));
      }

      $line = $this->getBillLine($bill_line_id);

      $project_id = $this->getProjectId($line["bill_id"]["id"]);

      // Setup the ui and page components
      $ui = &$this->getUI();
      $page = &$this->getPage();
      $this->addStyle("style.css");

      $content = '<form action="dispatch.php" method="get" style="margin: 0px">';
      $content.= session_form();
      $content.= '<input type="hidden" name="atknodetype" value="billing.bill_line">';
      $content.= '<input type="hidden" name="atkaction" value="mark_expenses">';
      $content.= '<table border="0" cellspacing="0" cellpadding="2">';
      $content.= '<tr>';
      $content.= '<td colspan="3"><b>'.atktext("setexpensesfilter", "billing").'</b></td>';
      $content.= '</tr><tr>';
       $checked = ($view=="all"?"checked":"");
      $content.= '<td valign="top"><input type="radio" name="view" value="all" '.$checked.'></td>';
      $content.= '<td colspan="2" valign="top">'.atktext("showallunbilledexpenses", "billing").".<br><hr></td>";
      $content.= '</tr><tr>';
       $checked = ($view=="selected"?"checked":"");
      $content.= '<td valign="top"><input type="radio" name="view" value="selected" '.$checked.'></td>';
      $content.= '<td colspan="2" valign="top">'.atktext("showselectedexpenses", "billing").':</td>';
      $content.= '</tr><tr>';

        // we have to pass a 'dummy' record to the attributes to set their default value.
      $dummyrec = array("startdate"=>$startdate,
                        "enddate"=>$enddate);

      $content.= '<td></td>';
      $content.= '<td>'.atktext("name", "billing").':</br></td>';
      $content.= '<td><SELECT name="userid">'.$this->get_employees($userid).'</SELECT></td>';
      $content.= '</tr><tr>';
      $content.= '<td></td>';
      $content.= '<td>'.atktext("timespan").': </td>';
       $startdateatt = new atkDateAttribute("startdate");
       $enddateatt = new atkDateAttribute("enddate");
      $content.= '<td>'.$startdateatt->edit($dummyrec).' &nbsp;'.atktext("until", "atk").'&nbsp; '.$enddateatt->edit($dummyrec).'</td>';
      $content.= '</tr><tr>';
      $content.= '</tr></table><input type="submit" value="'.atktext("refresh").'"></form><br>';
      $title = atktext("title_expensespecify", "billing");

      // Render the box and add it to the page
      $page->addContent($ui->renderBox(array("title" => $title, "content" => $content)));

      $start_date = $startdate["year"]."-".$startdate["month"]."-".$startdate["day"];
      $end_date = $enddate["year"]."-".$enddate["month"]."-".$enddate["day"];

      //Query to get all the hours with their rates

      $sql = "SELECT
                billing_expense.id AS expenseid,
                EMPLOYEE.userid as user,
                billing_expense.expensedate,
                billing_expense.remark,
                billing_expense.amount,
                billing_expense.bill_line_id
              FROM
                billing_expense,
                person EMPLOYEE
              WHERE
                billing_expense.project_id= ".$project_id."
                AND billing_expense.user_id = EMPLOYEE.id ";

      if ($view=="selected"&&$userid!=""&&$userid!="all")
      {
        $sql.= "AND billing_expense.user_id = $userid ";
      }

      $sql.= "AND (billing_expense.bill_line_id IS NULL OR billing_expense.bill_line_id='' OR billing_expense.bill_line_id='".$bill_line_id."') ";

      if ($view=="selected")
      {
        $sql.="AND billing_expense.expensedate >= '$start_date'
               AND billing_expense.expensedate <= '$end_date' ";
      }
      $sql.= "ORDER BY billing_expense.id";


      $db = &$this->getDb();
      $expenserec=$db->getrows($sql);

      $page->register_script(moduleDir("billing")."javascript/checkall.js");

      $expensetable = array();
      $expensetable[] = array(
        atktext("expensedate", "billing"),
        atktext("owner", "billing"),
        atktext("remark", "billing"),
        atktext("value", "billing"),
        atktext("mark", "billing"),
      );

      for ($i=0;$i<count($expenserec);$i++)
      {
        $checked = ($expenserec[$i]["bill_line_id"]!=0?"checked":"");
        $expensetable[] = array(
          $expenserec[$i]["expensedate"],
          $expenserec[$i]["user"],
          $expenserec[$i]["remark"],
          $expenserec[$i]["amount"],
          '<input type="checkbox" id="mark_'.$expenserec[$i]["expenseid"].'" name="mark['.$expenserec[$i]["expenseid"].']" value="1" '.$checked.'>',
        );
      }

      $tablerenderer = &atknew("atk.utils.atktablerenderer");
      $expensetablehtml = $tablerenderer->render($expensetable, TBL_DATA, "recordlist");
      $links = array(
        '<a href="javascript:bill_markAll()">'. atktext("mark_all", "billing").'</a>',
        '<a href="javascript:bill_markNone()">'. atktext("mark_none", "billing").'</a>',
      );

      $content = '<form action="dispatch.php" name="billform" method="post">';
      $content.= session_form();
      $content.= '<input type="hidden" name="atknodetype" value="billing.bill_line">';
      $content.= '<input type="hidden" name="atkaction" value="save_expenses">';
      $content.= '<input type="hidden" name="bill_line_id" value='.$bill_line_id.'>';
      $content.= $expensetablehtml;
      $content.= '<div align="right">[ '.implode(' ', $links).' ]</div>';
      $content.= '<table border="0" cellspacing="0" cellpadding="2">';
      $content.= '<tr>';
      $content.= '<td><br><input type="submit" name="saveclose" value="'.atktext("saveandclose", "atk").'"></td>';
      $content.= '<td><br><input type="submit" name="save" value="'.atktext("save", "atk").'"></td>';
      $content.= '</tr>';
      $content.= '</table>';
      $content.= '</form>';
      $title = atktext("title_selectexpenses", "billing");

      // Render the box and add it to the page
      $page->addContent($ui->renderBox(array("title" => $title, "content" => $content)));
    }

    function action_mark_hours(&$handler)
    {
      global $g_sessionManager;

      // Page vars
      $bill_line_id = $g_sessionManager->pageVar("bill_line_id");
      $activityid = $g_sessionManager->pageVar("activityid");
      $userid = $g_sessionManager->pageVar("userid");
      $startdate = $g_sessionManager->pageVar("startdate");
      $enddate = $g_sessionManager->pageVar("enddate");
      $view = $g_sessionManager->pageVar("view");
      if ($view=="") $view="all";

      if (!is_array($startdate))
      {
        $startstamp = mktime(12,0,0,date("m")-1,date("d"),date("Y"));
        $startdate = array("year"=>date("Y", $startstamp),
                           "month"=>date("m", $startstamp),
                           "day"=>date("d", $startstamp));
      }
      if (!is_array($enddate))
      {
        $enddate = array("year"=>date("Y"),
                        "month"=>date("m"),
                        "day"=>date("d"));
      }

      $line = $this->getBillLine($bill_line_id);

      $project_id = $this->getProjectId($line["bill_id"]["id"]);

      // Setup the ui and page components
      $ui = &$this->getUI();
      $page = &$this->getPage();
      $this->addStyle("style.css");

      $content = '<form action="dispatch.php" method="get">';
      $content.= session_form();
      $content.= '<input type="hidden" name="atknodetype" value="billing.bill_line">';
      $content.= '<input type="hidden" name="atkaction" value="mark_hours">';
      $content.= '<table border="0" cellspacing="0" cellpadding="2">';
      $content.= '<tr>';
      $content.= '<td colspan="3"><b>'.atktext("sethoursfilter").'</b></td>';
      $content.= '</tr><tr>';
     $checked = ($view=="all"?"checked":"");
      $content.= '<td valign="top"><input type="radio" name="view" value="all" '.$checked.'></td>';
      $content.= '<td colspan="2" valign="top">'.atktext("showallunbilledhours", "billing").'.<br><hr></td>';
      $content.= '</tr><tr>';
     $checked = ($view=="selected"?"checked":"");
      $content.= '<td valign="top"><input type="radio" name="view" value="selected" '.$checked.'></td>';
      $content.= '<td colspan="2" valign="top">'.atktext("showselectedhours", "billing").':</td>';
      $content.= '</tr><tr>';

        // we have to pass a 'dummy' record to the attributes to set their default value.
      $dummyrec = array("startdate"=>$startdate,
                        "enddate"=>$enddate);

      $content.= '<td></td>';
      $content.= '<td>'.atktext("activity").':</b></td>';
      $content.= '<td><SELECT name="activityid">'.$this->get_activities($activityid).'</SELECT></td>';
      $content.= '</tr><tr>';
      $content.= '<td></td>';
      $content.= '<td>'.atktext("name", "billing").':<br /></td>';
      $content.= '<td><SELECT name="userid">'.$this->get_employees($userid).'</SELECT></td>';
      $content.= '</tr><tr>';
      $content.= '<td></td>';
      $content.= '<td>'.atktext("timespan").': </td>';
      $startdateatt = new atkDateAttribute("startdate");
      $enddateatt = new atkDateAttribute("enddate");

      $content.= '<td>'.$startdateatt->edit($dummyrec).' &nbsp;'.atktext("until", "atk").'&nbsp; '.$enddateatt->edit($dummyrec).'</td>';
      $content.= '</tr><tr>';
      $content.= '</tr></table><input type="submit" value="'.atktext("refresh").'"></form><br>';
      $title = atktext("title_hourspecify", "billing");

      // Render the box and add it to the page
      $page->addContent($ui->renderBox(array("title" => $title, "content" => $content)));

      $start_date = $startdate["year"]."-".$startdate["month"]."-".$startdate["day"];
      $end_date = $enddate["year"]."-".$enddate["month"]."-".$enddate["day"];

      //Query to get all the hours with their rates

      $sql = "SELECT
                hours.id AS hoursid,
                hours.remark,
                EMPLOYEE.userid as user,
                hours.activitydate,
                hours.time,
                billing_rate.*,
                activity.name AS activityname,
                hours.billing_marked,
                hours.billing_billed
              FROM
                hours,
                billing_rate,
                phase,
                person CONTACT,
                person EMPLOYEE,
                project_person,
                project,
                activity
       WHERE
         hours.phaseid = phase.id
         AND hours.activityid = activity.id
         AND hours.userid = EMPLOYEE.id ";

      if ($view=="selected"&&$userid!=""&&$userid!="all")
      {
        $sql.= "AND hours.userid = '$userid' ";
      }
      if ($view=="selected"&&$activityid!=""&&$activityid!="all")
      {
        $sql.= "AND hours.activityid = '$activityid' ";
      }

      $sql.="AND project_person.projectid = phase.projectid
         AND CONTACT.id = project_person.personid
         AND phase.projectid = '$project_id'
         AND (project.id = phase.projectid)
         AND (billing_rate.user_id='' OR billing_rate.user_id IS NULL OR billing_rate.user_id = hours.userid)
         AND (billing_rate.activity_id ='' OR billing_rate.activity_id IS NULL OR billing_rate.activity_id = hours.activityid)
         AND (billing_rate.project_id = '' OR billing_rate.project_id IS NULL OR billing_rate.project_id = phase.projectid)
         AND (billing_rate.customer_id = '' OR billing_rate.customer_id IS NULL OR billing_rate.customer_id = CONTACT.company)
         AND (billing_rate.projecttype_id = '' OR billing_rate.projecttype_id IS NULL OR billing_rate.projecttype_id = project.billing_type)
         AND (hours.billing_bill_line_id IS NULL OR hours.billing_bill_line_id='' OR hours.billing_bill_line_id='".$bill_line_id."') ";

      if ($view=="selected")
      {
        $sql.="AND hours.activitydate >= '$start_date'
               AND hours.activitydate <= '$end_date' ";
      }
      $sql.= "ORDER BY hours.id, billing_rate.priority DESC, billing_rate.rate DESC;";


      $db = &$this->getDb();
      $hourrec=$db->getrows($sql);

      $page->register_script(moduleDir("billing")."javascript/checkall.js");
      $page->register_script("atk/javascript/recordlist.js");

      $hourstable = array();
      $hoursrecord = array(
        atktext("activitydate", "billing"),
        atktext("owner", "billing"),
        atktext("activity", "billing"),
        atktext("remark", "billing"),
        atktext("time", "billing"),
        atktext("rate", "billing"),
        atktext("mark", "billing"),
      );
      if ($line["calcoption"] == 'calc') $hoursrecord[] = atktext("bill", "billing");
      $hourstable[] = $hoursrecord;

      for ($i=0;$i<count($hourrec);$i++)
      {
        if ($hourrec[$i]["hoursid"] == $hourrec[$i-1]["hoursid"])
        {
        }
        else
        {
          $checked = ($hourrec[$i]["billing_marked"]!=0?"checked":"");
          $hoursrecord = array(
            '<input type="hidden" name="rate['.$hourrec[$i]["hoursid"].']" value="'.$hourrec[$i]["rate"].'">'.$hourrec[$i]["activitydate"],
            $hourrec[$i]["user"],
            $hourrec[$i]["activityname"],
            $hourrec[$i]["remark"],
            time_format($hourrec[$i]["time"]),
            atkconfig("currency_symbol") . " " . round($hourrec[$i]["rate"],2),
            '<input type="checkbox" id="mark_'.$hourrec[$i]["hoursid"].'" name="mark['.$hourrec[$i]["hoursid"].']" value="1" '.$checked.'>',
          );
          if ($line["calcoption"] == 'calc')
          {
            $checked = ($hourrec[$i]["billing_billed"]!=0?"checked":"");
            $hoursrecord[] = '<input type="checkbox" id="bill_'.$hourrec[$i]["hoursid"].'" name="bill['.$hourrec[$i]["hoursid"].']" value="1" '.$checked.' onClick="bill_mark('.$hourrec[$i]["hoursid"].')">';
          }
          $hourstable[] = $hoursrecord;
        }
      }
      $tablerenderer = &atknew("atk.utils.atktablerenderer");
      $hourstablehtml = $tablerenderer->render($hourstable, TBL_DATA, "recordlist");

      $links = array(
        '<a href="javascript:bill_markAll()">'.atktext("mark_all", "billing").'</a>',
        '<a href="javascript:bill_markNone()">'.atktext("mark_none", "billing").'</a>',
      );
      if ($line["calcoption"] == 'calc')
      {
        $links[] = '<a href="javascript:bill_billAll()">'.atktext("bill_all", "billing").'</a>';
        $links[] = '<a href="javascript:bill_billNone()">'.atktext("bill_none", "billing").'</a>';
      }

      $content = '<form action="dispatch.php" name="billform" method="post">';
      $content.= session_form();
      $content.= '<input type="hidden" name="atknodetype" value="billing.bill_line">';
      $content.= '<input type="hidden" name="atkaction" value="save_hours">';
      $content.= '<input type="hidden" name="bill_line_id" value='.$bill_line_id.'>';
      $content.= $hourstablehtml;
      $content.= '<div align="right">[ '.implode(' ', $links).' ]</div>';
      $content.= '<table border="0" cellspacing="0" cellpadding="2">';
      $content.= '<tr>';
      $content.= '<td><br><input type="submit" name="saveclose" value="'.atktext("saveandclose", "billing").'"></td>';
      $content.= '<td><br><input type="submit" name="save" value="'.atktext("save", "billing").'"></td>';
      $content.= '</tr>';
      $content.= '</table>';
      $content.= '</form>';
      $title = atktext("title_selecthours", "billing");

      // Render the box and add it to the page
      $page->addContent($ui->renderBox(array("title" => $title, "content" => $content)));
    }

    function unlinkRelatedItems($rec)
    {
      $db = &$this->getDb();

      if ($rec["calcoption"] == "fixed")
      {
        $sql = "UPDATE hours SET billing_bill_line_id = NULL, billing_marked = 0, billing_billed = 0, billing_rate = NULL WHERE billing_bill_line_id = '". $rec["id"] . "'";
        $xrec=$db->getrows($sql);
      }
      elseif ($rec["calcoption"] == "calc")
      {
        //query to empty the bill_line_id and registered fields from the hours table when you delete a bill_line.
        $sql = "UPDATE hours SET billing_bill_line_id = NULL, billing_marked = 0, billing_billed = 0, billing_rate = NULL WHERE billing_bill_line_id = '". $rec["id"] . "'";
        $xrec=$db->getrows($sql);
      }
      elseif ($rec["calcoption"] == "discount")
      {
        $sql = "DELETE FROM billing_discount WHERE bill_line_id = '". $rec["id"] . "'";;
        $xrec=$db->getrows($sql);
      }
      elseif ($rec["calcoption"] == "expenses")
      {
        $sql = "UPDATE billing_expense SET bill_line_id = '' WHERE bill_line_id = '". $rec["id"] . "'";;
        $xrec=$db->getrows($sql);
      }
    }

    function postDel($rec)
    {
      $this->unlinkRelatedItems($rec);
      return true;
    }

  }

?>
