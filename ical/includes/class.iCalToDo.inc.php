<?php

include_once 'class.iCalBase.inc.php';
include_once 'class.iCalAlarm.inc.php';

class iCalToDo extends iCalBase {

  /**
	* end date in iCal format
	*
	* @desc end date in iCal format
	* @var string
	*/
	var $completed;

	/**
	* Timestamp of the end date
	*
	* @desc Timestamp of the end date
	* @var int
	*/
	var $completed_ts;

	/**
	* start date in iCal format
	*
	* @desc start date in iCal format
	* @var string
	*/
	var $startdate;

	/**
	* Timestamp of the start date
	*
	* @desc Timestamp of the start date
	* @var int
	*/
	var $startdate_ts;

	/**
	* The percent completion of a ToDo
	*
	* @desc The percent completion of a ToDo
	* @var int
	*/
	var $percent;

	/**
	* Automaticaly created: md5 value of the start date + Summary
	*
	* @desc Automaticaly created: md5 value of the start date + Summary
	* @var string
	*/
	var $uid;

	/**
	* Duration of the ToDo in minutes
	*
	* @desc Duration of the ToDo in minutes
	* @var int
	*/
	var $duration;

	/**
	* If alarm is set, holds alarm object
	*
	* @desc If alarm is set, holds alarm object
	* @var object
	*/
	var $alarm;
	
	/**
	* Constructor
	*
	* Only job is to set all the variablesnames
	*
	* @desc Constructor
	* @param string $summary  Title for the event
	* @param string $description  Description
	* @param string $location  Location
	* @param int $start  Start time for the event (timestamp)
	* @param int $duration  Duration of the todo in minutes
	* @param int $end  Start time for the event (timestamp)
	* @param int $percent  The percent completion of the ToDo
	* @param int $prio  riority = 0–9
	* @param int $status  Status of the event (0 = TENTATIVE, 1 = CONFIRMED, 2 = CANCELLED)
	* @param int $class  (0 = PRIVATE | 1 = PUBLIC | 2 = CONFIDENTIAL)
	* @param array $organizer  The organizer – use array('Name', 'name@domain.com')
	* @param array $attendees  key = attendee name, value = e-mail, second value = role of the attendee [0 = CHAIR | 1 = REQ | 2 = OPT | 3 =NON] (example: array('Michi' => 'flaimo@gmx.net,1'); )
	* @param array $categories  Array with Strings (example: array('Freetime','Party'))
	* @param int $last_mod  Last modification of the to-to (timestamp)
	* @param array) $alarm  Array with all the alarm information, "''" for no alarm
	* @param int $frequency  frequency: 0 = once, secoundly – yearly = 1–7
	* @param mixed $rec_end  recurrency end: ('' = forever | integer = number of times | timestring = explicit date)
	* @param int $interval  Interval for frequency (every 2,3,4 weeks…)
	* @param string $days  Array with the number of the days the event accures (example: array(0,1,5) = Sunday, Monday, Friday
	* @param string $weekstart  Startday of the Week ( 0 = Sunday – 6 = Saturday)
	* @param string $exept_dates  exeption dates: Array with timestamps of dates that should not be includes in the recurring event
	* @param string $url  optional URL for that event
	* @param string $lang  Language of the strings used in the event (iso code)
	* @param string $uid  Optional UID for the ToDo
	* @uses iCalBase::setLanguage()
	* @uses iCalBase::setOrganizer()
	* @uses setStartDate()
	* @uses setDuration()
	* @uses setCompleted()
	* @uses iCalBase::setLastMod()
	* @uses setPercent()
	* @uses setLocation()
	* @uses iCalBase::setSequence()
	* @uses iCalBase::setCategories()
	* @uses iCalBase::setDescription()
	* @uses setSummary()
	* @uses iCalBase::setPriority()
	* @uses iCalBase::setClass()
	* @uses iCalBase::setAttendees()
	* @uses iCalBase::setStatus()
	* @uses iCalBase::setURL()
	* @uses iCalBase::setFrequency()
	* @uses setRecEnd()
	* @uses iCalBase::setInterval()
	* @uses iCalBase::setDays()
	* @uses iCalBase::setWeekStart()
	* @uses iCalBase::setExeptDates()
	* @uses setAlarm()
	* @uses setUID()
	*/
	function iCalToDo($summary, $description, $location, $start, $duration, $end,
					  $percent, $prio, $status, $class, $organizer, $attendees,
					  $categories, $last_mod, $alarm, $frequency, $rec_end,
					  $interval, $days, $weekstart, $exept_dates, $url, $lang, $uid) {

        parent::iCalBase();
		parent::setLanguage($lang);
		parent::setOrganizer($organizer);
		$this->setStartDate($start);
		$this->setDuration($duration);
		$this->setCompleted($end);
		parent::setLastMod($last_mod);
		$this->setPercent($percent);
		parent::setLocation($location);
		parent::setSequence(0);
		parent::setCategories($categories);
		parent::setDescription($description);
		parent::setSummary($summary);
		parent::setPriority($prio);
		parent::setClass($class);
		parent::setAttendees($attendees);
		parent::setStatus($status);
		parent::setURL($url);
		parent::setFrequency($frequency);
		$this->setRecEnd($rec_end);
		parent::setInterval($interval);
		parent::setDays($days);
		parent::setWeekStart($weekstart);
		parent::setExeptDates($exept_dates);
		$this->setAlarm($alarm);
        $this->setUID($uid);
	} // end constructor


  /**
	* Set $uid variable
	*
	* @desc Set $uid variable
  * @param int $uid
	* @see getUID(), $uid
	*/
	function setUID($uid = 0) {
		if (strlen(trim($uid)) > 0) {
            $this->uid = $uid;
        } else {
            $rawid = $this->startdate . 'plus' .  $this->summary;
            $this->uid = md5($rawid);
        }
	} // end function

	/**
	* Set $startdate_ts variable
	*
	* @desc Set $startdate_ts variable
	* @param int $timestamp
	* @see getStartDateTS()
	* @see $startdate_ts
	*/
	function setStartDateTS($timestamp = 0) {
		if (is_int($timestamp) && $timestamp > 0) {
			$this->startdate_ts = $timestamp;
		} // end if
	} // end function

	/**
	* Set $startdate variable
	*
	* @desc Set $startdate variable
	* @param int $timestamp
	* @see getStartDate()
	* @see $startdate
	* @uses setStartDateTS()
	*/
	function setStartDate($timestamp = 0) {
		$this->setStartDateTS($timestamp);
		$this->startdate = gmdate('Ymd\THi00\Z',$this->startdate_ts);
	} // end function

	/**
	* Set $completed_ts variable
	*
	* @desc Set $completed_ts variable
	* @param int $timestamp
	* @see getCompletedTS()
	* @see $completed_ts
	*/
	function setCompletedTS($timestamp = 0) {
		if (is_int($timestamp) && $timestamp > 0) {
			$this->completed_ts = $timestamp;
		} // end if
	} // end function

	/**
	* Set $completed variable
	*
	* @desc Set $completed variable
	* @param int $completed
	* @see getCompleted()
	* @see $completed
	* @uses setCompletedTS()
	*/
	function setCompleted($timestamp = 0) {
		$this->setCompletedTS($timestamp);
		$this->completed = date('Ymd\THi00\Z',$this->completed_ts);
	} // end function

	/**
	* Set $duration variable
	*
	* @desc Set $duration variable
	* @param int $int
	* @see getPercent()
	* @see $percent
	*/
	function setPercent($int = 0) {
		$this->percent = $int;
	} // end function

	/**
	* Set $duration variable
	*
	* @desc Set $duration variable
	* @param int $int
	* @see getDuration()
	* @see $duration
	*/
	function setDuration($int = 0) {
		$this->duration = $int;
	} // end function

	/**
	* Sets the end for a recurring event
	* (0 = never ending, integer < 4 numbers = number of times, integer >= 4 enddate)
	*
	* @desc Sets the end for a recurring event
	* @param int $freq
	* @see getRecEnd()
	* @see $rec_end
	*/
	function setRecEnd($freq = '') {
		if (strlen(trim($freq)) < 1) {
			$this->rec_end = 0;
		}
		elseif (is_int($freq) && strlen(trim($freq)) < 4) {
			$this->rec_end = $freq;
		} else {
			$this->rec_end = gmdate('Ymd\THi00\Z',$freq);
		} // end if
	} // end function

	/**
	* Set $attendees variable
	*
	* @desc Set $attendees variable
	* @param $attendees
	* @see getAttendees()
	* @see $attendees
	* @uses iCalAlarm
	*/
	function setAlarm($alarm = '') {
		if (is_array($alarm)) {
			$this->alarm = (object) new iCalAlarm($alarm[0], $alarm[1], $alarm[2],
												  $alarm[3], $alarm[4], $alarm[5],
												  $alarm[6], $this->lang);
		} // end if
	} // end function
	
	/**
	* Get $uid variable
	*
	* @desc Get $uid variable
	* @return string $uid
	* @see setUID()
	* @see $uid
	*/
	function &getUID() {
		return $this->uid;
	} // end function

	/**
	* Get $startdate_ts variable
	*
	* @desc Get $startdate_ts variable
	* @return $startdate_ts
	* @see setStartDateTS()
	* @see $startdate_ts
	*/
	function &getStartDateTS() {
		return $this->startdate_ts;
	} // end function

	/**
	* Get $startdate variable
	*
	* @desc Get $startdate variable
	* @return $startdate
	* @see setStartDate()
	* @see $startdate
	*/
	function &getStartDate() {
		return $this->startdate;
	} // end function

	/**
	* Get $completed_ts variable
	*
	* @desc Get $completed_ts variable
	* @return $completed_ts
	* @see setCompletedTS()
	* @see $completed_ts
	*/
	function &getCompletedTS() {
		return $this->completed_ts;
	} // end function

	/**
	* Get $completed variable
	*
	* @desc Get $completed variable
	* @return $completed
	* @see setCompleted()
	* @see $completed
	*/
	function &getCompleted() {
		return $this->completed;
	} // end function

	/**
	* Get $percent variable
	*
	* @desc Get $percent variable
	* @return $percent
	* @see setPercent()
	* @see $percent
	*/
	function &getPercent() {
		return $this->percent;
	} // end function

	/**
	* Get $duration variable
	*
	* @desc Get $duration variable
	* @return $duration
	* @see setDuration()
	* @see $duration
	*/
	function &getDuration() {
		return $this->duration;
	} // end function

	/**
	* Get $rec_end variable
	*
	* @desc Get $rec_end variable
	* @return (mixed) $rec_end
	* @see setRecEnd()
	* @see $rec_end
	*/
	function &getRecEnd() {
		return $this->rec_end;
	} // end function

	/**
	* Get $attendees variable
	*
	* @desc Get $attendees variable
	* @return string $attendees
	* @see setAttendees()
	* @see $attendees
	*/
	function &getAlarm() {
		return ((is_object($this->alarm)) ? $this->alarm : FALSE);
	} // end function
	
}
?>
