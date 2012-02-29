<?php

   class project
   {
     public function renderByTeam($team_id, $tpl="projectlist.tpl")
     {
       $ui = atkinstance("atk.ui.atkui");

       $data = $this->_getProjects("department='".$team_id."'");
       if (count($data))
       {
         return $ui->render($tpl, array("data"=>$data), "wiki");
       }
       return "";
     }

     private function _getProjects($filter)
     {
       useattrib("wiki.wikilinkattribute");

       $namer = new wikiLinkAttribute("", " project");

       $db = &atkGetDb();

       $to = date("Y-m-d");
       $from = date("Y-m-d", strtotime("-1 week"));

       $records = $db->getrows("SELECT DISTINCT p.name, p.id
                                FROM project p,
				     phase,
				     hours,
				     person
				WHERE
				   p.id = phase.projectid
				   AND phase.id = hours.phaseid
				   AND hours.userid = person.id
				   AND hours.activitydate BETWEEN '$from' AND '$to'
				   AND NOT
				   EXISTS (
				     SELECT 1
				     FROM person
				     WHERE role = 'employee'
				     AND p.name LIKE CONCAT(person.firstname,'%', person.lastname,'%')
				     )
				   AND $filter
				ORDER BY p.name");
				     // the 'not exist' clause filters out personal projects

       for($i=0, $_i=count($records); $i<$_i; $i++)
       {
         $records[$i]["wikilink"] = $namer->wikiName($records[$i]["name"]); 
       }
       
       return $records;
     }


   }
