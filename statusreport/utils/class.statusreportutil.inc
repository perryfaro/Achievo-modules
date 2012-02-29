<?php
class statusreportutil
{
  static function getEmployeeIdWithRight($module, $node, $right)
  {
    $db = &atkGetDb();
    $rows = $db->getrows("
      SELECT
        e.id
      FROM
       person e
       JOIN employeerole r ON e.id=r.employee_id
       JOIN accessright a ON r.role_id=a.role_id
      WHERE
       e.status='active' AND e.role='employee'
       AND a.node='$module.$node'
       AND a.action='$right'");

    $res = array();
    foreach ($rows as $r)
    {
      $res[] = $r['id'];
    }
    return $res;
  }

  static function getProjectData($project_id)
  {
    if(empty($project_id)) atkerror("Project id don't set");
    
    $db = &atkGetDb();
    list($row) = $db->getrows("
      SELECT
        p.coordinator,p.startdate,p.enddate,
        (SELECT SUM(initial_planning) FROM phase WHERE projectid='$project_id') initial,
        (SELECT SUM(current_planning) FROM phase WHERE projectid='$project_id') plan,
        (SELECT SUM(time) FROM hours h JOIN phase t ON h.phaseid=t.id WHERE t.projectid='$project_id') fact
      FROM project p WHERE p.id='$project_id'
    ");

    return $row;
  }

  static function date2date($string)
  {
    list($year,$month,$day) = explode("-",$string);
    return sprintf("%04d%02d%02d",$year,$month,$day);
  }

  static function getBookedProjectId($days,$coordinator)
  {
    $db = &atkGetDb();
    $rows = $db->getrows("
      SELECT
        t.projectid id
      FROM
       phase t
       JOIN hours h ON t.id=h.phaseid
       JOIN project p ON p.id=t.projectid
      WHERE
        p.coordinator='$coordinator' AND
        h.activitydate BETWEEN NOW() - INTERVAL $days DAY AND NOW()
      GROUP BY t.projectid
    ");

    $res = array();
    foreach ($rows as $r)
    {
      $res[] = $r['id'];
    }
    return $res;
  }

  static function getProjectIdByMTOwner($userid)
  {
    $db = &atkGetDb();
    $rows = $db->getrows("
      SELECT
        project_id
      FROM
        statusreport
      WHERE
       id IN(
        SELECT
          MAX(id) id
        FROM
          statusreport
        GROUP BY project_id)
        AND mt_owner='$userid'
    ");

    $res = array();
    foreach ($rows as $r)
    {
      $res[] = $r['project_id'];
    }
    return $res;
  }
  
  static function getFlaggedProjectid()
  {
    $db = &atkGetDb();
    $rows = $db->getrows("
      SELECT
        project_id
      FROM
        statusreport
      WHERE 
        id IN(
          SELECT
            max(id)
          FROM
            statusreport
          GROUP BY project_id)
        AND (pm_traffic_light!='green' || achievo_traffic_light='red')
  ");    
    $res = array();
    foreach ($rows as $r)
    {
      $res[] = $r['project_id'];
    }
    return $res;
    
  }

}