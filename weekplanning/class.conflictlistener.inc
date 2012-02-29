<?php

  atkimport("atk.utils.atkactionlistener");

  class conflictListener extends atkActionListener
  {

    function conflictListener($actionfilter=array())
    {
      $this->atkActionListener($actionfilter);
    }

    function notify($action, $record)
    {
      $node = &getNode("weekplanning.weekplanningconflictlog");
      $nodetype = $this->m_node->atkNodeType();

      atkdebug("Conflictlistener notify ".$nodetype);

      if ($nodetype=="timereg.hours")
      {
        // check conflicts CONFLICT_BOOKED_NEARS_PLANNED and CONFLICT_BOOKED_EXCEEDS_PLANNED
        $node->checkConflictBooked($record["phaseid"]["phaseid"]);
      }
      else if ($nodetype=="project.phase")
      {
        $node->checkConflictBooked($record["id"]);
        $node->checkConflictPlanned($record["id"]);
      }
      else if ($nodetype=="employee.usercontracts")
      {
        // Evaluate existing conflicts in this case
        $conflicts = $node->selectDb("weekplanning_conflict.subjectid='{$record["userid"]["id"]}' AND weekplanning_conflict.type='".CONFLICT_PLANNED_EXCEEDS_EMPLOYEE_CONTRACT."'", "", "", "", array("conflictdate"));
        foreach($conflicts as $conflict)
        {
          $node->checkConflictEmployee($record["userid"]["id"], dateutil::arr2str($conflict["conflictdate"]));
        }
      }
      else if ($nodetype=="weekplanning.weekplanning_peremployee" || $nodetype=="weekplanning.weekplanning_perproject")
      {
        // watch out, values are stored 'flat' in $record because this notify is manually called from
        // weekplanning node, with flat record.
        $node->checkConflictPlanned($record["phaseid"]);
        $node->checkConflictEmployee($record["employeeid"], $record["monday"]);
      }
    }


  }

?>