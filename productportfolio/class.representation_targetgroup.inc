<?php

  /**
   * representation_targetgroup node class file
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
   * $Id: class.representation_targetgroup.inc 3322 2006-08-11 15:13:52Z guido $
   */

  /**
   * The node class for representation_targetgroup.
   *
   * @author guido <guido@ibuildings.nl>
   * @package atk_modules
   * @subpackage productportfolio
   */
  class representation_targetgroup extends atkNode
  {
    /**
     * representation_targetgroup constructor
     */
    function representation_targetgroup()
    {
      $this->atkNode("representation_targetgroup", NF_ADD_LINK);

      userelation("atkManyToOneRelation");

      $this->add(new atkManyToOneRelation("representation_id", "productportfolio.representation", AF_PRIMARY));
      $this->add(new atkManyToOneRelation("targetgroup_id", "productportfolio.targetgroup", AF_PRIMARY));

      $this->setTable("productportfolio_representation_targetgroup");
    }
  }

?>