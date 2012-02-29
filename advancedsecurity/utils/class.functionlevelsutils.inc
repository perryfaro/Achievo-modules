<?php
  /**
   * One line description of the class
   * 
   * Full description of the class
   *
   * @author Dennis-Jan Broerse <dennisjan@ibuildings.nl>
   * @package package
   * @subpackage subpackage
   *
   */
  class functionlevelutils
  {
    
    /**
     * Constructor
     *
     */
    function functionlevelutils()
    {
      
    }  
    
    function getMaxLevelId()
    {
      $db = &atkGetDb();
      
      $sql = "SELECT MAX(functionlevel.name) AS flevel FROM functionlevel";
      
      $record = $db->getRows($sql);
      
      $name = $record[0]['flevel'];
      $sql = "SELECT functionlevel.id FROM functionlevel WHERE functionlevel.name = $name";
      
      if (!is_array($record) && count($record) < 1)
        return null;
        
      $record = $db->getRows($sql);
      
      if (is_array($record))
        return $record[0]['id'];
        
      return null;
    }
    
    function getLevelIdFromUser()
    {
      $user = getUser();
      
      if (is_array($user) && $user != '' && $user["name"]!='administrator')
      {
        return $user['functionlevel'];
      }
      elseif($user["name"]=='administrator')
      {
        return $this->getMaxLevelId();
      }     
      return null;
    }
    
    function getLevel($levelId)
    {
      if (!is_null($levelId))
      {
        $db = &atkGetDb();

        $sql = "SELECT functionlevel.name FROM functionlevel WHERE functionlevel.id = $levelId";

        $record = $db->getRows($sql);

        if (is_array($record) && count($record) > 0)
          return $record[0]['name'];
      }
        
      return null;
    }
    
    function getEmployees()
    {
      $level = $this->getLevel($this->getLevelIdFromUser());
      $maxLevel = $this->getLevel($this->getMaxLevelId());
      $user = atkGetUser();
      if ($level == $maxLevel || $user["name"]=="administrator")
      {
        atkdebug("Level is maxlevel. Returning all employees.");
        // User has max level and is allowed to see all the employees
        $employees = $this->getEmployeesFromDB();
      }
      else
      {
        atkdebug("Level: $level");
        $where = " AND functionlevel.name < $level";
        $employees = $this->getEmployeesFromDB($where);
      }
      
      if (is_array($employees) && count($employees) > 0)
        return $employees;
        
      return null;
    }
    
    function getEmployeesFromDB($where = '')
    {
      $db = &atkGetDb();
      
      $sql = "SELECT person.id,person.lastname, person.firstname FROM `person` 
              INNER JOIN functionlevel ON (person.functionlevel = functionlevel.id 
              AND person.status='active' AND person.role='employee' $where)";
            
      $records = $db->getRows($sql);
      
      if (is_array($records) && count($records) > 0)
        return $records;
        
      return null;
    }
    
    function getEmployeesDropDown($employees)
    {
      if (is_array($employees) && count($employees) > 0)
      {
        foreach ($employees as $employee)
        {
          $employee_code.='<OPTION VALUE="'.addslashes($employee["id"]).'">'.$employee["lastname"].', '.$employee["firstname"].'</OPTION>';
        }
        
        return $employee_code;
      }
      
      return null;
    }
    
    /**
     * Get all levels below level of logged-in user
     *
     * @return $array levels
     */
    function getFunctionLevels()
    {
      $level = $this->getLevel($this->getLevelIdFromUser());
      $maxLevel = $this->getLevel($this->getMaxLevelId());
      
      if ($level == $maxLevel)
      {
        $levels = $this->getLevelsFromDB();        
      }
      else 
      {
        $where = "functionlevel.name < $level";
        
        $levels = $this->getLevelsFromDB($where);
      }
      
      if (is_array($levels) && count($levels) > 0)
        return $levels;
        
      return null;
    }
    
    /**
     * Get all levels below a specified level
     *
     * @param $include_same_level 
     * @return $array levels
     */    
    function getFunctionLevelBelow($level_id, $include_same_level=false)
    {      
      if ($level_id == "all")
      {
        $where = 1;
      }
      else if (!is_numeric($level_id)) 
      {
        atkdebug("functionlevelutils::getFunctionLevelBelow | level_id $level_id is not numeric");
        return "";
      }
      else 
      {
        $level = $this->getLevel($level_id);
        if ($include_same_level) $where = "functionlevel.name <= $level";
        else $where = "functionlevel.name < $level";
      }
      
      $levels = $this->getLevelsFromDB($where);
      
      return $levels;
    }
    
    
    
    function getLevelsFromDB($where = 1)
    {
      $db = &atkGetDb();
      
      $sql = "SELECT functionlevel.name, functionlevel.id FROM functionlevel WHERE $where ORDER BY functionlevel.name DESC";
      
      $records = $db->getRows($sql);
      
      if (is_array($records) && count($records) > 0)
        return $records;
      
      return null;
    }
    
    
    
    function getFunctionLevelsDropDown($levels, $selected_level_id)
    {
      if (is_array($levels) && count($levels) > 0)
      {
        if (!empty($selected_level_id) && $selected_level_id == "all")  
          $sel = "selected";
        else
          $sel = "";
        
        $functionlevel_code = '<SELECT name="functionlevelswitch"><OPTION VALUE="all"'.$sel.'>'.atktext("allfunctionlevels").'</OPTION>';
        
        foreach ($levels as $level)
        {
          if (!empty($selected_level_id) && $selected_level_id == $level["id"])  
            $sel = "selected";
          else
            $sel = "";
          
          $functionlevel_code.='<OPTION VALUE="'.addslashes($level["id"]).'" '.$sel.'>'.$level["name"].'</OPTION>';
        }
        
        $functionlevel_code .= '</SELECT>';
        
        return $functionlevel_code;
      }
      
      return null;
    }
    
    function getCheckbox($name, $lowerlevels)
    {
      if ($lowerlevels == 'on') $checked = "CHECKED";
      else $checked = "";
      
      return '<INPUT TYPE=CHECKBOX '.$checked.' NAME="'.$name.'">'.atktext('lower_levels', 'advancedsecurity');
    }
  }
?>