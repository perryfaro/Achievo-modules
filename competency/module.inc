<?php


class mod_competency extends atkModule
{
  function getMenuItems()
  {
    $this->menuitem("Competence_management");
    $this->menuitem("Competency_domain",dispatch_url("competency.competencydomain","admin"),"Competence_management", array("competency.competencydomain", "admin"));
    $this->menuitem("Competency",dispatch_url("competency.competency","admin"),"Competence_management", array("competency.competency", "admin"));
    $this->menuitem("Competence_profile", dispatch_url("competency.competencyprofile", "admin"),"Competence_management", array("competency.competencyprofile", "admin"));
    $this->menuitem("competency_profile_match", dispatch_url("competency.competency_profile_match", "stats"),"Competence_management", array("competency.competency_profile_match", "stats"));
    $this->menuitem("competenceplanning",dispatch_url("competency.competenceplanning","admin"),"Competence_management", array("competency.competenceplanning",'admin'));
    $this->menuitem("<div width=100% style=\"border-top: 2px solid #ddd;\"></div>","","Competence_management");

    $this->menuitem("match", dispatch_url("competency.competencymatch", "stats"),"Competence_management", array("competency.competencymatch", "stats"));
    $this->menuitem("competencycompanyview", dispatch_url("competency.competencycompanyview", "admin"),"Competence_management", array("competency.competencycompanyview", "admin"));
    $this->menuitem("teamview", dispatch_url("competency.teamview", "stats"),"Competence_management", array("competency.teamview", "stats"));

    $this->menuitem("<div width=100% style=\"border-top: 2px solid #ddd;\"> </div>","","Competence_management");
    $this->menuitem("competencegrowth", dispatch_url("competency.competencegrowth", "stats"),"Competence_management", array("competency.competencegrowth", "admin"));
    $this->menuitem("competencegrowthteam", dispatch_url("competency.competencegrowthteam", "stats"),"Competence_management", array("competency.competencegrowthteam", "stats"));
    $this->menuitem("competency_domain_void", dispatch_url("competency.competencydomainholes", "stats"),"Competence_management", array("competency.competencydomainholes", "stats"));
    $this->menuitem("competency_void", dispatch_url("competency.competencyholes", "stats"),"Competence_management", array("competency.competencyholes", "stats"));
  }

  function getNodes()
  {
    registerNode("competency.competency", array("admin", "add", "edit", "delete","view"));
    registerNode("competency.competenceplanning", array("admin", "add", "edit", "delete"));
    registerNode("competency.competencydomain", array("admin", "add", "edit", "delete"));
    registerNode("competency.competencyniveau", array("admin", "add", "edit", "delete"));
    registerNode("competency.competencyprofile", array("admin", "add", "edit", "delete","addcompetences"));
    registerNode("competency.competencegrowth", array("stats","show_all_users","show_self","show_team"));
    registerNode("competency.competencegrowthteam", array("stats","show_all_users","show_own_team"));
    registerNode("competency.competencyteamview", array("stats"));
    registerNode("competency.competencymatch", array("add", "stats", "report","show_all_users","show_own_team", "schedule_add", "schedule_save"));
    registerNode("competency.competency_profile_match", array("add","delete", "stats", "report"));
    registerNode("competency.competencyholes", array("stats"));
    registerNode("competency.competencydomainholes", array("stats"));
    registerNode("competency.teamview", array("stats" ,"showallteams"));
    registerNode("competency.competencycompanyview", array("stats"));
  }


}
?>
