<?php

class iCalBase {
  
	/**
	* Detailed information for the module
	*
	* @desc Detailed information for the module
	*/
	var $description;

	/**
	* iso code language (en, de,…)
	*
	* @desc iso code language (en, de,…)
	*/
	var $lang;

	/**
	* If not empty, contains a Link for that module
	*
	* @desc If not empty, contains a Link for that module
	*/
	var $url;

	/**
	* Headline for the module (mostly displayed in your cal programm)
	*
	* @desc Headline for the module
	*/
	var $summary;

	/**
	* String of days for the recurring module (example: “SU,MO”)
	*
	* @desc String of days for the recurring module
	*/
	var $rec_days;

	/**
	* Short string symbolizing the startday of the week
	*
	* @desc Short string symbolizing the startday of the week
	*/
	var $week_start = 1;

	/**
	* Location of the module
	*
	* @desc Location of the module
	*/
	var $location;

	/**
	* String with the categories asigned to the module
	*
	* @desc String with the categories asigned to the module
	*/
	var $categories;

	/**
	* last modification date in iCal format
	*
	* @desc last modification date in iCal format
	*/
	var $last_mod;
	
	/**
	* Organizer of the module; $organizer[0] = Name, $organizer[1] = e-mail
	*
	* @desc Organizer of the module; $organizer[0] = Name, $organizer[1] = e-mail
	*/
	var $organizer = array('vCalEvent class', 'http://www.achievo.org');

	/**
	* List of short strings symbolizing the weekdays
	*
	* @desc List of short strings symbolizing the weekdays
	*/
	var $shortDaynames = array('SU','MO','TU','WE','TH','FR','SA');

	/**
	* If the method is REQUEST, all attendees are listet in the file
	*
	* key = attendee name, value = e-mail, second value = role of the attendee
	* [0 = CHAIR | 1 = REQ | 2 = OPT | 3 =NON] (example: array('Michi' => 'flaimo@gmx.net,1'); )
	*
	* @desc If the method is REQUEST, all attendees are listet in the file
	*/
	var $attendees = array();

	/**
	* Array with the categories asigned to the module (example:
	* array('Freetime','Party'))
	*
	* @desc Array with the categories asigned to the module
	*/
	var $categories_array;

	/**
	* Exeptions dates for the recurring module (Array of timestamps)
	*
	* @desc Exeptions dates for the recurring module
	*/
	var $exept_dates;
	
	/**
	* set to 0
	*
	* @desc set to 0
	*/
	var $sequence;

	/**
	* 0 = once, 1-7 = secoundly - yearly
	*
	* @desc 0 = once, 1-7 = secoundly - yearly
	*/
	var $frequency;

	/**
	* If not empty, contains the status of the module
	* (0 = TENTATIVE, 1 = CONFIRMED, 2 = CANCELLED)
	*
	* @desc If not empty, contains the status of the module
	*/
	var $status;

	/**
	* interval of the recurring date (example: every 2,3,4 weeks)
	*
	* @desc
	*/
	var $interval = 1;

	/**
	* PRIVATE (0) or PUBLIC (1) or CONFIDENTIAL (2)
	*
	* @desc PRIVATE (0) or PUBLIC (1) or CONFIDENTIAL (2)
	*/
	var $class;

	/**
	* set to 5 (value between 0 and 9)
	*
	* @desc set to 5 (value between 0 and 9)
	*/
	var $priority;

	/**
	* Timestamp of the last modification
	*
	* @desc Timestamp of the last modification
	*/
	var $last_mod_ts;
	

	function iCalBase() {

	} 

	/**
	* Set $startdate variable
	*
	* @desc Set $startdate variable
	* @param string $isocode
	* @see getStartDate()
	* @see $startdate
	* @uses isValidLanguageCode()
	*/
	function setLanguage($isocode = '') {
		$this->lang = (($this->isValidLanguageCode($isocode) == TRUE) ? ';LANGUAGE=' . $isocode : '');
	} // end function

	/**
	* Set $description variable
	*
	* @desc Set $description variable
	* @param string $description
	* @see getDescription()
	* @see $description
	*/
	function setDescription($description) {
		$this->description = $description;
	} // end function

	/**
	* Set $organizer variable
	*
	* @desc Set $organizer variable
	* @param $organizer
	* @see getOrganizerName()
	* @see getOrganizerMail()
	* @see $organizer
	*/
	function setOrganizer($organizer = '') {
		if (is_array($organizer)) {
			$this->organizer = $organizer;
		} // end if
	} // end function

	/**
	* Set $url variable
	*
	* @desc Set $url variable
	* @param string $url
	* @see getURL()
	* @see $url
	*/
	function setURL($url = '') {
		$this->url = $url;
	} // end function

	/**
	* Set $summary variable
	*
	* @desc Set $summary variable
	* @param string $summary
	* @see getSummary()
	* @see $summary
	*/
	function setSummary($summary = '') {
		$this->summary = $summary;
	} // end function

	/**
	* Set $sequence variable
	*
	* @desc Set $sequence variable
	* @param int $int
	* @see getSequence()
	* @see $sequence
	*/
	function setSequence($int = 0) {
		$this->sequence = $int;
	} // end function

	/**
	* Sets a string with weekdays of the recurring module
	*
	* @desc Sets a string with weekdays of the recurring event
	* @param $recdays integers
	* @see getDays()
	* @see $rec_days
	*/
	function setDays($recdays = '') {
		$this->rec_days = '';
		if (!is_array($recdays) || count($recdays) == 0) {
			$this->rec_days = $this->shortDaynames[1];
		} else {
			if (count($recdays) > 1) {
				$recdays = array_values(array_unique($recdays));
			} // end if
			foreach ($recdays as $day) {
				if (array_key_exists($day, $this->shortDaynames)) {
					$this->rec_days .= $this->shortDaynames[$day] . ',';
				} // end if
			} // end foreach
			$this->rec_days = substr($this->rec_days,0,strlen($this->rec_days)-1);
		} // end if
	} // end function

	/**
	* Sets the starting day for the week (0 = Sunday)
	*
	* @desc Sets the starting day for the week (0 = Sunday)
	* @param int $weekstart  0–6
	* @see getWeekStart()
	* @see $week_start
	*/
	function setWeekStart($weekstart = 1) {
		if (is_int($weekstart) && preg_match('(^([0-6]{1})$)', $weekstart)) {
			$this->week_start = $weekstart;
		} // end if
	} // end function

	/**
	* Set $attendees variable
	*
	* @desc Set $attendees variable
	* @param $attendees
	* @see getAttendees()
	* @see $attendees
	*/
	function setAttendees($attendees = '') {
		if (is_array($attendees)) {
			$this->attendees = $attendees;
		} // end if
	} // end function

	/**
	* Set $location variable
	*
	* @desc Set $location variable
	* @param string $location
	* @see getLocation()
	* @see $location
	*/
	function setLocation($location = '') {
		if (strlen(trim($location)) > 0) {
			$this->location = $location;
		} // end if
	} // end function

	/**
	* Set $categories_array variable
	*
	* @desc Set $categories_array variable
	* @param string $categories
	* @see getCategoriesArray()
	* @see $categories_array
	*/
	function setCategoriesArray($categories = '') {
		$this->categories_array = $categories;
	} // end function

	/**
	* Set $categories variable
	*
	* @desc Set $categories variable
	* @param string $categories
	* @see getCategories()
	* @see $categories
	*/
	function setCategories($categories = '') {
		$this->setCategoriesArray($categories);
		$this->categories = implode(',',$categories);
	} // end function

	/**
	* Sets the frequency of a recurring event
	*
	* @desc Sets the frequency of a recurring event
	* @param int $int  Integer 0–7
	* @see getFrequency()
	* @see $frequencies
	*/
	function setFrequency($int = 0) {
		$this->frequency = $int;
	} // end function

	/**
	* Set $status variable
	*
	* @desc Set $status variable
	* @param int $status
	* @see getStatus()
	* @see $status
	*/
	function setStatus($status = 1) {
		$this->status = $status;
	} // end function

	/**
	* Sets the interval for a recurring event (2 = every 2 [days|weeks|years|…])
	*
	* @desc Sets the interval for a recurring event
	* @param int $interval
	* @see getInterval()
	* @see $interval
	*/
	function setInterval($interval = 1) {
			$this->interval = $interval;
	} // end function

	/**
	* Sets an array of formated exeptions dates based on an array with timestamps
	*
	* @desc Sets an array of formated exeptions dates based on an array with timestamps
	* @param $exeptdates
	* @see getExeptDates()
	* @see $exept_dates
	*/
	function setExeptDates($exeptdates = '') {
		if (!is_array($exeptdates)) {
			$this->exept_dates = array();
		} else {
			foreach ($exeptdates as $timestamp) {
				$this->exept_dates[] = gmdate('Ymd\THi00\Z',$timestamp);
			} // end foreach
		} // end if
	} // end function

	/**
	* Set $class variable
	*
	* @desc Set $class variable
	* @param int $int
	* @see getClass()
	* @see $class
	*/
	function setClass($int = 0) {
		$this->class = $int;
	} // end function

	/**
	* Set $priority variable
	*
	* @desc Set $priority variable
	* @param int $int
	* @see getPriority()
	* @see $priority
	*/
	function setPriority($int = 5) {
		$this->priority = ((is_int($int) && preg_match('(^([0-9]{1})$)', $int)) ? $int : 5);
	} // end function

	/**
	* Set $last_mod_ts variable
	*
	* @desc Set $last_mod_ts variable
	* @param int $timestamp
	* @see getLastModTS()
	* @see $last_mod_ts
	*/
	function setLastModTS($timestamp = 0) {
		if (is_int($timestamp) && $timestamp > 0) {
			$this->last_mod_ts = $timestamp;
		} // end if
	} // end function

	/**
	* Set $last_mod variable
	*
	* @desc Set $last_mod variable
	* @param int $last_mod
	* @see getLastMod()
	* @see $last_mod
	*/
	function setLastMod($timestamp = 0) {
		$this->setLastModTS($timestamp);
		$this->last_mod = gmdate('Ymd\THi00\Z',$this->last_mod_ts);
	} // end function
	

	/**
	* Checks if a given string is a valid iso-language-code
	*
	* @desc Checks if a given string is a valid iso-language-code
	* @param string $code  String that should validated
	* @return boolean isvalid  If string is valid or not
	*/
	function isValidLanguageCode($code = '') {
		return  ((preg_match('(^([a-zA-Z]{2})((_|-)[a-zA-Z]{2})?$)',trim($code)) > 0) ? TRUE : FALSE);
	} // end function

	/**
	* Get $startdate variable
	*
	* @desc Get $startdate variable
	* @return $startdate
	* @see setStartDate()
	* @see $startdate
	*/
	function &getLanguage() {
		return $this->lang;
	} // end function

	/**
	* Get $description variable
	*
	* @desc Get $description variable
	* @return string $description
	* @see setDescription()
	* @see $description
	*/
	function &getDescription() {
		return $this->description;
	} // end function

	/**
	* Get name from $organizer variable
	*
	* @desc Get name from $organizer variable
	* @return $organizer
	* @see setOrganizer()
	* @see getOrganizerMail()
	* @see $organizer
	*/
	function &getOrganizerName() {
		return $this->organizer[0];
	} // end function

	/**
	* Get e-mail from $organizer variable
	*
	* @desc Get e-mail from $organizer variable
	* @return $organizer
	* @see setOrganizer()
	* @see getOrganizerName()
	* @see $organizer
	*/
	function &getOrganizerMail() {
		return $this->organizer[1];
	} // end function

	/**
	* Get $url variable
	*
	* @desc Get $url variable
	* @return string $url
	* @see setURL()
	* @see $url
	*/
	function &getURL() {
		return $this->url;
	} // end function

	/**
	* Get $summary variable
	*
	* @desc Get $summary variable
	* @return string $summary
	* @see setSummary()
	* @see $summary
	*/
	function &getSummary() {
		return $this->summary;
	} // end function

	/**
	* Get $sequence variable
	*
	* @desc Get $sequence variable
	* @return $sequence
	* @see setSequence()
	* @see $sequence
	*/
	function &getSequence() {
		return $this->sequence;
	} // end function

	/**
	* Returns a string with recurring days
	*
	* @desc Returns a string with recurring days
	* @return string $rec_days
	* @see setDays()
	* @see $rec_days
	*/
	function &getDays() {
		return $this->rec_days;
	} // end function

	/**
	* Get the string from the $week_start variable
	*
	* @desc Get the string from the $week_start variable
	* @return string $shortDaynames
	* @see setWeekStart()
	* @see $week_start
	*/
	function &getWeekStart() {
		return ((array_key_exists($this->week_start, $this->shortDaynames)) ? $this->shortDaynames[$this->week_start] : $this->shortDaynames[1]);
	} // end function

	/**
	* Get $attendees variable
	*
	* @desc Get $attendees variable
	* @return string $attendees
	* @see setAttendees()
	* @see $attendees
	*/
	function &getAttendees() {
		return $this->attendees;
	} // end function

	/**
	* Get $location variable
	*
	* @desc Get $location variable
	* @return string $location
	* @see setLocation()
	* @see $location
	*/
	function &getLocation() {
		return $this->location;
	} // end function

	/**
	* Get $categories_array variable
	*
	* @desc Get $categories_array variable
	* @return $categories_array
	* @see setCategoriesArray()
	* @see $categories_array
	*/
	function &getCategoriesArray() {
		return $this->categories_array;
	} // end function

	/**
	* Get $categories variable
	*
	* @desc Get $categories variable
	* @return string $categories
	* @see setCategories()
	* @see $categories
	*/
	function &getCategories() {
		return $this->categories;
	} // end function

	/**
	* Get $frequency variable
	*
	* @desc Get $frequency variable
	* @return string $frequencies
	* @see setFrequency()
	* @see $frequencies
	*/
	function &getFrequency() {
		return $this->frequency;
	} // end function

	/**
	* Get $status variable
	*
	* @desc Get $status variable
	* @return string $statuscode
	* @see setStatus()
	* @see $status
	*/
	function &getStatus() {
		return $this->status;
	} // end function

	/**
	* Get $interval variable
	*
	* @desc Get $interval variable
	* @return $interval
	* @see setInterval()
	* @see $interval
	*/
	function &getInterval() {
		return $this->interval;
	} // end function

	/**
	* Returns a string with exeptiondates
	*
	* @desc Returns a string with exeptiondates
	* @return string $return
	* @see setExeptDates()
	* @see $exept_dates
	*/
	function &getExeptDates() {
		$return = '';
		foreach ($this->exept_dates as $date) {
			$return .= $date . ',';
		} // end foreach
		$return = substr($return,0,strlen($return)-1);
		return $return;
	} // end function

	/**
	* Get $class variable
	*
	* @desc Get $class variable
	* @return string $class
	* @see setClass()
	* @see $class
	*/
	function &getClass() {
		return $this->class;
	} // end function

	/**
	* Get $priority variable
	*
	* @desc Get $priority variable
	* @return string $priority
	* @see setPriority()
	* @see $priority
	*/
	function &getPriority() {
		return $this->priority;
	} // end function

	/**
	* Get $last_mod_ts variable
	*
	* @desc Get $last_mod_ts variable
	* @return $last_mod_ts
	* @see setLastModTS()
	* @see $last_mod_ts
	*/
	function &getLastModTS() {
		return $this->last_mod_ts;
	} // end function

	/**
	* Get $last_mod variable
	*
	* @desc Get $last_mod variable
	* @return $last_mod
	* @see setLastMod()
	* @see $last_mod
	*/
	function &getLastMod() {
		return $this->last_mod;
	} // end function
	
}
?>