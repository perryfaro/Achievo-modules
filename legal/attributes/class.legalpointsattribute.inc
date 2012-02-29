<?php

  // Load used classes
  useattrib("atknumberattribute");

  /**
   * legalPointsAttribute class
   */
  class legalPointsAttribute extends atkNumberAttribute
  {

    /**
     * legalPointsAttribute constructor
     *
     * @param $name  Name of the attribute
     * @param $flags Flags for this attribute
     * @return legalPointsAttribute
     */
    function legalPointsAttribute($name, $flags=0)
    {
      $this->atkNumberAttribute($name, $flags, 10, 0); // base class constructor
    }

    function fetchValue($postvars)
    {
      if (is_array($postvars))
      {
        $result = atkNumberAttribute::standardiseSeparator(atkArrayNvl($postvars, $this->fieldName(), "")) * 180;
      }
      else
      {
        $result = null;
      }
      return $result;
    }

    function display($record, $mode="")
    {
      //var_dump($record[$this->m_name]);
      if (isset($record[$this->m_name]) && !is_null($record[$this->m_name]))
      {
        // Default display
        $defaultdisplay = number_format($record[$this->m_name] / 180, 2) . " " . atktext("points", "legal");

        // Add a time display
        useattrib("atkdurationattribute");
        $durationattribute = new atkDurationAttribute("dummy");
        $timedisplay = $durationattribute->display(array("dummy"=>($record[$this->m_name]))) . " " . atktext("hours","timereg");

        // Compose the final display and return it
        return sprintf("%s (%s)", $defaultdisplay, $timedisplay);
      }
      return "";
    }

    function edit($record="", $fieldprefix="")
    {
      $dummyrecord = array($this->m_name => number_format($record[$this->m_name] / 180, 2));
      return parent::edit($dummyrecord, $fieldprefix);
    }

  }

?>