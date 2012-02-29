<?php

  // Use this setting to descide what projects to display. Can
  // be either 'recent' to display the recent projects (default)
  // or 'byteam' to display the projects by team (and project
  // startdate and enddate)
  $config["project_selection"] = "recent";

  // When the setting above is 'by team', set the id of a default
  // activity to display. Other activities can be added with an
  // automatically added set of selectboxes.
  // Set to 0 or remove config to fallback to default functionality.
  // $config["project_selection_byteam_activityfilter"]  = 1;

  // Set to 'true' to hide the 'add project/phase/activity' link
  $config["hide_add_project_link"] = false;

  // If set to true, active projects do not need to have "Always
  // visible in time registration" to be set.
  $config["autoshowactiveprojects"] = false;

  // When set to true AND PROJECT_SELECTION = "byteam"!!!
  // the dates outside the projectdates will be hidden
  $config["no_booking_outside_projectdates"] = false;
  
  //The from name if you are sending mails Name <your@email.address>
  $config['from'] = 'Achievo <automailer@achievo.org>';
  
  // Each coordinator has to validate all hours before a week is
  // entirely validated
  $config["approve_week_per_coordinator"] = false;
  
  // Sends out a mail to the coordinators when a user locks a week
  // (only works when 'approve week per coordinator' is set to true)
  $config["mail_coordinators_on_lock"] = false;
  
  //The from name if you are sending mails Name <your@email.address>
  $config['from'] = 'Achievo <automailer@achievo.org>';
  
  // If you want to use a calendar in stead of a dropdown box
  $config['useCalendar'] = false;
  
  // If you want project and phase are seperated colums set this config to true
  // This config is also used for the report module.
  $config['seperatedProjectPhase'] = false;
  
  // collapses the items in the list if there is more than one item
  // per project/projectPhase.
  $config['collapseProjects'] = true;
?>