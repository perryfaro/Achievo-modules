<?php
  /**
   * atkUserStoryStatusAttribute class
   *
   * @package achievo_modules
   * @subpackage xp
   *
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2005 Ibuildings.nl BV
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision: 3861 $
   * $Id: class.atkuserstorystatusattribute.inc 3861 2006-01-23 16:20:23Z dennis $
   */

  /**
   * UserStoryStatusAttribute class for displaying the latest progression-step and its date
   *
   * @author guido <guido@ibuildings.nl>
   * @package achievo_modules
   * @subpackage xp
   */
  class atkUserStoryStatusAttribute extends atkDummyAttribute
  {

    /**
     * atkUserStoryStatusAttribute Constructor
     *
     * @param String $name The name of the attribute
     * @param int $flags The flags for this attribute
     */
    function atkUserStoryStatusAttribute($name, $flags = 0)
    {
      // Call the base class constructor:
      $this->atkDummyAttribute($name, "", $flags);
    }

    /**
     * Formats the given date into a localized string
     *
     * @param $value Array with month, day and year elements
     * @return string Parsed string
     */
    function formatDate($value)
    {
      // Return empty string if invalid date array given
      if (empty($value["month"]) or empty($value["day"]) or empty( $value["year"])) return "";

      // Construct a date object
      $date = adodb_getdate(adodb_mktime(0, 0, 0, $value["month"], $value["day"], $value["year"]));

      // Try to get the local date format and use "F j Y" in case of failure
      $txt_date_format_view = atktext("date_format_view", "atk", "", "", "", true);
      $format = (!empty($txt_date_format_view)) ? $txt_date_format_view : "F j Y";

      // Make M (short month) and F (full month name) inrecognisable to the adodb_data format function
      // so later on they can be replaced by localized strings
      $format = str_replace("M", "%-%", $format);
      $format = str_replace("F", "%=%", $format);

      // Get the formatted date string
      $str_date = adodb_date($format, $date[0]);

      // Invoke a static array to cache the localized month and shortmonth strings
      static $langcache = array();

      // Determine strings that identify the month and shortmonth localized strings
      $month = $date['month'];
      $shortmonth = substr(strtolower($date["month"]), 0, 3);

      // Check for cached localized month and load it if not present
      if(!isset($langcache[$month]))
      {
        $langcache[$month]= text(strtolower($month), "", "atk");
      }

      // Check for cached localized shortmonth and load it if not present
      if(!isset($langcache[$shortmonth]))
      {
        $langcache[$shortmonth] = text($shortmonth, "", "atk");
      }

      // Replace the self-made month and shortmonth values by the localized strings
      $str_date = str_replace("%-%", $langcache[$shortmonth], $str_date);
      $str_date = str_replace("%=%", $langcache[$month], $str_date);

      // Return the formatted date string
      return $str_date;
    }

    /**
     * Determines the date of the last progression-step of a user story record
     *
     * Display the last progression-step and corresponding date for a user story record
     * The last progression step will be determined depending on the dates present in a
     * record. From first shown to last shown these dates will be evaluated:
     * implemented_on, planned_on, accepted_on. If no date is present for any of these
     * steps, then the text "Not processed yet" is shown.
     *
     * @param array $record Array with attributes of a record
     * @return string Date of last progression-step
     */
    function display($record)
    {
      // If date in implemented_on column set, display "Implemented on [date]"
      if (!empty($record["implemented_on"]))
      {
      	return atktext("implemented_on", "xp") . " " . $this->formatDate($record["implemented_on"]);
      }

      // If date in planned_on column set, display "Planned on [date]"
      else if (!empty($record["planned_on"]))
      {
      	return atktext("planned_on", "xp")     . " " . $this->formatDate($record["planned_on"]);
      }

      // If date in accepted_on column set, display "Accepted on [date]"
      else if (!empty($record["accepted_on"]))
      {
      	return atktext("accepted_on", "xp")    . " " . $this->formatDate($record["accepted_on"]);
      }

      // If date in submitted_on column set, display "Accepted on [date]"
      else if (!empty($record["submitted_on"]))
      {
      	return atktext("submitted_on", "xp")    . " " . $this->formatDate($record["submitted_on"]);
      }

      // If none of the columns released_on, implemented_on, planned_on and accepted_on set, display "Not processed yet"
      else
      {
      	return atktext("not_processed", "xp");
      }
    }

  }
?>