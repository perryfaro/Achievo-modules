<?php

   class team
   {
     public function getTeams()
     {
       $team = atkGetNode("employee.department");
       return $team->selectDb();
     }

     public function renderMembers($team_id)
     {
       $address = atknew("module.wiki.utils.addressbook");
       $result = $address->render("department='".$team_id."'", "team.tpl");
       return $result;
     }

     public function renderProjects($team_id)
     {
       $project = atknew("module.wiki.utils.project");
       $result = $project->renderByTeam($team_id);
       return $result;
     }
   }
