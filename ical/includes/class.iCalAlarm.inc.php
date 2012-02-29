<?php

include_once 'class.iCalBase.inc.php';

class iCalAlarm extends iCalBase {

	/**
	* Kind of alarm (0 = DISPLAY, 1 = EMAIL, (not supported: 2 = AUDIO, 3 = PROCEDURE))
	*
	* @desc Kind of alarm (0 = DISPLAY, 1 = EMAIL, (not supported: 2 = AUDIO, 3 = PROCEDURE))
	*/
	var $action;

	/**
	* Minutes the alarm goes off before the event/todo
	*
	* @desc Minutes the alarm goes off before the event/todo
	*/
	var $trigger = 0;

	/**
	* Headline for the alarm (if action = Display)
	*
	* @desc Headline for the alarm (if action = Display)
	* @var string
	*/
	var $summary;

	/**
	* Duration between the alarms in minutes
	*
	* @desc Duration between the alarms in minutes
	*/
	var $duration;

	/**
	* How often should the alarm be repeated
	*
	* @desc How often should the alarm be repeated
	*/
	var $repeat;
	
	/**
	* Constructor
	*
	* Only job is to set all the variablesnames
	*
	* @desc Constructor
	* @param int $action  0 = DISPLAY, 1 = EMAIL, (not supported: 2 = AUDIO, 3 = PROCEDURE)
	* @param int $trigger  Minutes the alarm goes off before the event/todo
	* @param string $summary  Title for the alarm
	* @param string $description  Description
	* @param $attendees  key = attendee name, value = e-mail, second value = role of the attendee
	* [0 = CHAIR | 1 = REQ | 2 = OPT | 3 =NON] (example: array('Michi' => 'flaimo@gmx.net,1'); )
	* @param int $duration  Duration between the alarms in minutes
	* @param int $repeat  How often should the alarm be repeated
	* @param string $lang  Language of the strings used in the event (iso code)
	* @uses setAction()
	* @uses setTrigger()
	* @uses setSummary()
	* @uses iCalBase::setDescription()
	* @uses setAttendees()
	* @uses setDuration()
	* @uses setRepeat()
	* @uses iCalBase::setLanguage()
	*/
	function iCalAlarm($action, $trigger, $summary, $description, $attendees,
					   $duration, $repeat, $lang) {
        parent::iCalBase();
        $this->setAction($action);
		$this->setTrigger($trigger);
		parent::setSummary($summary);
		parent::setDescription($description);
		parent::setAttendees($attendees);
		$this->setDuration($duration);
		$this->setRepeat($repeat);
		parent::setLanguage($lang);
	} // end constructor

	/**
	* Set $action variable
	*
	* @desc Set $action variable
	* @param int $action 0 = DISPLAY, 1 = EMAIL, (not supported: 2 = AUDIO, 3 = PROCEDURE)
	* @see getAction()
	* @see $action
	*/
	function setAction($action = 0) {
		$this->action = $action;
	} // end function

	/**
	* Set $trigger variable
	*
	* @desc Set $trigger variable
	* @param int $minutes
	* @see getTrigger()
	* @see  $minutes
	*/
	function setTrigger($minutes = 0) {
		$this->trigger = $minutes;
	} // end function

	/**
	* Set $duration variable
	*
	* @desc Set $duration variable
	* @param int $int
	* @see getDuration()
	* @see  $duration
	*/
	function setDuration($int = 0) {
		$this->duration = $int;
	} // end function

	/**
	* Set $repeat variable
	*
	* @desc Set $repeat variable
	* @param int $int  in minutes
	* @see getRepeat()
	* @see  $repeat
	*/
	function setRepeat($int = 0) {
		$this->duration = $int;
	} // end function
	

	/**
	* Get $action variable
	*
	* @desc Get $action variable
	* @return string $action
	* @see setAction()
	* @see $action
	*/
	function &getAction() {
		$action_status = array('DISPLAY', 'EMAIL', 'AUDIO', 'PROCEDURE');
		return ((array_key_exists($this->action, $action_status)) ? $action_status[$this->action] : $action_status[0]);
	} // end function

	/**
	* Get $trigger variable
	*
	* @desc Get $trigger variable
	* @return int $trigger
	* @see setTrigger()
	* @see $trigger
	*/
	function &getTrigger() {
		return $this->trigger;
	} // end function
	

	/**
	* Get $duration variable
	*
	* @desc Get $duration variable
	* @return int $duration
	* @see setDuration()
	* @see $duration
	*/
	function &getDuration() {
		return $this->duration;
	} // end function

	/**
	* Get $repeat variable
	*
	* @desc Get $repeat variable
	* @return int $repeat
	* @see setRepeat()
	* @see $repeat
	*/
	function &getRepeat() {
		return $this->duration;
	} // end function
}
?>