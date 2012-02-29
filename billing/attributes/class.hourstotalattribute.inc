<?php

  useattrib("atkAttribute");
  class hoursTotalAttribute extends atkAttribute
  {

    function hourstotalattribute($name, $flags=0)
    {
      $this->atkAttribute($name, $flags);
    }

    function display($record, $mode="")
    {
      $decimalseparator = atktext("decimal_separator","atk","","","",true);
      if ($decimalseparator == "") $decimalseparator='.';
      $thousandsseparator = atktext("thousands_separator","atk","","","",true);
      if ($thousandsseparator == "") $thousandsseparator=',';
      $value = $record[$this->fieldName()];
      return number_format($value,2,$decimalseparator,$thousandsseparator);
    }

    /**
     * Make sure the value is not stored. (always calculated on the fly)
     * @access private
     * @return int
     */
    function storageType() { return NOSTORE; }

    /**
     * Make sure the value is loaded *after* the main record is loaded.
     * @access private
     * @return int
     */
    function loadType() { return POSTLOAD; }

    function load(&$db, $record)
    {
      $time = atkArrayNvl($record, "time");
      $billing_rate = atkArrayNvl($record, "billing_rate");
      $result = is_null($billing_rate) ? null : round($time * $billing_rate / 60, 2);
      return $result;
    }
  }

?>
