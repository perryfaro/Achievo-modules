<?php

  /**
   * The atkDateAttribute class offers an advanced date widget for date fields.
   * @author  Lineke Willems <lineke@achievo.org>
   *
   * @version $Revision: 3751 $
   *
   * $Id: class.weekdateattribute.inc 3751 2006-04-16 01:21:17Z guido $
   */

  class weekDateAttribute extends atkAttribute
  {
    /**
     * Default Constructor, sets up the atkDateAttribute
     * The API of this method has changed, but is has been made
     * backwards compatible with existing modules!
     *
     * @param $name the attribute's name
     * @param $format_edit the format the edit/add box(es) will look like
     * @param $format_view the format in which dates are listed
     * @param $min the minimum date that has to be selected (0 is unlimited)
     * @param $max the maximum date that may be selected (0 is unlimited)
     * @param $flags the attribute's flags
     *
     * @see atkAttribute
     */
    function weekDateAttribute($name, $flags=0)
    {
       /* base class constructor */
      $this->atkAttribute($name, $flags|AF_HIDE_SEARCH); // FIXME.. until the searchCondition() and
                                                         // search() methods are fixed,
                                                         // you can't search on dates..
    }

    /**
     * Returns a piece of html code that can be used in a form to edit this
     * attribute's value. (Month will be a dropdownbox, year and day text fields)
     * @params $record Array with 3 fields (year, month, day)
     * @return Piece a of HTML Code
     */
    function edit($record="", $fieldprefix="", $mode="")
    {
      $fieldname = $fieldprefix.$this->fieldName();
      $day  = $record[$this->fieldName()]["day"];
      $month = $record[$this->fieldName()]["month"];
      $year = $record[$this->fieldName()]["year"];

      $weeknumber = date("W", mktime(12,12,0,$month,$day,$year));

      $result = '<select id="'.$fieldname.'" name="'.$fieldname.'[week]">';
      for ($i = 1; $i < 54; $i++)
      {
        $result .= '<option value="'.$i.'"'.($i == $weeknumber ? ' selected' : '').'>'.atktext("week")." ".$i.'</option>';
      }
      $result .= '</select> ';

      /* normal input box */
      $result .= '<input type="text" id="'.$fieldname.'[year]" name="'.$fieldname.'[year]" size="4" maxlength="4" value="'.$year.'">';

      /* return result */
      return $result;
    }


    // TODO FIXME: to be useable in general nodes, this attribute should get value2db, db2value and fetchValue methods.
  }
?>