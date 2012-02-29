<?php

  userelation("atkmanytoonerelation");
  useattrib("atkdateattribute");
  useattrib("atknumberattribute");
  useattrib("atkdummyattribute");
  useattrib("atktimeattribute");
  useattrib("atklistattribute");
  useattrib("billing.billingprojectphase");

  define("RATECALCULATION_NORMAL", 0);
  define("RATECALCULATION_MODIFICATION", 1);

  class billing_rate extends atkNode
  {

    /**
     * rate class constructor
     */
    function billing_rate()
    {
      $this->atkNode("billing_rate", NF_ADD_LINK);

      $this->add(new atkAttribute("id" ,AF_AUTOKEY));
      $this->add(new atkAttribute("description", AF_OBLIGATORY|AF_SEARCHABLE, 100));
      $this->add(new atkDateAttribute("startdate", AF_OBLIGATORY));
      $this->add(new atkDateAttribute("enddate"));

      if (!atkconfig("use_duration", true))
      {
        $this->add(new atkTimeAttribute("starttime","0","23", array(0,15,30,45),"12", AF_OBLIGATORY));
        $this->add(new atkTimeAttribute("endtime",  "0","23", array(0,15,30,45),"12", AF_OBLIGATORY));
      }

      $calculationtypeattr = &$this->add(new atkListAttribute("calculationtype", array("normal", "modification"), array(RATECALCULATION_NORMAL, RATECALCULATION_MODIFICATION), AF_OBLIGATORY));
      $calculationtypeattr->addOnChangeHandler(sprintf('if (newvalue==0) {writeLayer("ratelabel","%s");} else {writeLayer("ratelabel","%s");}', atktext("rate", "billing"), atktext("factor", "billing")));

      $rateattr = &$this->add(new atkNumberAttribute("rate", AF_OBLIGATORY|AF_SEARCHABLE, 10, 2));
      $rateattr->setLabel('<span id="ratelabel">'.atktext("rate", "billing").'/'.atktext("factor", "billing").'</span>');

      $this->add(new atkNumberAttribute("priority",AF_OBLIGATORY));

      $this->add(new atkManytoOneRelation("user_id","employee.employee", AF_RELATION_AUTOLINK));
      $this->add(new atkManytoOneRelation("functionlevel_id","employee.functionlevel", AF_RELATION_AUTOLINK));
      $this->add(new atkManytoOneRelation("project_id","project.project", AF_RELATION_AUTOLINK));
      $this->add(new billingprojectphase("phaseid",AF_RELATION_AUTOLINK));

      // Determine wheter the activity attribute should be displayed or not.
      $displayactivityattribute = $this->getActivityCount() != 1;
      $this->add(new atkManytoOneRelation("activity_id","project.activity", AF_RELATION_AUTOLINK|($displayactivityattribute ? 0 : AF_HIDE)));

      $this->add(new atkManytoOneRelation("customer_id","organization.organization", AF_RELATION_AUTOLINK));

      $this->add(new atkManyToOneRelation("projecttype_id", "billing.projecttype"));
      $this->add(new atkListAttribute("ratetype", array("salary", "commercial"),array(0,1), AF_OBLIGATORY));

      $this->setTable("billing_rate");
      $this->setOrder("priority, rate");
    }

    function calculationtype_edit($record="", $fieldprefix="", $mode="")
    {
      $page = &$this->getPage();
      $page->register_script(moduleDir("billing")."javascript/ratecalculationtype.js");
      $page->register_loadscript('calculationtype_onChange(document.entryform.calculationtype);');

      $calculationattr = &$this->getAttribute("calculationtype");
      return $calculationattr->edit($record, $fieldprefix, $mode);
    }

    /**
     * Determine the number of available activities
     *
     * @return integer Number of activities in the database
     */
    function getActivityCount()
    {
      $db = &atkGetDb();
      $result = $db->getrows("SELECT count(*) as activitycount FROM activity");
      return $result[0]['activitycount'];
    }

    /**
     * Defines the default values for several fields to be used when editing a record
     *
     * @return array Fieldname=>Defaultvalue pairs
     */
    function initial_values()
  	{
  	  // Setup the initial values array
  	  $values = array(
        "priority" => 10,
        "enddate" => array("year" => NULL, "day" => NULL, "month" => NULL),
        "ratetype" => 1
      );

      // Set an initial activityid if there's exactly one activity
      if ($this->getActivityCount() == 1) {
        $db = &atkGetDb();
        $result = $db->getrows("SELECT id FROM activity");
        $values["activity_id"] = $result[0]["id"];
      }

      // Return the initial values
      return $values;
  	}

    function descriptor_def()
    {
      return "[description] ([rate])";
    }

    function getRateByTimeEntry($hours_id)
    {
      // Setup the sql query template to be used when selecting rates corresponding to the given timeregistration
      $sqltpl = "SELECT DISTINCT
                   billing_rate.id,
                   billing_rate.rate
                 FROM (billing_rate, hours)
                 LEFT JOIN phase ON hours.phaseid = phase.id
                 LEFT JOIN project ON project.id = phase.projectid
                 LEFT JOIN activity ON hours.activityid = activity.id
                 LEFT JOIN person EMPLOYEE ON hours.userid = EMPLOYEE.id
                 LEFT JOIN project_person ON project_person.projectid = phase.projectid
                 LEFT JOIN person CONTACT ON CONTACT.id = project_person.personid
                 WHERE
                   hours.id = ".$hours_id."
                   AND (billing_rate.startdate <= hours.activitydate)
                   AND (billing_rate.enddate='' OR billing_rate.enddate IS NULL OR billing_rate.enddate >= hours.activitydate)
                   AND (billing_rate.startdate <= hours.activitydate)
                   AND (billing_rate.enddate='' OR billing_rate.enddate IS NULL OR billing_rate.enddate >= hours.activitydate)
                   AND (billing_rate.user_id='' OR billing_rate.user_id IS NULL OR billing_rate.user_id = hours.userid)
                   AND (billing_rate.activity_id ='' OR billing_rate.activity_id IS NULL OR billing_rate.activity_id = hours.activityid)
                   AND (billing_rate.project_id = '' OR billing_rate.project_id IS NULL OR billing_rate.project_id = phase.projectid)
                   AND (billing_rate.customer_id = '' OR billing_rate.customer_id IS NULL OR billing_rate.customer_id = CONTACT.company)
                   AND (billing_rate.projecttype_id = '' OR billing_rate.projecttype_id IS NULL OR billing_rate.projecttype_id = project.billing_type)
                   AND (billing_rate.calculationtype='%d')
                 ORDER BY
                   billing_rate.priority DESC, billing_rate.rate DESC";

      return $this->getRate($sqltpl);
    }

    function getRateByParams($projectid=false,$phaseid=false,$userid=false,$from_date=false,$to_date=false,$functionlevelid=false,$billingtypeid=false,$customerid=false)
    {
      // build filter based on the params passed to this method
      $filter = array();

      // project filter
      if ($projectid !== false && is_numeric($projectid))
        $filter[] = "(billing_rate.project_id = '$projectid' OR billing_rate.project_id IS NULL OR billing_rate.project_id = '0')";

      // phaseid filter
      if ($phaseid !== false && is_numeric($phaseid))
        $filter[] = "(billing_rate.phaseid = '$phaseid' OR billing_rate.phaseid IS NULL OR billing_rate.phaseid = '0')";

      // user filter
      if ($userid !== false && is_numeric($userid))
        $filter[] = "(billing_rate.user_id = '$userid' OR billing_rate.user_id IS NULL OR billing_rate.user_id = '0')";

      // from-date filter
      if ($from_date !== false)
        $filter[] = "('$from_date' <= billing_rate.enddate OR billing_rate.enddate IS NULL OR billing_rate.enddate = '0000-00-00')";

      // to-date filter
      if ($to_date !== false)
        $filter[] = "('$to_date' >= billing_rate.startdate)";

      // functionlevel filter
      if ($functionlevelid !== false)
        $filter[] = "(billing_rate.functionlevel_id = '$functionlevelid' OR billing_rate.functionlevel_id IS NULL OR billing_rate.functionlevel_id = '0')";

      // billingtype filter
      if ($billingtypeid !== false)
        $filter[] = "(billing_rate.projecttype_id = '$billingtypeid' OR billing_rate.projecttype_id IS NULL OR billing_rate.projecttype_id = '0')";

      // customerid filter
      if ($customerid !== false)
        $filter[] = "(billing_rate.customer_id = '$customerid' OR billing_rate.customer_id IS NULL OR billing_rate.customer_id = '0')";

      // build filter whereclause
      $whereclause = " AND ";
      if (sizeof($filter) > 0)
      {
        $whereclause .= implode(" AND ", $filter);
      }

      // monsterquery
      $sqltpl = "SELECT
                   billing_rate.rate
                 FROM
                   billing_rate
                 WHERE billing_rate.calculationtype='%d'
                 $whereclause
                 ORDER BY
                   billing_rate.priority DESC, billing_rate.rate DESC";

      return $this->getRate($sqltpl);
    }

    function getRate($sqltpl)
    {
      // Get a database reference
      $db = &$this->getDb();

      // Load the normal rate to be applied to this hours registration
      $normalrecs = $db->getrows(sprintf($sqltpl, RATECALCULATION_NORMAL));

      // If no normal rates found, set the final rate to 0
      if (count($normalrecs) == 0)
      {
        $finalrate = 0;
      }

      // If one or more rate found, search for modifications and calculate the final rate
      else
      {
        // Use the first normal rate as basis for the finalrate
        $finalrate = $normalrecs[0]["rate"];

        // Find any modifications that have to be made
        $modificationrecs = $db->getrows(sprintf($sqltpl, RATECALCULATION_MODIFICATION));

        // Use all modifications
        foreach ($modificationrecs as $modificationrec)
        {
          $finalrate *= $modificationrec["rate"];
        }
      }

      // Return the final rate
      return $finalrate;
    }
  }

?>