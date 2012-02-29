<?php

  /**
   * targetgroup node class file
   *
   * @package atk_modules
   * @subpackage productportfolio
   *
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2006 Ibuildings.nl BV
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision: 3322 $
   * $Id: class.targetgroup.inc 3322 2006-08-11 15:13:52Z guido $
   */

  /**
   * The node class for targetgroup.
   *
   * @author guido <guido@ibuildings.nl>
   * @package atk_modules
   * @subpackage productportfolio
   */
  class targetgroup extends atkNode
  {
    /**
     * targetgroup constructor
     */
    function targetgroup()
    {
      $this->atkNode("targetgroup", NF_ADD_LINK);

      useattrib("atkAttribute");
      useattrib("atkNumberAttribute");
      useattrib("atkFckAttribute");
      userelation("atkShuttleRelation");

      $this->add(new atkNumberAttribute("id", AF_AUTOKEY));
      $this->add(new atkAttribute("name", AF_OBLIGATORY));
      $this->add(new atkFckAttribute("comment"));

      $this->setTable("productportfolio_targetgroup");
      $this->setOrder("productportfolio_targetgroup.name ASC");
      $this->setIndex("name");
      $this->setDescriptorTemplate("[name]");
    }

    /**
     * Set up default values for this node
     *
     * @return array Array containing Attributename=>Initialvalue pairs
     */
    function initial_values()
    {
      return array();
    }
  }

?>