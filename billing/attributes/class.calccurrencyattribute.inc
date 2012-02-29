<?php

  useattrib("atkcurrencyattribute");
  
  class calcCurrencyAttribute extends atkCurrencyAttribute
  {
    /**
     * The calculation to perform.
     * @access private
     * @var String
     */
    var $m_calculation = NULL;

    /**
     * Constructor
     *
     * @param String $name Unique name of this attribute within the node.
     * @param String $calculation The calculation to perform. Must be a valid php expression.
     * @param int $flags Flags of the attribute.
     */
    function calcCurrencyAttribute($name, $calculation, $flags=0, $size=10,$currencysymbol="",$decimals=2,$decimalseparator="",$thousandsseparator="")
    {
      $this->atkCurrencyAttribute($name, $flags|AF_NO_SORT|AF_HIDE_SEARCH|AF_READONLY,$size,$currencysymbol,$decimals,$decimalseparator,$thousandsseparator);

      $this->m_calculation = $calculation;
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
      atkimport("atk.utils.atkstringparser");
      $parser = &new atkStringParser($this->m_calculation);
      $str = "\$result = ".$parser->parse($record).";";
      eval("\$result = ".$parser->parse($record).";");
      return $result;
    }
    
    /**
     * Returns a displayable string for this value, to be used in HTML pages.
     * 
     * @access public
     * @param array $record The record that holds the value for this attribute
     * @param String $mode The display mode.
     * @return String HTML String
     */
    function display($record, $mode="")
    {
      if($this->m_ownerInstance->m_partial == "attribute.".$this->fieldName().".refresh")
      {
        $record[$this->fieldName()] = $this->load($db, $record);
      }
      return parent::display($record, $mode);
    }    
    
  }
?>