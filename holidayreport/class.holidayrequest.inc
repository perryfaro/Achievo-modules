<?php

  userelation("atkmanytoonerelation");
  useattrib("atklistattribute");
  useattrib("atktextattribute");
  useattrib("atkdateattribute");

  /**
   * The node class for holiday_request.
   */
  class holidayrequest extends atkNode
  {

    // How long a requested holiday may be at most
    var $maxholidayduration = 27;

    // How many days a holiday must be requested in advance, depending on
    // it's duration. The values in this array must be given as:
    // <minimal number of days for a rule to apply> => <minimal number of days for a holiday to be requested in advance if not a manager>
    // All items in this array must be sorted DESCENDING on the minimal
    // number of days for a rule to apply.
    var $holidayplanaheaddays = array(
                                  21 => 90,
                                  14 => 42,
                                  7 => 28
                                );

    function holidayrequest($name="holidayrequest", $flags=0)
    {
      $this->atkNode($name, $flags);
      $this->add(new atkAttribute("id", AF_AUTOKEY));

      $additionalflags = $this->userIsManager() ? 0 : AF_HIDE|AF_FORCE_LOAD|AF_READONLY;
      $this->add(new atkManyToOneRelation("userid","employee.employee", AF_OBLIGATORY|$additionalflags));

      // todo: date selection here..

      $this->add(new atkTextAttribute("remark"));

      $this->add(new atkListAttribute("status", array("request", "granted", "denied"), "", AF_OBLIGATORY|AF_HIDE_ADD|AF_HIDE_EDIT));
      $this->add(new atkDateAttribute("fromdate", AF_OBLIGATORY));
      $this->add(new atkDateAttribute("todate", AF_OBLIGATORY));
      $this->add(new atkDateAttribute("entrydate",AF_HIDE_ADD|AF_READONLY|AF_OBLIGATORY));

      $this->setOrder("entrydate DESC");
      $this->setTable("holidayreport_request");
      $this->setDescriptorTemplate("[userid.lastname], [userid.firstname]");
    }

    /**
     * Determines wheter the user has the manager right
     *
     * @return boolean True if the current user is a manager, false if he is not
     */
    function userIsManager()
    {
      global $g_securityManager;
      return ($g_securityManager->allowed("holidayreport.holidayrequest", "manager"));
    }

    /**
     * Set up default values for this node
     *
     * @return array Associative array where the key is the attribute name,
     *               and value is the default value.
     */
    function initial_values()
    {
      $user = &getUser();
      return array("userid" => array("id" => $user["id"]),
                   "status" => "request",
                   "entrydate" => array("year" => date("Y"), "month" => date("m"), "day" => date("d"))
                  );
    }

    /**
     * Validates a holiday record.
     *
     * Validates the holidayduration and days until the start of the holiday
     *
     * @param array $record The holiday record to validate
     * @param String mode The mode for which validation is performed
     * @param array ignoreList The list of attributes that should not be validated
     */
    function validate(&$record, $mode, $ignoreList=array())
    {
      // Calculate the rounded entrydate and set the record's entrydate to today:
      $entrydate = mktime(0, 0, 0, date("m"), date("d"), date("Y"));
      $record["entrydate"] = array("year" => date("Y"), "month" => date("m"), "day" => date("d"));

      // If the user is not a manager, check the holidayduration and days until the start of the holiday
      if (!$this->userIsManager())
      {
        // Calculate the rounded startdate and enddate
        $fromdate = mktime(0, 0, 0, $record["fromdate"]["month"], $record["fromdate"]["day"], $record["fromdate"]["year"]);
        $todate = mktime(0, 0, 0, $record["todate"]["month"], $record["todate"]["day"],  $record["todate"]["year"]);

        // Calculate the holiday duration and the number of days until the holiday starts
        $daysduration = (($todate - $fromdate) / 86400) + 1;
        $daysahead = ($fromdate - $entrydate) / 86400;

        // Reject the holiday if it exceeds the maximum duration:
        if ($daysduration > $this->maxholidayduration)
        {
          triggerError($record, "todate", "error_holiday_duration_exceeds_maximum_duration", sprintf(atktext("error_holiday_duration_exceeds_maximum_duration_of_%d_days", "holidayreport"), $this->maxholidayduration));
        }

        // Check if the days between now and the holidaystart isn't too low for the holidayduration
        else
        {
          $errortriggered = false;
          $previousmaxlength = $this->maxholidayduration;
          foreach ($this->holidayplanaheaddays as $key => $value) {
            if ((!$errortriggered) && ($daysduration >= $key) && ($daysahead < $value))
          	{
              triggerError($record, "fromdate", "error_holiday_request_ahead_time_too_low", sprintf(atktext("error_holidays_between_%d_and_%d_days_long_must_be_requested_%d_days_in_advance", "holidayreport"), $key, $previousmaxlength, $value));
              $errortriggered = true;
          	}
          	$previousmaxlength = $key;
          }
        }
      }

      // Call the parents validate function to perform the default validation on the record
      atkNode::validate($record, $mode, $ignoreList);
    }

    /**
     * Adds a userfilter to the listview when nog in review mode and the user is a manager:
     *
     * @param atkAdminHandler handler atkAdminHandler object used for default handling of the admin action
     * @return unknown Result returned by default atkAdminHandler
     */
    function action_admin(&$handler)
    {
      // Add a userfilter if not in review mode and the user is not a manager:
      if (($this->m_type == "holidayrequest") && (!$this->userIsManager()))
      {

        // Get the userid and apply the userfilter
        $user = getUser();
        $this->addFilter("holidayreport_request.userid=".$user["id"]);

      }

      // Continue to handle the admin action as usual
      return $handler->action_admin();
    }

    /**
     * Return record descriptor (for use in relations etc.)
     * @return String A template containing fieldnames between brackets.
     */
    function descriptor_def()
    {
      // @todo: replace this with proper descriptor fields.
      return "[id]";
    }

    function status_display($record, $mode)
    {
      $attr = &$this->getAttribute("status");
      $result = $attr->display($record, $mode);
      if ($record["status"]=="granted")
        return '<span style="color: #008800;">'.$result.'</span>';
      if ($record["status"]=="denied")
        return '<span style="color: #cc0000;">'.$result.'</span>';
      return $result;
    }

    function recordActions($record, &$actions)
    {
      // Once a request has been processed, we should no longer
      // edit it.
      if ($record["status"]!="request")
      {
        unset($actions["edit"]);
        unset($actions["delete"]);
      }
    }
  }

?>