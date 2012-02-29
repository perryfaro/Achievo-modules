<?php
  useattrib("atkCurrencyAttribute");

  class totalBillPriceAttribute extends atkCurrencyAttribute
  {
    function totalBillPriceAttribute($name,$flags=0)
    {
      $flags |= AF_HIDE_ADD|AF_READONLY_EDIT;
      $this->atkCurrencyAttribute($name,$flags);
    }

    /**
     * Make sure the value is loaded *after* the main record is loaded.
     * @access private
     * @return int
     */
    function loadType()
    {
      return POSTLOAD;
    }

    /**
     * The load method performs the calculation.
     *
     * @access private
     * @param atkDb $db
     * @param array $record
     * @return String result of the calculation
     */
    function load(&$db, $record)
    {
      $invoice_id = $record['id'];
      $query = "SELECT SUM(fixed_amount) AS total FROM billing_bill_line WHERE bill_id = '$invoice_id'";

      $result = $db->getRows($query);
      return $result[0]['total'];
    }

    /**
     * Make sure the value is not stored. (always calculated on the fly)
     * @access private
     * @return int
     */
    function storageType()
    {
      return NOSTORE;
    }
  }

?>