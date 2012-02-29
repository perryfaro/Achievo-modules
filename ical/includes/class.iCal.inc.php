<?php

include_once 'class.iCalEvent.inc.php';
include_once 'class.iCalToDo.inc.php';

/**
* Create a iCalendar file for download
*
* $iCal = new iCal('', 0, '');
* $iCal->addEvent(…);
* $iCal->addToDo(…);
* …
* $iCal->outputFile('ics'); // output file as isc (xcs and rdf possible)
*
* Date/Time is stored with an absolute “z” value, which means that the
* calendar programm should import the time 1:1 not regarding timezones and
* Daylight Saving Time. MS Outlook imports “z” dates wrong, so you have to
* “correct” the dates BEFORE you add a new event.
* Also if you have an event series and not a single event, you have to use
* “File >> Import” in Outlook to import the whole series and not just the
* first date.
*
* @author Michael Wimmer <flaimo 'at' gmx 'dot' net>
*
*/
class iCal {
  
	/**
	* Array with all the iCalEvent objects
	*
	* @desc Array with all the iCalEvent objects
	*/
	var $icalevents = array();

	/**
	* Array with all the iCalToDo objects
	*
	* @desc Array with all the iCalToDo objects
	*/
	var $icaltodos = array();

	/**
	* Array with all the freebusy objects
	*
	* @desc Array with all the freebusy objects
	*/
	var $icalfbs = array();

	/**
	* Array with all the journal objects
	*
	* @desc Array with all the journal objects
	*/
	var $icaljournals = array();

	/**
	* Programm ID for the File
	*
	* @desc Programm ID for the File
	*/
	var $prodid = '-//achievo.org//iCal Class MIMEDIR//EN';

	/**
	* Output string to be written in the iCal file
	*
	* @desc Output string to be written in the iCal file
	*/
	var $output;

	/**
	* Format of the output (ics, xcs, rdf)
	*
	* @desc Format of the output (ics, xcs, rdf)
	*/
	var $output_format;

	/**
	* Download directory where iCal file will be saved
	*
	* @desc Download directory where iCal file will be saved
	*/
	var $download_dir = 'icaldownload';

	/**
	* Filename for the iCal file to be saved
	*
	* @desc Filename for the iCal file to be saved
	*/
	var $events_filename;

	/**
	* Time the entry was created (iCal format)
	*
	* @desc Time the entry was created (iCal format)
	*/
	var $ical_timestamp;
	
	/**
	* ID number for the event array
	*
	* @desc ID number for the event array
	*/
	var $eventid = 0;

	/**
	* ID number for the todo array
	*
	* @desc ID number for the todo array
	*/
	var $todoid = 0;

	/**
	* Method: PUBLISH (1) or REQUEST (0)
	*
	* @desc Method: PUBLISH (1) or REQUEST (0)
	*/
	var $method = 1;
	

	/**
	* Constructor
	*
	* Only job is to set all the variablesnames
	*
	* @desc Constructor
	* @param string $prodid  ID code for the iCal file (see setProdID)
	* @param int $method  PUBLISH (1) or REQUEST (0)
	* @param string $downloaddir
	* @return void
	* @uses setiCalTimestamp()
	* @uses setProdID()
	* @uses setMethod()
	* @uses checkClass()
	*/
	function iCal($prodid = '', $method = 1, $downloaddir = '') {
		$this->setiCalTimestamp();
		$this->setProdID($prodid);
        $this->setMethod($method);
		if (strlen(trim($downloaddir)) > 0) {
			$this->download_dir = $downloaddir;
		} // end if
		$this->events_filename  = time() . '.ics';
	} 

	/**
	* Encodes a string for QUOTE-PRINTABLE
	*
	* @desc Encodes a string for QUOTE-PRINTABLE
	* @param string $quotprint  String to be encoded
	* @return string  Encodes string
	*/
	function quotedPrintableEncode($quotprint = '') {
		$quotprint = str_replace('\r\n',chr(13) . chr(10),$quotprint);
		$quotprint = str_replace('\n',chr(13) . chr(10),$quotprint);
		$quotprint = preg_replace("~([\x01-\x1F\x3D\x7F-\xFF])~e", "sprintf('=%02X', ord('\\1'))", $quotprint);
		$quotprint = str_replace('\=0D=0A','=0D=0A',$quotprint);
		return $quotprint;
	} // end function

	/**
	* Checks if the download directory exists, else trys to create it
	*
	* @desc Checks if the download directory exists, else trys to create it
	* @return boolean
	*/
	function checkDownloadDir() {
		if (!is_dir($this->download_dir)) {
			return ((!mkdir($this->download_dir, 0700)) ? FALSE : TRUE);
		} else {
			return  TRUE;
		} // end if
	} // end function


	/**
	* Returns string with the status of an attendee
	*
	* @desc Returns string with the status of an attendee
	* @param int $role
	* @return string $roles Status
	*/
	function getAttendeeRole($role = 2) {
		$roles = array('CHAIR','REQ-PARTICIPANT','OPT-PARTICIPANT','NON-PARTICIPANT');
		return ((array_key_exists($role, $roles)) ? $roles[$role] : $roles[2]);
	} // end function

	/**
	* Set $prodid variable
	*
	* @desc Set $prodid variable
	* @param string $prodid
	* @see getProdID()
	* @see $prodid
	*/
	function setProdID($prodid = '') {
		if (strlen(trim($prodid)) > 0) {
			$this->prodid = $prodid;
		} // end if
	} // end function

	/**
	* Set $method variable
	*
	* @desc Set $method variable
	* @param int $method
	* @see getMethod()
	* @see $method
	*/
	function setMethod($method = 1) {
		if (is_int($method) && preg_match('(^([0-1]{1})$)', $method)) {
			$this->method = $method;
		} // end if
	} // end function

	/**
	* Set $ical_timestamp variable
	*
	* @desc Set $ical_timestamp variable
	* @see getiCalTimestamp()
	* @see $ical_timestamp
	*/
	function setiCalTimestamp() {
		$this->ical_timestamp = gmdate('Ymd\THi00\Z',time());
	} // end function
	

	/**
	* Get $prodid variable
	*
	* @desc Get $prodid variable
	* @return string $prodid
	* @see setProdID()
	* @see $prodid
	*/
	function getProdID() {
		return $this->prodid;
	} // end function

	/**
	* Get $method variable
	*
	* @desc Get $method variable
	* @return string $methods
	* @see setMethod()
	* @see $methods
	*/
	function getMethod() {
		$methods = array('REQUEST','PUBLISH');
		return ((array_key_exists($this->method, $methods)) ? $methods[$this->method] : $methods[1]);
	} // end function

	/**
	* Get $ical_timestamp variable
	*
	* @desc Get $ical_timestamp variable
	* @return string $ical_timestamp
	* @see setiCalTimestamp()
	* @see $ical_timestamp
	*/
	function &getiCalTimestamp() {
		return $this->ical_timestamp;
	} // end function

	/**
	* Get class name
	*
	* @desc Get class name
	* @param int $int
	* @return string $classes
	*/
	function &getClassName($int = 0) {
		$classes = array('PRIVATE','PUBLIC','CONFIDENTIAL');
		return ((array_key_exists($int, $classes)) ? $classes[$int] : $classes[0]);
	} // end function

	/**
	* Get status name
	*
	* @desc Get status name
	* @param int $int
	* @return string $statuscode
	*/
	function &getStatusName($int = 0) {
		$statuscode = array('TENTATIVE','CONFIRMED','CANCELLED');
		return ((array_key_exists($int, $statuscode)) ? $statuscode[$int] : $statuscode[0]);
	} // end function

	/**
	* Get frequency name
	*
	* @desc Get frequency name
	* @return string $frequencies
	* @see setFrequency(), $frequencies
	*/
	function &getFrequencyName($int = 0) {
		$frequencies = array('ONCE','SECONDLY','MINUTELY','HOURLY','DAILY','WEEKLY','MONTHLY','YEARLY');
		return ((array_key_exists($int, $frequencies)) ? $frequencies[$int] : $frequencies[0]);
	} // end function
	
	/**
	* Adds a new Event Object to the Events Array
	*
	* @desc Adds a new Event Object to the Events Array
	* @param array $organizer  The organizer - use array('Name', 'name@domain.com')
	* @param int $start  Start time for the event (timestamp; if you want an allday event the startdate has to start at 00:00:00)
	* @param int $end  Start time for the event (timestamp or write 'allday' for an allday event)
	* @param string $location  Location
	* @param int $transp  Transparancy (0 = OPAQUE | 1 = TRANSPARENT)
	* @param array $categories  Array with Strings (example: array('Freetime','Party'))
	* @param string $description  Description
	* @param string $summary  Title for the event
	* @param int $class  (0 = PRIVATE | 1 = PUBLIC | 2 = CONFIDENTIAL)
	* @param array $attendees  key = attendee name, value = e-mail, second value = role of the attendee [0 = CHAIR | 1 = REQ | 2 = OPT | 3 =NON] (example: array('Michi' => 'flaimo@gmx.net,1'); )
	* @param int $prio  riority = 0-9
	* @param int $frequency  frequency: 0 = once, secoundly – yearly = 1–7
	* @param mixed $rec_end  recurrency end: ('' = forever | integer = number of times | timestring = explicit date)
	* @param int $interval  Interval for frequency (every 2,3,4 weeks…)
	* @param string $days  Array with the number of the days the event accures (example: array(0,1,5) = Sunday, Monday, Friday
	* @param string $weekstart  Startday of the Week ( 0 = Sunday – 6 = Saturday)
	* @param string $exept_dates  exeption dates: Array with timestamps of dates that should not be includes in the recurring event
	* @param int $alarm  Array with all the alarm information, “''” for no alarm
	* @param int $status  Status of the event (0 = TENTATIVE, 1 = CONFIRMED, 2 = CANCELLED)
	* @param string $url  optional URL for that event
	* @param string $language  Language of the strings used in the event (iso code)
	* @param string $uid  Optional UID for the event
	* @see getEvent()
	* @uses iCalEvent
	*/
	function addEvent($organizer, $start, $end, $location, $transp, $categories,
					  $description, $summary, $class, $attendees, $prio,
					  $frequency, $rec_end, $interval, $days, $weekstart,
					  $exept_dates, $alarm, $status, $url, $language, $uid) {

		$event = (object) new iCalEvent($organizer, $start, $end, $location,
										$transp, $categories, $description,
										$summary, $class, $attendees, $prio,
										$frequency, $rec_end, $interval, $days,
										$weekstart, $exept_dates, $alarm,
										$status, $url, $language, $uid);

		$this->icalevents[$this->eventid++] = $event;
		unset($event);
	} // end function

	/**
	* Adds a new ToDo Object to the ToDo Array
	*
	* @desc Adds a new ToDo Object to the ToDo Array
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
	* @param array $alarm  Array with all the alarm information, “''” for no alarm
	* @param int $frequency  frequency: 0 = once, secoundly – yearly = 1–7
	* @param mixed $rec_end  recurrency end: ('' = forever | integer = number of times | timestring = explicit date)
	* @param int $interval  Interval for frequency (every 2,3,4 weeks…)
	* @param string $days  Array with the number of the days the event accures (example: array(0,1,5) = Sunday, Monday, Friday
	* @param string $weekstart  Startday of the Week ( 0 = Sunday – 6 = Saturday)
	* @param string $exept_dates  exeption dates: Array with timestamps of dates that should not be includes in the recurring event
	* @param string $url  optional URL for that event
	* @param string $lang  Language of the strings used in the event (iso code)
	* @param string $uid  Optional UID for the ToDo
	* @uses iCalToDo
	*/
	function addToDo($summary, $description, $location, $start, $duration, $end,
					 $percent, $prio, $status, $class, $organizer, $attendees,
					 $categories, $last_mod, $alarm, $frequency, $rec_end,
					 $interval, $days, $weekstart, $exept_dates, $url, $lang, $uid) {

		$todo = (object) new iCalToDo($summary, $description, $location, $start,
									  $duration, $end, $percent, $prio, $status,
									  $class, $organizer, $attendees, $categories,
									  $last_mod, $alarm, $frequency, $rec_end,
									  $interval, $days, $weekstart, $exept_dates,
									  $url, $lang, $uid);

		$this->icaltodos[$this->todoid++] = $todo;
		unset($todo);
	} // end function

	/**
	* Fetches an event from the array by the ID number
	*
	* @desc Fetches an event from the array by the ID number
	* @see addEvent()
	* @see iCalEvent::iCalEvent()
	*/
	function &getEvent($id = 0) {
		if (count($this->icalevents) < 1) {
			return 'No Dates found';
		} elseif (is_int($id) && array_key_exists($id, $this->icalevents)) {
			return (object) $this->icalevents[$id];
		} else {
			return (object) $this->icalevents[0];
		} // end if
	} // end function

	/**
	* Fetches an event from the array by the ID number
	*
	* @desc Fetches an event from the array by the ID number
	* @see addToDo()
	* @see iCalToDo::iCalToDo()
	*/
	function &getToDo($id = 0) {
		if (count($this->icaltodos) < 1) {
			return 'No ToDos found';
		} elseif (is_int($id) && array_key_exists($id, $this->icaltodos)) {
			return (object) $this->icaltodos[$id];
		} else {
			return (object) $this->icaltodos[0];
		} // end if
	} // end function


	/**
	* Returns the array with the iCal Event Objects
	*
	* @desc Returns the array with the iCal Event Objects
	* @return $icalevents
	* @see addEvent()
	* @see getEvent()
	*/
	function &getEvents() {
		return $this->icalevents;
	} // end function

	/**
	* Returns the array with the iCal ToDo Objects
	*
	* @desc Returns the array with the iCal ToDo Objects
	* @return array $icaltodos
	* @see addToDo()
	* @see getToDo()
	*/
	function &getToDos() {
		return $this->icaltodos;
	} // end function

	/**
	* Returns the number of created events
	*
	* @desc Returns the number of created events
	* @return $icalevents
	* @uses $icalevents
	*/
	function countEvents() {
		return count($this->icalevents);
	} // end function

	/**
	* Returns the number of created ToDos
	*
	* @desc Returns the number of created ToDos
	* @return int $icaltodos
	* @uses $icaltodos
	*/
	function countToDos() {
		return count($this->icaltodos);
	} // end function

	/**
	* Deletes an event-object from the event-array
	*
	* @desc Deletes an event-object from the event-array
	* @see addEvent()
	*/
	function deleteEvent($id = 0) {
		if (array_key_exists($id, $this->icalevents)) {
			$this->icalevents[$id] = '';
			$this->icalevents = array_filter($this->icalevents, 'strlen');
			return  TRUE;
		} else {
			return  FALSE;
		} // end if
	} // end function

	/**
	* Deletes an todo-object from the todo-array
	*
	* @desc Deletes an todo-object from the todo-array
	* @see addToDo()
	*/
	function deleteToDo($id = 0) {
		if (array_key_exists($id, $this->icaltodos)) {
			$this->icaltodos[$id] = '';
			$this->icaltodos = array_filter($this->icaltodos, 'strlen');
			return  TRUE;
		} else {
			return  FALSE;
		} // end if
	} // end function

	

	/**
	* Returns the number of iCal-Objects which would be returned when generating the iCal file
	*
	* @desc Returns the number of iCal-Objects which would be returned when generating the iCal file
	* @return int
	* @uses countEvents
	* @uses countToDos()
	* @uses countFreeBusys()
	* @uses countJournals()
	*/
	function countiCalObjects() {
		return ($this->countEvents() + $this->countToDos());
	} // end function

	/**
	* Generates the string for the alarm
	*
	* @desc Generates the string for the alarm
	* @param object $alarm
	* @param string $format  ics | xcs
	* @see generateOutput()
	* @uses generateAttendeesOutput()
	* @uses iCalAlarm::getTrigger()
	* @uses iCalAlarm::getAction()
	* @uses iCalAlarm::getLanguage()
	* @uses iCalAlarm::getDescription()
	* @uses iCalAlarm::getSummary()
	* @uses iCalAlarm::getRepeat()
	* @uses iCalAlarm::getDuration()
	* @uses iCalAlarm::getAttendees()
	*/
	function generateAlarmOutput($alarm, $format = 'ics') {
		$output = '';
		if (is_object($alarm)) {
			if ($format === 'ics') {
				if ($alarm->getTrigger() > 0) {
					$output .= "BEGIN:VALARM\r\n";
					$output .= "ACTION:" . $alarm->getAction() . "\r\n";
					$output .= "TRIGGER:-PT" . $alarm->getTrigger() . "M\r\n";

					if ($alarm->getAction() == 'DISPLAY' || $alarm->getAction() == 'EMAIL') {
						$output .= "DESCRIPTION" . $alarm->getLanguage() . ":" . $alarm->getDescription() . "\r\n";
					} // end if

					if ($alarm->getAction() == 'DISPLAY' || $alarm->getAction() == 'EMAIL') {
						$output .= "SUMMARY" . $alarm->getLanguage() . ":" . $alarm->getSummary() . "\r\n";
					} // end if

					if ($alarm->getDuration() != 0 && $alarm->getRepeat() != 0) {
						$output .= "DURATION:" . $alarm->getDuration() . "\r\n";
						$output .= "REPEAT:" . $alarm->getRepeat() . "\r\n";
					} // end if

					$output .= $this->generateAttendeesOutput($alarm->getAttendees(), $format);
					$output .= "END:VALARM\r\n";
				}
			} elseif ($format === 'xcs') {
				if ($alarm->getTrigger() > 0) {
					$output .= '<valarm>';
					$output .= '<action>' . $alarm->getAction() . '</action>';
					$output .= '<trigger>-PT' . $alarm->getTrigger() . '</trigger>';

					if ($alarm->getAction() == 'DISPLAY' || $alarm->getAction() == 'EMAIL') {
						$output .= '<description>' . $alarm->getDescription() . '</description>';
					} // end if

					if ($alarm->getAction() == 'DISPLAY' || $alarm->getAction() == 'EMAIL') {
						$output .= '<summary>' . $alarm->getSummary() . '</summary>';
					} // end if

					if ($alarm->getDuration() != 0 && $alarm->getRepeat() != 0) {
						$output .= '<duration>' . $alarm->getDuration() . '</duration>';
						$output .= '<repeat>' . $alarm->getRepeat() . '</repeat>';
					} // end if

					$output .= $this->generateAttendeesOutput($alarm->getAttendees(), $format);
					$output .= '</valarm>';
				} // end if
			} // end if
		} // end if
		return $output;
	} // end function

	/**
	* Generates the string for the attendees
	*
	* @desc Generates the string for the attendees
	* @param array $attendees
	* @param string $format  ics | xcs
	* @see generateOutput()
	* @uses getAttendeeRole()
	*/
	function generateAttendeesOutput($attendees, $format = 'ics') {
		$output = '';
		if ($this->method == 0 && count($attendees) > 0) {
			if ($format === 'ics') {
				if (count($attendees) > 0) {
					foreach ($attendees as $name => $data) {
						$values = explode(',',$data);
						$email = $values[0];
						if (strlen(trim($email)) > 5) {
							$role = $values[1];
							$output .= "ATTENDEE;ROLE=" . $this->getAttendeeRole($role) . ";CN=" . $name . ":MAILTO:" . $email . "\r\n";
						} // end if
					} // end foreach
				} // end if
			} elseif ($format === 'vcs') {
				if (count($attendees) > 0) {
					foreach ($attendees as $name => $data) {
						$values = explode(',',$data);
						$email = $values[0];
						if (strlen(trim($email)) > 5) {
							$role = $values[1];
							$output .= "ATTENDEE;ROLE=" . $this->getAttendeeRole($role) . ";STATUS=CONFIRMED;ENCODING=QUOTED-PRINTABLE: " . $this->quotedPrintableEncode($name). "\r\n";
						} // end if
					} // end foreach
				}
			} elseif ($format === 'xcs') {
				if (count($attendees) > 0) {
					foreach ($attendees as $name => $data) {
						$values = explode(',',$data);
						$email = $values[0];
						if (strlen(trim($email)) > 5) {
							$role = $values[1];
							$output .= '<attendee cn="' . $name . '" role="' . $this->getAttendeeRole($role) . '">MAILTO:' . $email . '</attendee>';
						} // end if
					} // end foreach
				} // end if
			} // end if
		} // end if
		return $output;
	} // end function

	/**
	* Generates the string to be written in the file later on
	*
	* you can choose between ics, xcs or rdf format.
	* only ics has been tested; the other two are not, or are not
	* finished coded yet
	*
	* @desc Generates the string to be written in the file later on
	* @param string $format  ics | xcs | rdf
	* @see getOutput()
	* @see writeFile()
	* @uses iCalEvent
	* @uses iCalToDo
	* @uses iCalFreeBusy
	* @uses iCalJournal
	* @uses quotedPrintableEncode()
	* @uses getClassName()
	* @uses getStatusName()
	* @uses getFrequencyName()
	*/
	function generateOutput($format = 'ics') {
		if (!function_exists(isEmpty)) {
	  function &isEmpty(&$variable) {
           return  ((strlen(trim($variable)) > 0) ? FALSE : TRUE);
        } }

        $this->output_format = $format;
		if ($this->output_format == 'ics') {
			$this->output  = "BEGIN:VCALENDAR\r\n";
			$this->output .= "PRODID:" . $this->prodid . "\r\n";
			$this->output .= "VERSION:2.0\r\n";
			$this->output .= "METHOD:" . $this->getMethod() . "\r\n";
			$eventkeys = array_keys($this->icalevents);
			foreach ($eventkeys as $id) {
				$this->output .= "BEGIN:VEVENT\r\n";
				$event =& $this->icalevents[$id];
				$this->output .= $this->generateAttendeesOutput($event->getAttendees(), $format);
				if (!isEmpty($event->getOrganizerMail())) {
					$name = '';
					if (!isEmpty($event->getOrganizerName())) {
						$name = ";CN=" . $event->getOrganizerName();
					} // end if
					$this->output .= "ORGANIZER" . $name . ":MAILTO:" . $event->getOrganizerMail() . "\r\n";
				} // end if
				$this->output .= "DTSTART:" . $event->getStartDate() . "\r\n";
				if (strlen(trim($event->getEndDate())) > 0) {
					$this->output .= "DTEND:" . $event->getEndDate() . "\r\n";
				}

				if ($event->getFrequency() > 0) {
					$this->output .= "RRULE:FREQ=" . $this->getFrequencyName($event->getFrequency());
					if (is_string($event->getRecEnd())) {
						$this->output .= ";UNTIL=" . $event->getRecEnd();
					} elseif (is_int($event->getRecEnd())) {
						$this->output .= ";COUNT=" . $event->getRecEnd();
					} // end if
					$this->output .= ";INTERVAL=" . $event->getInterval() . ";BYDAY=" . $event->getDays() . ";WKST=" . $event->getWeekStart() . "\r\n";
				} // end if
				if (!isEmpty($event->getExeptDates())) {
					$this->output .= "EXDATE:" . $event->getExeptDates() . "\r\n";
				} // end if
				if (!isEmpty($event->getLocation())) {
					$this->output .= "LOCATION" . $event->getLanguage() . ";ENCODING=QUOTED-PRINTABLE:" . $this->quotedPrintableEncode($event->getLocation()) . "\r\n";
				} // end if
				$this->output .= "TRANSP:" . $event->getTransp() . "\r\n";
				$this->output .= "SEQUENCE:" . $event->getSequence() . "\r\n";
				$this->output .= "UID:" . $event->getUID() . "\r\n";
				$this->output .= "DTSTAMP:" . $this->ical_timestamp . "\r\n";
				if (!isEmpty($event->getCategories())) {
					$this->output .= "CATEGORIES" . $event->getLanguage() . ";ENCODING=QUOTED-PRINTABLE:" . $this->quotedPrintableEncode($event->getCategories()) . "\r\n";
				} // end if
				if (!isEmpty($event->getDescription())) {
					$this->output .= "DESCRIPTION" . $event->getLanguage() . ";ENCODING=QUOTED-PRINTABLE:" . str_replace('\n', '=0D=0A=',str_replace('\r', '=0D=0A=', $this->quotedPrintableEncode($event->getDescription()))) . "\r\n";
				} // end if
				$this->output .= "SUMMARY" . $event->getLanguage() . ";ENCODING=QUOTED-PRINTABLE:" . $this->quotedPrintableEncode($event->getSummary()) . "\r\n";
				$this->output .= "PRIORITY:" . $event->getPriority() . "\r\n";
				$this->output .= "CLASS:" . $this->getClassName($event->getClass()) . "\r\n";
				if (!isEmpty($event->getURL())) {
					$this->output .= "URL:" . $event->getURL() . "\r\n";
				} // end if
				if (!isEmpty($event->getStatus())) {
					$this->output .= "STATUS:" . $this->getStatusName($event->getStatus()) . "\r\n";
				} // end if
				$this->output .= $this->generateAlarmOutput($event->getAlarm(), $format);
				$this->output .= "END:VEVENT\r\n";
			} // end foreach
			$todokeys = array_keys($this->icaltodos);
			foreach ($todokeys as $id) {
				$this->output .= "BEGIN:VTODO\r\n";
				$todo =& $this->icaltodos[$id];
				$this->output .= $this->generateAttendeesOutput($todo->getAttendees(), $format);
				if (!isEmpty($todo->getOrganizerMail())) {
					$name = '';
					if (!isEmpty($todo->getOrganizerName())) {
						$name = ";CN=" . $todo->getOrganizerName();
					} // end if
					$this->output .= "ORGANIZER" . $name . ":MAILTO:" . $todo->getOrganizerMail() . "\r\n";
				} // end if
				$this->output .= "SEQUENCE:" . $todo->getSequence() . "\r\n";
				$this->output .= "UID:" . $todo->getUID() . "\r\n";
				$this->output .= "DTSTAMP:" . $this->ical_timestamp . "\r\n";
				if (!isEmpty($todo->getCategories())) {
					$this->output .= "CATEGORIES" . $todo->getLanguage() . ";ENCODING=QUOTED-PRINTABLE:" . $this->quotedPrintableEncode($todo->getCategories()) . "\r\n";
				} // end if
				if (!isEmpty($todo->getDescription())) {
					$this->output .= "DESCRIPTION" . $todo->getLanguage() . ";ENCODING=QUOTED-PRINTABLE:" . str_replace('\n', '=0D=0A=',str_replace('\r', '=0D=0A=', $this->quotedPrintableEncode($todo->getDescription()))) . "\r\n";
				} // end if
				$this->output .= "SUMMARY" . $todo->getLanguage() . ";ENCODING=QUOTED-PRINTABLE:" . $this->quotedPrintableEncode($todo->getSummary()) . "\r\n";
				$this->output .= "PRIORITY:" . $todo->getPriority() . "\r\n";
				$this->output .= "CLASS:" . $this->getClassName($todo->getClass()) . "\r\n";
				if (!isEmpty($todo->getLocation())) {
					$this->output .= "LOCATION" . $todo->getLanguage() . ";ENCODING=QUOTED-PRINTABLE:" . $this->quotedPrintableEncode($todo->getLocation()) . "\r\n";
				} // end if
				if (!isEmpty($todo->getURL())) {
					$this->output .= "URL:" . $todo->getURL() . "\r\n";
				} // end if
				if (!isEmpty($todo->getStatus())) {
					$this->output .= "STATUS:" . $this->getStatusName($todo->getStatus()) . "\r\n";
				} // end if
				if (!isEmpty($todo->getPercent()) && $todo->getPercent() > 0) {
					$this->output .= "PERCENT-COMPLETE:" . $todo->getPercent() . "\r\n";
				} // end if
				if (!isEmpty($todo->getDuration()) && $todo->getDuration() > 0) {
					$this->output .= "DURATION:PT" . $todo->getDuration() . "M\r\n";
				} // end if
				if (!isEmpty($todo->getLastMod())) {
					$this->output .= "LAST-MODIFIED:" . $todo->getLastMod() . "\r\n";
				} // end if
				if (!isEmpty($todo->getStartDate())) {
					$this->output .= "DTSTART:" . $todo->getStartDate() . "\r\n";
				} // end if
				if (!isEmpty($todo->getCompleted())) {
					$this->output .= "COMPLETED:" . $todo->getCompleted() . "\r\n";
				} // end if
				if ($todo->getFrequency() != 'ONCE') {
					$this->output .= "RRULE:FREQ=" . $todo->getFrequency();
					if (is_string($todo->getRecEnd())) {
						$this->output .= ";UNTIL=" . $todo->getRecEnd();
					} elseif (is_int($todo->getRecEnd())) {
						$this->output .= ";COUNT=" . $todo->getRecEnd();
					} // end if
					$this->output .= ";INTERVAL=" . $todo->getInterval() . ";BYDAY=" . $todo->getDays() . ";WKST=" . $todo->getWeekStart() . "\r\n";
				} // end if
				if (!isEmpty($todo->getExeptDates())) {
					$this->output .= "EXDATE:" . $todo->getExeptDates() . "\r\n";
				} // end if
				$this->output .= $this->generateAlarmOutput($todo->getAlarm(), $format);
				$this->output .= "END:VTODO\r\n";
			} // end foreach
			$journalkeys = array_keys($this->icaljournals);
			foreach ($journalkeys as $id) {
				$this->output .= "BEGIN:VJOURNAL\r\n";
				$journal =& $this->icaljournals[$id];
				$this->output .= $this->generateAttendeesOutput($journal->getAttendees(), $format);
				if (!isEmpty($journal->getOrganizerMail())) {
					$name = '';
					if (!isEmpty($journal->getOrganizerName())) {
						$name = ";CN=" . $journal->getOrganizerName();
					} // end if
					$this->output .= "ORGANIZER" . $name . ":MAILTO:" . $journal->getOrganizerMail() . "\r\n";
				} // end if
				$this->output .= "SEQUENCE:" . $journal->getSequence() . "\r\n";
				$this->output .= "UID:" . $journal->getUID() . "\r\n";
				$this->output .= "DTSTAMP:" . $this->ical_timestamp . "\r\n";
				if (!isEmpty($journal->getCategories())) {
					$this->output .= "CATEGORIES" . $journal->getLanguage() . ";ENCODING=QUOTED-PRINTABLE:" . $this->quotedPrintableEncode($journal->getCategories()) . "\r\n";
				} // end if
				if (!isEmpty($journal->getDescription())) {
					$this->output .= "DESCRIPTION" . $journal->getLanguage() . ";ENCODING=QUOTED-PRINTABLE:" . str_replace('\n', '=0D=0A=',str_replace('\r', '=0D=0A=', $this->quotedPrintableEncode($journal->getDescription()))) . "\r\n";
				} // end if
				$this->output .= "SUMMARY" . $journal->getLanguage() . ";ENCODING=QUOTED-PRINTABLE:" . $this->quotedPrintableEncode($journal->getSummary()) . "\r\n";
				$this->output .= "CLASS:" . $this->getClassName($journal->getClass()) . "\r\n";
				if (!isEmpty($journal->getURL())) {
					$this->output .= "URL:" . $journal->getURL() . "\r\n";
				} // end if
				if (!isEmpty($journal->getStatus())) {
					$this->output .= "STATUS:" . $this->getStatusName($journal->getStatus()) . "\r\n";
				} // end if
				if (!isEmpty($journal->getLastMod())) {
					$this->output .= "LAST-MODIFIED:" . $journal->getLastMod() . "\r\n";
				} // end if
				if (!isEmpty($journal->getStartDate())) {
					$this->output .= "DTSTART:" . $journal->getStartDate() . "\r\n";
				} // end if
				if (!isEmpty($journal->getCreated())) {
					$this->output .= "CREATED:" . $journal->getCreated() . "\r\n";
				} // end if
				if ($journal->getFrequency() > 0) {
					$this->output .= "RRULE:FREQ=" . $this->getFrequencyName($journal->getFrequency());
					if (is_string($journal->getRecEnd())) {
						$this->output .= ";UNTIL=" . $journal->getRecEnd();
					} elseif (is_int($journal->getRecEnd())) {
						$this->output .= ";COUNT=" . $journal->getRecEnd();
					} // end if
					$this->output .= ";INTERVAL=" . $journal->getInterval() . ";BYDAY=" . $journal->getDays() . ";WKST=" . $journal->getWeekStart() . "\r\n";
				} // end if
				if (!isEmpty($journal->getExeptDates())) {
					$this->output .= "EXDATE:" . $journal->getExeptDates() . "\r\n";
				} // end if
				$this->output .= "END:VJOURNAL\r\n";
			} // end foreach
			$fbkeys = array_keys($this->icalfbs);
			foreach ($fbkeys as $id) {
				$this->output .= "BEGIN:VFREEBUSY\r\n";
				$fb =& $this->icalfbs[$id];
				$this->output .= $this->generateAttendeesOutput($fb->getAttendees(), $format);
				if (!isEmpty($fb->getOrganizerMail())) {
					$name = '';
					if (!isEmpty($fb->getOrganizerName())) {
						$name = ";CN=" . $fb->getOrganizerName();
					} // end if
					$this->output .= "ORGANIZER" . $name . ":MAILTO:" . $fb->getOrganizerMail() . "\r\n";
				} // end if
				$this->output .= "UID:" . $fb->getUID() . "\r\n";
				$this->output .= "DTSTAMP:" . $this->ical_timestamp . "\r\n";
				if (!isEmpty($fb->getURL())) {
					$this->output .= "URL:" . $fb->getURL() . "\r\n";
				} // end if
				if (!isEmpty($fb->getDuration()) && $fb->getDuration() > 0) {
					$this->output .= "DURATION:PT" . $fb->getDuration() . "M\r\n";
				} // end if
				if (!isEmpty($fb->getStartDate())) {
					$this->output .= "DTSTART:" . $fb->getStartDate() . "\r\n";
				} // end if
				if (!isEmpty($fb->getEndDate())) {
					$this->output .= "DTEND:" . $fb->getEndDate() . "\r\n";
				} // end if
				if (count($fb->getFBTimes()) > 0) {
					foreach ($fb->getFBTimes() as $timestamp => $data) {
						$values = explode(',',$data);
						$this->output .= "FREEBUSY;FBTYPE=" . $values[1] . ":" . $timestamp . "/" . $values[0] . "\r\n";
					} // end foreach
					unset($values);
				} // end if
				$this->output .= "END:VFREEBUSY\r\n";
			} // end foreach
			$this->output .= "END:VCALENDAR\r\n";
		} // end if ics
		elseif ($this->output_format == 'vcs') {
			$this->output  = "BEGIN:VCALENDAR\r\n";
			$this->output .= "PRODID:" . $this->prodid . "\r\n";
			$this->output .= "VERSION:1.0\r\n";
			$this->output .= "METHOD:" . $this->getMethod() . "\r\n";
			$eventkeys = array_keys($this->icalevents);
			foreach ($eventkeys as $id) {
				$this->output .= "BEGIN:VEVENT\r\n";
				$event =& $this->icalevents[$id];
				$this->output .= "UID:" . $event->getUID() . "\r\n";
				$this->output .= "SUMMARY;ENCODING=QUOTED-PRINTABLE:" . $this->quotedPrintableEncode($event->getSummary()) . "\r\n";
				if (!isEmpty($event->getDescription())) {
					$this->output .= "DESCRIPTION;ENCODING=QUOTED-PRINTABLE:" . str_replace('\n', '=0D=0A=',str_replace('\r', '=0D=0A=', $this->quotedPrintableEncode($event->getDescription()))) . "\r\n";
				}
				$this->output .= "CLASS:" . $this->getClassName($event->getClass()) . "\r\n";
				if (!isEmpty($event->getOrganizerName())) {
					$this->output .= "ATTENDEE;ROLE=OWNER;STATUS=CONFIRMED;ENCODING=QUOTED-PRINTABLE: " . $this->quotedPrintableEncode($event->getOrganizerName()) . " \r\n";
				} // end if
				$this->output .= $this->generateAttendeesOutput($event->getAttendees(), $format);
				$this->output .= "DTSTART:" . $event->getStartDate() . "\r\n";
				if (strlen(trim($event->getEndDate())) > 0) {
					$this->output .= "DTEND:" . $event->getEndDate() . "\r\n";
				}
				$this->output .= "END:VEVENT\r\n";
			} // end foreach
			$this->output .= "END:VCALENDAR\r\n";
		} // end if vcs
		elseif ($this->output_format == 'xcs') {
			$this->output  = '<?xml version="1.0" encoding="UTF-8"?>';
			//$this->output .= '<iCalendar>';
			if (count($this->icalevents) > 0) {
				$this->output .= '<vCalendar version="2.0" prodid="' . $this->prodid . '" method="' . $this->getMethod() . '">';
				$eventkeys = array_keys($this->icalevents);
				foreach ($eventkeys as $id) {
					$this->output .= '<vevent>';
					$event =& $this->icalevents[$id];
					$this->output .= $this->generateAttendeesOutput($event->getAttendees(), $format);
					if (!isEmpty($event->getOrganizerMail())) {
						$name = '';
						if (!isEmpty($event->getOrganizerName())) {
							$name = ' cn="' . $event->getOrganizerName() . '"';
						} // end if
						$this->output .= '<organizer' . $name . '>MAILTO:' . $event->getOrganizerMail() . '</organizer>';
					} // end if
					$this->output .= '<dtstart>' . $event->getStartDate() . '</dtstart>';

					if (strlen(trim($event->getEndDate())) > 0) {
						$this->output .= '<dtend>' . $event->getEndDate() . '</dtend>';
					} // end if
					if ($event->getFrequency() > 0) {
						$this->output .= '<rrule>FREQ=' . $this->getFrequencyName($event->getFrequency());
						if (is_string($event->getRecEnd())) {
							$this->output .= ";UNTIL=" . $event->getRecEnd();
						} elseif (is_int($event->getRecEnd())) {
							$this->output .= ";COUNT=" . $event->getRecEnd();
						} // end if
						$this->output .= ";INTERVAL=" . $event->getInterval() . ";BYDAY=" . $event->getDays() . ";WKST=" . $event->getWeekStart() . '</rrule>';
					} // end if
					if (!isEmpty($event->getExeptDates())) {
						$this->output .= '<exdate>' . $event->getExeptDates() . '</exdate>';
					} // end if
					$this->output .= '<location>' . $event->getLocation() . '</location>';
					$this->output .= '<transp>' . $event->getTransp() . '</transp>';
					$this->output .= '<sequence>' . $event->getSequence() . '</sequence>';
					$this->output .= '<uid>' . $event->getUID() . '</uid>';
					$this->output .= '<dtstamp>' . $this->ical_timestamp . '</dtstamp>';
					if (!isEmpty($event->getCategories())) {
						$this->output .= '<categories>';
						foreach ($event->getCategoriesArray() as $item) {
							$this->output .= '<item>' . $item . '</item>';
						} // end foreach
						$this->output .= '</categories>';
					} // end if
					if (!isEmpty($event->getDescription())) {
						$this->output .= '<description>' . $event->getDescription() . '</description>';
					} // end if
					$this->output .= '<summary>' . $event->getSummary() . '</summary>';
					$this->output .= '<priority>' . $event->getPriority() . '</priority>';
					$this->output .= '<class>' . $this->getClassName($event->getClass()) . '</class>';
					if (!isEmpty($event->getURL())) {
						$this->output .= '<url>' . $event->getURL() . '</url>';
					} // end if
					if (!isEmpty($event->getStatus())) {
						$this->output .= '<status>' . $this->getStatusName($event->getStatus()) . '</status>';
					} // end if
					$this->output .= $this->generateAlarmOutput($event->getAlarm(), $format);
					$this->output .= '</vevent>';
				} // end foreach event
				$this->output .= '</vCalendar>';
			} // end if count($this->icalevents) > 0
			if (count($this->icaltodos) > 0) {
				$this->output .= '<vCalendar version="2.0" prodid="' . $this->prodid . '" method="' . $this->getMethod() . '">';
				$todokeys = array_keys($this->icaltodos);
				foreach ($todokeys as $id) {
					$this->output .= '<vtodo>';
					$todo =& $this->icaltodos[$id];
					$this->output .= $this->generateAttendeesOutput($todo->getAttendees(), $format);
					if (!isEmpty($todo->getOrganizerMail())) {
						$name = '';
						if (!isEmpty($todo->getOrganizerName())) {
							$name = ' cn="' . $todo->getOrganizerName() . '"';
						} // end if
						$this->output .= '<organizer' . $name . '>MAILTO:' . $todo->getOrganizerMail() . '</organizer>';
					} // end if
					if (!isEmpty($todo->getStartDate())) {
						$this->output .= '<dtstart>' . $todo->getStartDate() . '</dtstart>';
					} // end if
					if (!isEmpty($todo->getCompleted())) {
						$this->output .= '<completed>' . $todo->getCompleted() . '</completed>';
					} // end if
					if (!isEmpty($todo->getDuration()) && $todo->getDuration() > 0) {
						$this->output .= '<duration>PT' . $todo->getDuration() . 'M</duration>';
					} // end if
					if (!isEmpty($todo->getLocation())) {
						$this->output .= '<location>' . $todo->getLocation() . '</location>';
					} // end if
					$this->output .= '<sequence>' . $todo->getSequence() . '</sequence>';
					$this->output .= '<uid>' . $todo->getUID() . '</uid>';
					$this->output .= '<dtstamp>' . $this->ical_timestamp . '</dtstamp>';
					if (!isEmpty($todo->getCategories())) {
						$this->output .= '<categories>';
						foreach ($todo->getCategoriesArray() as $item) {
							$this->output .= '<item>' . $item . '</item>';
						} // end foreach
						$this->output .= '</categories>';
					} // end if
					if (!isEmpty($todo->getDescription())) {
						$this->output .= '<description>' . $todo->getDescription() . '</description>';
					} // end if
					$this->output .= '<summary>' . $todo->getSummary() . '</summary>';
					$this->output .= '<priority>' . $todo->getPriority() . '</priority>';
					$this->output .= '<class>' . $this->getClassName($todo->getClass()) . '</class>';
					if (!isEmpty($todo->getURL())) {
						$this->output .= '<url>' . $todo->getURL() . '</url>';
					} // end if
					if (!isEmpty($todo->getStatus())) {
						$this->output .= '<status>' . $this->getStatusName($todo->getStatus()) . '</status>';
					} // end if
					if (!isEmpty($todo->getPercent()) && $todo->getPercent() > 0) {
						$this->output .= '<percent>' . $todo->getPercent() . '</percent>';
					} // end if
					if (!isEmpty($todo->getLastMod())) {
						$this->output .= '<last-modified>' . $todo->getLastMod() . '</last-modified>';
					} // end if
					if ($todo->getFrequency() > 0) {
						$this->output .= '<rrule>FREQ=' . $this->getFrequencyName($todo->getFrequency());
						if (is_string($todo->getRecEnd())) {
							$this->output .= ";UNTIL=" . $todo->getRecEnd();
						} elseif (is_int($todo->getRecEnd())) {
							$this->output .= ";COUNT=" . $todo->getRecEnd();
						} // end if
						$this->output .= ";INTERVAL=" . $todo->getInterval() . ";BYDAY=" . $todo->getDays() . ";WKST=" . $todo->getWeekStart() . '</rrule>';
					} // end if
					if (!isEmpty($todo->getExeptDates())) {
						$this->output .= '<exdate>' . $todo->getExeptDates() . '</exdate>';
					} // end if
					$this->output .= $this->generateAlarmOutput($todo->getAlarm(), $format);
					$this->output .= '</vtodo>';
				} // end foreach event
				$this->output .= '</vCalendar>';
			} // end if count($this->icaljournals) > 0
			if (count($this->icaljournals) > 0) {
				$this->output .= '<vCalendar version="2.0" prodid="' . $this->prodid . '" method="' . $this->getMethod() . '">';
				$journalkeys = array_keys($this->icaljournals);
				foreach ($journalkeys as $id) {
					$this->output .= '<vjournal>';
					$journal =& $this->icaljournals[$id];
					$this->output .= $this->generateAttendeesOutput($journal->getAttendees(), $format);
					if (!isEmpty($journal->getOrganizerMail())) {
						$name = '';
						if (!isEmpty($journal->getOrganizerName())) {
							$name = ' cn="' . $journal->getOrganizerName() . '"';
						} // end if
						$this->output .= '<organizer' . $name . '>MAILTO:' . $journal->getOrganizerMail() . '</organizer>';
					} // end if
					if (!isEmpty($journal->getStartDate())) {
						$this->output .= '<dtstart>' . $journal->getStartDate() . '</dtstart>';
					} // end if
					if (!isEmpty($journal->getCreated()) && $journal->getCreated() > 0) {
						$this->output .= '<created>' . $journal->getCreated() . '</created>';
					} // end if
					if (!isEmpty($journal->getLastMod()) && $journal->getLastMod() > 0) {
						$this->output .= '<last-modified>' . $journal->getLastMod() . '</last-modified>';
					} // end if
					$this->output .= '<sequence>' . $journal->getSequence() . '</sequence>';
					$this->output .= '<uid>' . $journal->getUID() . '</uid>';
					$this->output .= '<dtstamp>' . $this->ical_timestamp . '</dtstamp>';
					if (!isEmpty($journal->getCategories())) {
						$this->output .= '<categories>';
						foreach ($journal->getCategoriesArray() as $item) {
							$this->output .= '<item>' . $item . '</item>';
						} // end foreach
						$this->output .= '</categories>';
					} // end if
					if (!isEmpty($journal->getDescription())) {
						$this->output .= '<description>' . $journal->getDescription() . '</description>';
					} // end if
					$this->output .= '<summary>' . $journal->getSummary() . '</summary>';
					$this->output .= '<class>' . $this->getClassName($journal->getClass()) . '</class>';
					if (!isEmpty($journal->getURL())) {
						$this->output .= '<url>' . $journal->getURL() . '</url>';
					} // end if
					if (!isEmpty($journal->getStatus())) {
						$this->output .= '<status>' . $this->getStatusName($journal->getStatus()) . '</status>';
					} // end if
					if ($journal->getFrequency() != 'ONCE') {
						$this->output .= '<rrule>FREQ=' . $journal->getFrequency();
						if (is_string($journal->getRecEnd())) {
							$this->output .= ";UNTIL=" . $journal->getRecEnd();
						} elseif (is_int($journal->getRecEnd())) {
							$this->output .= ";COUNT=" . $journal->getRecEnd();
						} // end if
						$this->output .= ";INTERVAL=" . $journal->getInterval() . ";BYDAY=" . $journal->getDays() . ";WKST=" . $journal->getWeekStart() . '</rrule>';
					} // end if
					if (!isEmpty($journal->getExeptDates())) {
						$this->output .= '<exdate>' . $journal->getExeptDates() . '</exdate>';
					} // end if
					$this->output .= '</vjournal>';
				} // end foreach event
				$this->output .= '</vCalendar>';
			} // end if count($this->icaltodos) > 0
			if (count($this->icalfbs) > 0) {
				$this->output .= '<vCalendar version="2.0" prodid="' . $this->prodid . '" method="' . $this->getMethod() . '">';
				$fbkeys = array_keys($this->icalfbs);
				foreach ($fbkeys as $id) {
					$this->output .= '<vfreebusy>';
					$fb =& $this->icalfbs[$id];
					$this->output .= $this->generateAttendeesOutput($fb->getAttendees(), $format);
					if (!isEmpty($fb->getOrganizerMail())) {
						$name = '';
						if (!isEmpty($fb->getOrganizerName())) {
							$name = ' cn="' . $fb->getOrganizerName() . '"';
						} // end if
						$this->output .= '<organizer' . $name . '>MAILTO:' . $fb->getOrganizerMail() . '</organizer>';
					} // end if
					if (!isEmpty($fb->getStartDate())) {
						$this->output .= '<dtstart>' . $fb->getStartDate() . '</dtstart>';
					} // end if
					if (!isEmpty($fb->getEndDate())) {
						$this->output .= '<dtend>' . $fb->getEndDate() . '</dtend>';
					} // end if
					if (!isEmpty($fb->getDuration()) && $fb->getDuration() > 0) {
						$this->output .= '<duration>PT' . $fb->getDuration() . 'M</duration>';
					} // end if
					$this->output .= '<uid>' . $fb->getUID() . '</uid>';
					$this->output .= '<dtstamp>' . $this->ical_timestamp . '</dtstamp>';
					if (!isEmpty($fb->getURL())) {
						$this->output .= '<url>' . $fb->getURL() . '</url>';
					} // end if
					if (count($fb->getFBTimes()) > 0) {
						foreach ($fb->getFBTimes() as $timestamp => $data) {
							$values = explode(',',$data);
							$this->output .= '<freebusy fbtype="' . $values[1] . '">' . $timestamp . '/' . $values[0] . '</freebusy>';
						} // end foreach
						unset($values);
					} // end if
					$this->output .= '</vfreebusy>';
				} // end foreach event
				$this->output .= '</vCalendar>';
			} // end if count($this->icaltodos) > 0
			$this->output .= '</iCalendar>';
		} // end if xcs
		elseif ($this->output_format == 'rdf') {
			$this->output  = '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>';
			$this->output .= '<rdf:RDF xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#" xmlns="http://www.w3.org/2000/10/swap/pim/ical#" xmlns:i="http://www.w3.org/2000/10/swap/pim/ical#">';
			$this->output .= '<Vcalendar rdf:about="">';
			$this->output .= '<version>2.0</version>';
			$this->output .= '<prodid>' . $this->prodid . '</prodid>';
			$this->output .= '</Vcalendar>';
			$eventkeys = array_keys($this->icalevents);
			foreach ($eventkeys as $id) {
				$event =& $this->icalevents[$id];
				$this->output .= '<Vevent>';
				$this->output .= '<uid>' . $event->getUID() . '</uid>';
				$this->output .= '<summary>' . $event->getSummary() . '</summary>';
				if (!isEmpty($event->getDescription())) {
					$this->output .= '<description>' . $event->getDescription() . '</description>';
				} // end if
				if (!isEmpty($event->getCategories())) {
					$this->output .= '<categories>' . $event->getCategories() . '</categories>';
				} // end if
				$this->output .= '<status/>';
				$this->output .= '<class resource="http://www.w3.org/2000/10/swap/pim/ical#private"/>';
				$this->output .= '<dtstart parseType="Resource">';
				$this->output .= '<value>' . $event->getStartDate() . '</value>';
				$this->output .= '</dtstart>';
				$this->output .= '<dtstamp>' . $this->ical_timestamp . '</dtstamp>';
				$this->output .= '<due/>';
				$this->output .= '</Vevent>';
			} // end foreach event
			$this->output .= '</rdf:RDF>';
		} // end if rdf
		if (isset($event)) {
			unset($event);
		}
	} // end function
	
	/**
	* Loads the string into the variable if it hasn't been set before
	*
	* @desc Loads the string into the variable
	* @param string $format  ics | xcs | rdf
	* @return string $output
	* @see generateOutput()
	* @see writeFile()
	*/
	function getOutput($format = 'ics') {
		if (!isset($this->output) || $this->output_format != $format) {
			$this->generateOutput($format);
		} // end if
		return $this->output;
	} // end function

	/**
	* Sends the right header information and outputs the generated content to
	* the browser
	*
	* @desc Sends the right header information
	* @param string $format  ics | vcs (only Events)| xcs | rdf (only Events)
	* @param string $name  The name of the file
	* @return void
	* @uses getOutput()
	*/
	function outputFile($format = 'ics', $name = 'iCal_download') {
		if ($format == 'ics') {
			header('Content-Type: text/Calendar');
			header('Content-Disposition: attachment; filename=' . $name . '.ics');
			echo $this->getOutput('ics');
		} elseif ($format == 'vcs') {
		    header("Content-Type: text/x-vCalendar");
			header('Content-Disposition: attachment; filename=' . $name . '.vcs');
			echo $this->getOutput('vcs');
		} elseif ($format == 'xcs') {
			header('Content-Type: text/Calendar');
			header('Content-Disposition: attachment; filename=' . $name . '.xcs');
			echo $this->getOutput('xcs');
		} elseif ($format == 'rdf') {
			header('Content-Type: text/xml');
			header('Content-Disposition: attachment; filename=' . $name . '.rdf');
			echo $this->getOutput('rdf');
		} // end if
	} // end function


	/**
	* Writes the string into the file and saves it to the download directory
	*
	* @desc Writes the string into the file and saves it to the download directory
	* @return void
	* @see getOutput()
	* @uses checkDownloadDir()
	* @uses generateOutput()
	* @uses deleteOldFiles()
	*/
	function writeFile() {
		if ($this->checkDownloadDir() == FALSE) {
			die('error creating download directory');
		} // end if
		if (!isset($this->output)) {
			$this->generateOutput();
		} // end if
		$handle = fopen($this->download_dir . '/' . $this->events_filename, 'w');
		fputs($handle, $this->output);
		fclose($handle);
		$this->deleteOldFiles(300);
		if (isset($handle)) {
			unset($handle);
		}
	} // end function

	/**
	* Returns the full path to the saved file where it can be downloaded.
	*
	* Can be used for “header(Location:…”
	*
	* @desc Returns the full path to the saved file where it can be downloaded.
	* @return string  Full http path
	*/
	function getFilePath() {
		$path_parts = pathinfo($_SERVER['SCRIPT_NAME']);
		$port = (($_SERVER['SERVER_PORT'] != 80) ? ':' . $_SERVER['SERVER_PORT'] : '' );
		return 'http://' . $_SERVER['SERVER_NAME'] . $port . $path_parts["dirname"] . '/' . $this->download_dir . '/' . $this->events_filename;
	} // end function
	

	/**
	* Writes the string into the file and saves it to the download directory
	*
	* @desc Writes the string into the file and saves it to the download directory
	* @param int $time  Minimum age of the files (in seconds) before file get deleted
	* @return void
	* @see writeFile()
	*/
	function deleteOldFiles($time = 300) {
		if ($this->checkDownloadDir() == FALSE) {
			die('error creating download directory');
		} // end if
		if (!is_int($time) || $time < 1) {
			$time = 300;
		} // end if
		$handle = opendir($this->download_dir);
		while ($file = readdir($handle)) {
			if (!eregi("^\.{1,2}$",$file) && !is_dir($this->download_dir . '/' . $file) && eregi("\.ics",$file) && ((time() - filemtime($this->download_dir . '/' . $file)) > $time)) {
				unlink($this->download_dir . '/' . $file);
			} // end if
		} // end while
		closedir($handle);
		if (isset($handle)) {
			unset($handle);
		}
		if (isset($file)) {
			unset($file);
		}
	} // end function
}
?>