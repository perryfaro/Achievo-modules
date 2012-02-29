<?php
  /**
   * points class file
   *
   * @package achievo_modules
   * @subpackage legal
   *
   * @author patrick <patrick@ibuildings.nl>
   *
   * @copyright (c) 2006 Ibuildings.nl BV
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision: 3228 $
   * $Id: class.points.inc 3228 2006-06-28 16:07:49Z guido $
   */

  useattrib("atkDateAttribute");
  useattrib("atkNumberAttribute");
  useattrib("legal.legalPointsAttribute");
  userelation("atkManyToOneRelation");

  /**
   * points class
   *
   * @author patrick <patrick@ibuildings.nl>
   * @package achievo_modules
   * @subpackage legal
   */
  class points extends atkNode
  {

    /**
     * points class constructor
     */
    function points()
    {
      $this->atkNode("points");

      $this->add(new atkAttribute("id", AF_PRIMARY|AF_HIDE|AF_AUTO_INCREMENT));
      $this->add(new atkDateAttribute("entrydate", AF_OBLIGATORY));
      $this->add(new legalPointsAttribute("points", AF_OBLIGATORY|AF_TOTAL));
      $this->add(new atkManyToOneRelation("projectid","project.project", AF_OBLIGATORY|AF_HIDE));

      $this->setTable("legal_points");
      $this->setOrder("legal_points.entrydate ASC");
      $this->setSecurityAlias("project.project");
    }

  }
?>