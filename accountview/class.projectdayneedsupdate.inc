<?php
  /**
   * projectdayneedsupdate node class file
   *
   * @package achievo_modules
   * @subpackage accountview
   *
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2006 Ibuildings.nl BV
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision: 2502 $
   * $Id: class.projectdayneedsupdate.inc 2502 2006-05-03 00:22:21Z guido $
   */

  /**
   * @internal Load used files
   */
  useattrib("atknumberattribute");
  useattrib("atkdateattribute");
  userelation("atkmanytoonerelation");

  /**
   * The node class for projectdayneedsupdate.
   *
   * @author guido <guido@ibuildings.nl>
   * @package achievo_modules
   * @subpackage accountview
   */
  class projectdayneedsupdate extends atkNode
  {
    /**
     * projectdayneedsupdate constructor
     */
    function projectdayneedsupdate()
    {
      $this->atkNode("projectdayneedsupdate");
      $this->add(new atkNumberAttribute("id", AF_AUTOKEY));
      $this->add(new atkManyToOneRelation("project_id", "project.project", AF_OBLIGATORY));
      $this->add(new atkDateAttribute("day", AF_OBLIGATORY));
      $this->setTable("projectdayneedsupdate");
    }

    /**
     * Adds the projectdayneedsupdate record if it doesn't exist yet for the given project/day
     *
     * @param int $projectid Id of the project
     * @param Array $day Date array representing the date
     */
    function setProjectDayNeedsUpdate($project_id, $day)
    {
      // Build the projectdayneedsupdate record
      $record = array(
        "project_id"=>$project_id,
        "day"=>$day,
      );

      // Try to find if this projectday already needs an update
      $dayattribute = &$this->getAttribute("day");
      $condition = sprintf("`projectdayneedsupdate`.`project_id`='%d'", $project_id["id"]);
      $condition.= sprintf(" AND `projectdayneedsupdate`.`day`='%s'", $dayattribute->value2db($record));
      $needsupdate = $this->countDb($condition);

      // If it the projectdayneedsupdate record doesn't exist yet, add it to the db
      if ($needsupdate == 0)
      {
        $this->addDb($record);
      }
    }
  }
?>