<?php
  //
  // iCal configuration options
  //
  // Change the config_atkroot so it points to the achievo homedir
  // 
  $config_atkroot = "../../../achievo-local/";
   
  // The number of months that the events go back
  $monthsback = 1;
  // The number of months that the events go in the future
  $monthsahead = 3;

  // below there should be nothing to edit
  include_once($config_atkroot."atk.inc");
  include_once($config_atkroot."version.inc");
  require_once($config_atkroot.moduleDir("ical")."includes/class.iCal.inc.php");
  require_once($config_atkroot.moduleDir("ical")."tools.php");
  
  // check achievo version (if g_db exist it's 1.1 or earlier)
  if (is_object($g_db)) { $achievo = 1; } else { $achievo = 0; }
   
  // check if it's a valid user with ID given in URL
  foreach ($_GET as $key => $value) { $iduser=$key; break; }
  
  // Some programs add there own stuff to the URL, so we strip it here if known!
  // 2006-01-12: Yagoon Time adds _ics
  $nontoken = array("_ics");
  $iduser = str_replace($nontoken,"",$iduser);

  
  // db instance (depends on version of achievo which one to call)
  if ($achievo==1)
  {
      $db = "atk".atkconfig("database")."query";
  } else {
      $db = &atkGetDb();
      $g_db = $db;
  }
  
  // make query to check if user is known
  if ($achievo==1) $query = new $db();
  else $query = &$db->createQuery();
  $query->addTable('ical_preferences');
  $query->addField('*', ' ', 'ical_preferences');
  $query->addCondition("ical_user_url = '".$iduser."'");
  $querystring = $query->buildSelect(TRUE);
  $user = $g_db->getRows($querystring);
  $userid = $user[0]['ical_user_id'];
  
  // No user is found, check if TOKEN is the standard token
  if (count($user)==0)
  {
    $userToken = decrypt($iduser,atkconfig('auth_accountenableexpression'));  
    $userid = explode(":",$userToken);
    
    if (is_numeric($userid[0]))  
    {
     if ($achievo==1) $query = new $db();
     else $query = &$db->createQuery();
    $query->addTable('person');
    $query->addField('userid as ical_user', ' ', 'person');
    $query->addField('id', ' ', 'person');
    $query->addCondition("person.id = '".$userid[0]."'");
    $querystring = $query->buildSelect(TRUE);
    $user = $g_db->getRows($querystring);
    $userid = $user[0]['id'];
    // Show only the calendar events if no prefs are set!
    $user[0]['ical_events'] = 1;
    }

    // NO USER FOR TOKEN show 401
    if (count($user)==0)  
    {
     header('HTTP/1.0 401 Unauthorized');
     die("<h1>You are unauthorized to perform this action</h1>");
    }
  }
  
  // All ready to go!
  if (count($user)>0)
  {
  global $g_db;
  
  $iCal = new iCal('Achievo', 0);
  
  if ($user[0]['ical_events']==1)
  {
    $edate = date('Y-m-d',mktime(12,0,0,date("m")-$monthsback,1,date("Y")));
    $bdate = date('Y-m-d',mktime(12,0,0,date("m")+$monthsahead,1,date("Y")));
    
    if ($achievo==1) $query = new $db();
    else $query = &$db->createQuery();
    $query->addTable('schedule');
    $query->addJoin('schedule_attendee', '', 'schedule_attendee.schedule_id=schedule.id', TRUE);
    $query->addJoin('person', '', 'person.id=schedule_attendee.person_id', TRUE);
    $query->addJoin('person', 'owner', 'owner.id=schedule.owner', TRUE);
    $query->addJoin('schedule_type', 'stype', 'stype.id=schedule.scheduletype', TRUE);

    $query->addField('id', ' ', 'schedule');
    $query->addField('startdate', ' ', 'schedule');
    $query->addField('enddate', ' ', 'schedule');
    $query->addField('starttime', ' ', 'schedule');
    $query->addField('endtime', ' ', 'schedule');
    $query->addField('title', ' ', 'schedule');
    $query->addField('description', ' ', 'schedule');
    $query->addField('location', ' ', 'schedule');
    $query->addField('allday', ' ', 'schedule');
    $query->addField('publicitem', ' ', 'schedule');
    $query->addField('owner', ' ', 'schedule');
    $query->addField('status', ' ', 'schedule');
    $query->addField('all_users', ' ', 'schedule');
    $query->addField('description AS stype', '', 'stype');    
    $query->addField('firstname', ' ', 'owner');    
    $query->addField('lastname', ' ', 'owner');    
    $query->addField('email', ' ', 'owner');    
    $query->addField('id AS personid', ' ', 'person');    
    if ($user[0]['ical_only_events_privat']==1) 
    {
    $query->addCondition("schedule.startdate <='$bdate' AND schedule.enddate >= '$edate' AND (schedule.owner='".$user[0]['ical_user']."' OR person.userid='".$user[0]['ical_user']."')");    
    } else {
    $query->addCondition("(schedule.startdate <='$bdate' AND schedule.enddate >= '$edate')
                          OR (schedule.startdate <='$bdate' AND schedule.enddate >= '$edate' AND (schedule.owner='".$user[0]['ical_user']."' OR person.userid='".$user[0]['ical_user']."'))");
    }
    $query->addGroupBy('schedule.id');
    $query->addOrderBy('startdate,starttime');
    $querystring = $query->buildSelect(TRUE);
    $recs = $g_db->getRows($querystring);


    // Loop thru data
    for ($i=0;$i<count($recs);$i++)
    {
        // Get attendees for event
        $attendees = array();
        $attendee = $g_db->getRows("SELECT DISTINCT person.userid,person.lastname,person.firstname,person.email,person.role FROM schedule_attendee LEFT JOIN person ON person.id=schedule_attendee.person_id WHERE schedule_attendee.schedule_id='".$recs[$i]['id']."'");
        for ($a=0;$a<count($attendee);$a++)
        {
            $attendees[$attendee[$a]['firstname']." ".$attendee[$a]['lastname']]=$attendee[$a]['email'].",3";
        }
        // Get links inside the description
        $links = extractLinks($recs[$i]['description']);
        
        // Only add when event is public or your the owner
        if ($recs[$i]['publicitem']==1 or ($userid==$recs[$i]['owner'] or $userid==$recs[$i]['personid'] and $user[0]['ical_events_privat']==1))
        {
        $organizer = array($recs[$i]['firstname']." ".$recs[$i]['lastname'], $recs[$i]['email']);
        
        $iCal->addEvent($organizer, // SET Organiser for this event
                        ($recs[$i]['allday']=="0" ? timestamp($recs[$i]['startdate'],$recs[$i]['starttime']) : timestamp($recs[$i]['startdate'],"00:00:00",array("0","0","0","0","1","0"))), // Set starttime of event (When its an allday event set it to tommorow 00:00:00)
                       ($recs[$i]['allday']=="0" ? timestamp($recs[$i]['enddate'],$recs[$i]['endtime']) : timestamp($recs[$i]['enddate'],"00:00:00",array("0","0","0","0","1","0"))), // Set endtime of event (When its an allday event set it to tommorow 00:00:00)
                        $recs[$i]['location'], // Set location of the event
                        0, // Set Transparancy (0 = OPAQUE | 1 = TRANSPARENT)
                        array(str_replace("/"," ",$recs[$i]['stype'])), // Set event Categorie
                        preg_replace("/(\r\n|\n|\r)/", " ", str_replace(",","",$recs[$i]['description'])), //Set event description (DSTRIP line breaks, because not all ical programs can handle them!)
                        $recs[$i]['title']." (".$recs[$i]['firstname']." ".$recs[$i]['lastname'].")", // Set title of the event
                        $recs[$i]['publicitem'], // Set kind of class
                        $attendees, // Attendees array(key = attendee name, value = e-mail, role of the attendee [0 = CHAIR | 1 = REQ | 2 = OPT | 3 =NON])
                        1, // Set priority (scale 1 to 9)
                        0, // Set frequency of event (0 = once | 1 = secondly | 2 = minutly | 3 = hourly | 4 = daily | 5 = weekly | 6 = monthly | 7 = yearly) NOTE: is set to 0 for Outlook Compatibility!!
                        ($recs[$i]['allday']=="0" ? 0 : timestamp($recs[$i]['enddate'],$recs[$i]['endtime'])), // set recurrency end ('' = forever | integer = number of times | timestring = explicit date)
                        0, // Set interval for frequency (every 2,3,4 weeks...) NOTE: is set to 0 for Outlook Compatibility!!
                        array(0,1,2,3,4,5,6), // Array with the number of the days the event accures (example: array(0,1,5) = Sunday, Monday, Friday
                        1, // Set startday of the Week (0 = Sunday | 6 = Saturday|1 = Monday)
                        "", // Set exeption dates: Array with timestamps of dates that should not be includes in the recurring event
                        ($user[0]['ical_alarm']=="1" && $starttime>time() ? array(
  					                                                              0, // Action: 0 = DISPLAY, 1 = EMAIL, (not supported: 2 = AUDIO, 3 = PROCEDURE)
	  				                                                              $user[0]['ical_alarm_time'],  // Trigger: alarm before the event in minutes
		  			                                                              $recs[$i]['title']." (".$recs[$i]['firstname']." ".$recs[$i]['lastname'].")", // Title
			  		                                                              preg_replace("/(\r\n|\n|\r)/", " ", str_replace(",","",$recs[$i]['description'])), // Description
				  	                                                              $attendees, // Array (key = attendee name, value = e-mail, second value = role of the attendee [0 = CHAIR | 1 = REQ | 2 = OPT | 3 =NON])
					                                                              5, // Duration between the alarms in minutes
					                                                              0  // How often should the alarm be repeated
					                                                              ) : "" ), // Set alarm for event
                         ($recs[$i]['status'] == "confirmed" ? ($recs[$i]['status'] == "cancelled" ?  2 : 1) : 0 ), // Set Status of the event
                         $links[0], // Set link for event
                         substr($config_languagefile,0,strpos($config_languagefile,".")), // Set language for event strings
                         $recs[$i]['id'] // Set UID of the event
                         );
        }
    }
  }
  
  if ($user[0]['ical_todo']==1)
  {
    // TODO's from Achievo
    if ($achievo==1) $query = new $db();
    else $query = &$db->createQuery();
    $query->addTable('todo');

    $query->addJoin('person', 'assigned_to', 'todo.assigned_to=assigned_to.id', TRUE);
    $query->addJoin('person', 'owner', 'owner.id=todo.owner', TRUE);
    $query->addJoin('project', 'projectid', 'todo.projectid=projectid.id', TRUE);

    $query->addField('id', ' ', 'todo');
    $query->addField('lastname', ' ', 'owner');
    $query->addField('firstname', ' ', 'owner');
    $query->addField('email', ' ', 'owner');
    $query->addField('id AS ownerid', ' ', 'owner');
    $query->addField('cellular', ' ', 'owner');
    $query->addField('fax', ' ', 'owner');
    $query->addField('status AS ownerstatus', ' ', 'owner');
    $query->addField('name', ' ', 'projectid');
    $query->addField('id AS projectid', ' ', 'projectid');
    $query->addField('title', ' ', 'todo');
    $query->addField('lastname AS lastname_assigned', ' ', 'assigned_to');
    $query->addField('firstname AS firstname_assigned', ' ', 'assigned_to');
    $query->addField('email AS firstname_email', ' ', 'assigned_to');
    $query->addField('id AS id_assigned', ' ', 'assigned_to');
    $query->addField('cellular AS cellular_assigned', '', 'assigned_to');    
    $query->addField('fax AS fax_assigned', ' ', 'assigned_to');    
    $query->addField('status AS status_assigned', ' ', 'assigned_to');    
    $query->addField('entrydate', ' ', 'todo');    
    $query->addField('duedate', ' ', 'todo');    
    $query->addField('updated', ' ', 'todo');    
    $query->addField('priority', ' ', 'todo');    
    $query->addField('description', ' ', 'todo');
    $query->addField('status', ' ', 'todo');
    
    $query->addCondition("assigned_to='".$userid."' OR todo.owner='".$userid."' AND todo.status NOT IN (5,2)");

    $query->addOrderBy('duedate, priority');
    $querystring = $query->buildSelect(TRUE);
    $recs = $g_db->getRows($querystring);
    
    for ($i=0;$i<count($recs);$i++)
    {

    $iCal->addToDo(
               $recs[$i]['title'], // Todo title
               $recs[$i]['description'], // Todo description  
			   '', // Todo location
               timestamp($recs[$i]['entrydate']), // Todo Start time / Entry date  
			   '', // Todo duration [not used, but can be the difference between start - end date]
               timestamp($recs[$i]['duedate']),// Todo End time / Deadline
			   0, // Todo percentage complete [not inluded yet]
               $recs[$i]['priority'], // Todo priority
			   ($recs[$i]['status']=="1" or $recs[$i]['status']=="3" ? 0 : ($recs[$i]['status']=="2" or $recs[$i]['status']=="4" ? 2 : ($recs[$i]['status']=="5" ? 5 : ''))), //Todo status
			   0,// Todo Class (0 = PRIVATE | 1 = PUBLIC | 2 = CONFIDENTIAL)
			   array($recs[$i]['firstname']." ".$recs[$i]['lastname'], $recs[$i]['email']), // Todo organiser
			   array($recs[$i]['firstname_assigned']." ".$recs[$i]['lastname_assigned'], $recs[$i]['email_assigned'],'3'), // Todo attendees (person who as the todo assigned)
			   array($recs[$i]['name']), // Todo categories
			   timestamp(substr($recs[$i]['updated'],0,10),substr($recs[$i]['updated'],11,18)), // Last modification date/time in unix timestamp
			   ($user[0]['ical_alarm']=="1" ? array(0,$user[0]['ical_alarm_time'],$title, $description, $attendees, 5, $user[0]['ical_alarm_repeat']) : 0), // Todo Alarm
			   0, // Todo frequency
			   '', // Todo recurrency end
			   0, // Todo interval for frequency
			   array(0,1,2,3,4,5,6), // Todo day numbers, array with the number of the days the event accures
			   1, // Startday of the Week
			   "", // Exeption dates: Array with timestamps of dates that should not be includes in the recurring event
			   "", // Todo link
			   substr($config_languagefile,0,strpos($config_languagefile,".")), // Language of the Strings
               $recs[$i]['id'] // Optional UID for this TODO
			  );

    }
  }
   
  // RENDER the file
  $filename = "Calendar_".urlencode($user[0]['ical_user'])."_".date("F_j_Y");
  if (empty($user[0]['ical_format'])) $user[0]['ical_format'] = "ics";
  $iCal->outputFile($user[0]['ical_format'],$filename);
  
 }
?>
