<?php
  // Set modifiers
  $modifiers[] = "project.project";
  $modifiers[] = "employee.employee";
  $modifiers[] = "reports.surveyhours";
  $modifiers[] = "timereg.hours";

  // Define project.project billing_expensereg_limit options
  define("PRJ_EXPENSEREG_MEMBERS_ONLY", 1);
  define("PRJ_EXPENSEREG_ALL_USERS", 0);

  /**
   * Billing module
   */
  class mod_billing extends achievoModule
  {
    function getNodes()
    {
      // Register nodes and their actions
      registerNode("billing.billing_rate",     array("admin", "add", "edit", "delete"));
      registerNode("billing.expense",          array("admin", "add", "edit", "delete", "any_user", "any_project"));
      registerNode("billing.expensetype",      array("admin", "add", "edit", "delete"));
      registerNode("billing.expense_lock",     array("admin", "add", "delete"));
      registerNode("billing.finance_customer", array("admin", "edit"));
      registerNode("billing.finance_project",  array("admin", "edit"));
      registerNode("billing.currency",         array("admin", "add", "edit", "delete"));
      registerNode("billing.bill",             array("admin", "add", "edit", "delete", "set_new", "set_final", "set_sent", "set_cancelled", "set_paid"));
      registerNode("billing.expensereport",    array("report", "view_all"));
      registerNode("billing.income",           array("add", "edit", "delete"));
      registerNode("billing.fund",             array("add", "edit", "delete"));
      registerNode("billing.projecttype",      array("admin", "add", "edit", "delete"));
      registerNode("billing.report",           array("ratereport", "declarabilityreport", "coveragereport", "any_user"));
      registerNode("billing.taxrate",          array("admin", "add", "edit", "delete"));
    }

    function getMenuItems()
    {
      // Add the expenses menuitem and billing submenu and a spacer after it to the main menu
      $this->menuitem("expenses", dispatch_url("billing.regulars", "admin"), "main", array("billing.expense", "admin"), 101);
      //$this->menuitem("expense", dispatch_url("billing.expense", "admin"), "expenses", array("billing.expense", "admin"));
      $this->menuitem("regular_expenses", dispatch_url("billing.regulars", "admin"), "expenses", array("billing.expense", "admin"));
      $this->menuitem("investments", dispatch_url("billing.investments", "admin"), "expenses", array("billing.expense", "admin"));

      $this->menuitem("billing",  "",                                       "main", array("billing.billing_rate", "admin", "billing.bill", "admin", "billing.expense", "admin"),675);
      $this->menuitem("-",        "",                                       "main", array("billing.billing_rate", "admin", "billing.bill", "admin", "billing.expense", "admin"),676);

      // Add several items to the billing menu
      $this->menuitem("bills",            dispatch_url("billing.bill",             "admin"), "billing", array("billing.bill",             "admin"));
      $this->menuitem("-", "", "billing", true);
      $this->menuitem("currency",         dispatch_url("billing.currency",         "admin"), "billing", array("billing.currency",         "admin"));
      $this->menuitem("-", "", "billing", true);
      $this->menuitem("rates",            dispatch_url("billing.billing_rate",             "admin"), "billing", array("billing.billing_rate",             "admin"));
      $this->menuitem("finance_customer", dispatch_url("billing.finance_customer", "admin"), "billing", array("billing.finance_customer", "admin"));
      $this->menuitem("finance_project",  dispatch_url("billing.finance_project",  "admin"), "billing", array("billing.finance_project",  "admin"));
      $this->menuitem("-", "", "billing", true);
      $this->menuitem("expense_lock",     dispatch_url("billing.expense_lock",     "admin"), "billing", array("billing.expense_lock",     "admin"));
      $this->menuitem("-", "", "billing", true, 100000);

      // Add several items to the reports menu
      $this->menuitem("expensereport",       dispatch_url("billing.expensereport",  "report"),              "reports", array("billing.expensereport", "report"));
      $this->menuitem("ratereport",          dispatch_url("billing.report",         "ratereport"),          "reports", array("billing.report",        "ratereport"));
      $this->menuitem("declarabilityreport", dispatch_url("billing.report",         "declarabilityreport"), "reports", array("billing.report",        "declarabilityreport"));
      $this->menuitem("coveragereport",      dispatch_url("billing.coveragereport", "coveragereport"),      "reports", array("billing.report",        "coveragereport"));

      // Add several setup menu options
      $this->menuitem("expensetype",   dispatch_url("billing.expensetype", "admin"), "setup", array("billing.expensetype", "admin"));
      $this->menuitem("project_types", dispatch_url("billing.projecttype", "admin"), "setup", array("billing.projecttype", "admin"));
      $this->menuitem("taxrates",      dispatch_url("billing.taxrate",     "admin"), "setup", array("billing.taxrate",     "admin"));
      $this->menuitem("expense_category",      dispatch_url("billing.expense_category",     "admin"), "setup", array("billing.taxrate",     "admin"));
    }

    function getDescription()
    {
      return "Module for generating bills.";
    }

    function project_project_modifier(&$node)
    {
      // Load used attribute/relation classes
      userelation("atkmanytoonerelation");
      userelation("atkonetomanyrelation");

      // This is a listattrib and not a boolattrib, because more options may be added in the future.
      $node->add(new atkListAttribute("billing_expensereg_limit", array("members_only", "all_users"), array(PRJ_EXPENSEREG_MEMBERS_ONLY, PRJ_EXPENSEREG_ALL_USERS), AF_HIDE_ADD|AF_HIDE_LIST), "finance");
      $node->add(new atkManyToOneRelation("billing_type", "billing.projecttype", AF_HIDE_ADD|AF_HIDE_LIST), "finance");
      $node->add(new atkOneToManyRelation("billing_rates", "billing.billing_rate", "project_id", AF_HIDE_LIST), "finance");
      $node->add(new atkOneToManyRelation("income", "billing.income", "project_id", AF_HIDE_LIST|AF_HIDE_ADD), "finance");
      $node->add(new atkBoolAttribute("billing_hoursdeclarable", AF_HIDE_LIST|AF_HIDE_ADD), "finance");

      $node->add(new atkOneToManyRelation("billing_bills", "billing.bill_perproject", "project_id", AF_HIDE_LIST), "finance");
    }

    function employee_employee_modifier(&$node)
    {
      // Load used attribute/relation classes
      useattrib("atknumberattribute");
      userelation("atkonetomanyrelation");

      // Add the billing_declarability attribute
      $attr = new atkNumberAttribute("billing_declarability", AF_HIDE_LIST);
      $attr->setRange(0, 100);
      $node->add($attr, "finance");

      // Add the onetomany relation to rates
      $node->add(new atkOneToManyRelation("rates", "billing.billing_rate", "user_id", AF_HIDE_LIST), "finance");
    }

    function hours_modifier($node)
    {
      // Load used attribute/relation/listener classes
      userelation("atkmanytoonerelation");
      useattrib("atkboolattribute");
      useattrib("atkcurrencyattribute");
      useattrib("billing.hoursTotalAttribute");

      // Add ManyToOne relation to bill_line and boolean billed attribute to hours node
      $hideflags = $this->hasHoursBillingDataViewRights() ? AF_HIDE_ADD|AF_HIDE_EDIT : AF_HIDE;
      $currencysymbol=atktext("currencysymbol","atk","","","",true);

      $node->add(new atkManyToOneRelation("billing_bill_line_id", "billing.bill_line", AF_HIDE));
      $node->add(new atkBoolAttribute("billing_billed", $hideflags));
      $node->add(new atkBoolAttribute("billing_marked", AF_OBLIGATORY|$hideflags));
      $node->add(new atkCurrencyAttribute("billing_rate", $hideflags|AF_HIDE_SEARCH, 10, $currencysymbol, 2));
      $node->add(new hoursTotalAttribute("billing_total", $hideflags|AF_TOTAL|AF_NO_SORT|AF_HIDE_SEARCH));
    }

    function reports_surveyhours_modifier($node)
    {
      $this->hours_modifier($node);
    }

    function timereg_hours_modifier($node)
    {
      // Apply the hours modifiers
      $this->hours_modifier($node);

      // Load used attribute/relation/listener classes
      atkimport("module.billing.ratecalculator");
      atkimport("module.billing.advancebilltimereglistener");

      // Notify the ratecalculator on save/update and the billtimereglistener on save
      $node->addListener(new rateCalculator(array("save", "update")));
      $node->addListener(new advanceBillTimeregListener(array("save")));
    }

    function hasHoursBillingDataViewRights()
    {
      $securityManager = &atkGetSecurityManager();
      return $securityManager->allowed("billing.bill", "admin");
    }

    function getHourSurveyColumns()
    {
      $extracolumns = array();
      if ($this->hasHoursBillingDataViewRights())
      {
        $extracolumns[] = "billing_billed";
        $extracolumns[] = "billing_marked";
        $extracolumns[] = "billing_rate";
        $extracolumns[] = "billing_total";
        //$extracolumns[] = "billing_projecttypeid";
      }
      return $extracolumns;
    }

    function search($expression)
    {
      $billnode = &atkGetNode("billing.bill");
      $bill_remindernode = &atkGetNode("billing.bill_reminder_number");
      return array(
        "bills"=>$this->recLinks($billnode->searchDb($expression), "billing.bill"),
        "bill_reminders"=>$this->recLinks($bill_remindernode->searchDb($expression), "billing.bill_reminder_number"),
      );
    }
  }

?>