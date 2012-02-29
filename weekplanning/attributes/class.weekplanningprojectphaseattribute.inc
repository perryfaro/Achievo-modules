<?php
  /**
   * weekScheduleProjectPhaseAttribute attribute class file
   *
   * @package achievo_modules
   * @subpackage weekschedule
   *
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2005 Ibuildings.nl BV
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision: 3604 $
   * $Id: class.weekplanningprojectphaseattribute.inc 3604 2005-11-16 17:24:55Z guido $
   */

  // Load parent attribute class file
  useattrib("project.projectphaseattrib");

  /**
   * weekScheduleProjectPhaseAttribute attribute class
   *
   * @author guido <guido@ibuildings.nl>
   * @package achievo_modules
   * @subpackage weekschedule
   */
  class weekPlanningProjectPhaseAttribute extends atkProjectPhaseAttribute
  {

    /**
     * Constructor
     */
    function weekPlanningProjectPhaseAttribute($name, $flags=0)
    {
      parent::atkProjectPhaseAttribute($name, $flags);
    }

    /**
     * Adds this attribute to database queries.
     *
     * Replaces the addToQuery function in atkProjectPhaseAttribute
     *
     * Database queries (select, insert and update) are passed to this method
     * so the attribute can 'hook' itself into the query.
     *
     * Framework method. It should not be necessary to call this method
     * directly. Derived attributes that consist of more than a single simple
     * database field (like relations for example), may have to reimplement
     * this method.
     *
     * @param atkQuery $query The SQL query object
     * @param String $tablename The name of the table of this attribute
     * @param String $fieldaliasprefix Prefix to use in front of the alias
     *                                 in the query.
     * @param int $level Recursion level if relations point to eachother, an
     *                   endless loop could occur if they keep loading
     *                   eachothers data. The $level is used to detect this
     *                   loop. If overriden in a derived class, any subcall to
     *                   an addToQuery method should pass the $level+1.
     * @param String $mode Indicates what kind of query is being processing:
     *                     This can be any action performed on a node (edit,
     *                     add, etc) Mind you that "add" and "update" are the
     *                     actions that store something in the database,
     *                     whereas the rest are probably select queries.
     * @return type description
     */
    function addToQuery(&$query, $tablename = "", $fieldaliasprefix = "", $rec = "", $level = 0, $mode = "")
    {
      return atkAttribute::addToQuery($query, $tablename, $fieldaliasprefix, $rec, $level, $mode);
    }

    /**
     * Reads this attributes value from a weekschedule record
     *
     * @param array Record
     * @return array Projectname, phasename and phaseid values
     */
    function db2value($rec)
    {
      $phasenode = &getNode("project.phase");
      $excludelist = $this->_getExcludeListForNode($phasenode, array("projectid","name"));
      $phasedata = $phasenode->selectDb("phase.id = '{$rec["phaseid"]}'", null, null, $excludelist);
      return Array("projname" => $phasedata[0]["projectid"]["name"],
                   "phaseid" => $rec["phaseid"],
                   "phasename" => $phasedata[0]["name"]);
    }

  }
?>