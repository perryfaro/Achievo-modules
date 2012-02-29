<?php

  /**
   * product_targetgroup node class file
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
   * $Id: class.product_targetgroup.inc 3322 2006-08-11 15:13:52Z guido $
   */

  /**
   * The node class for product_targetgroup.
   *
   * @author guido <guido@ibuildings.nl>
   * @package atk_modules
   * @subpackage productportfolio
   */
  class product_targetgroup extends atkNode
  {
    /**
     * product_targetgroup constructor
     */
    function product_targetgroup()
    {
      $this->atkNode("product_targetgroup", NF_ADD_LINK);

      userelation("atkManyToOneRelation");

      $this->add(new atkManyToOneRelation("product_id", "productportfolio.product", AF_PRIMARY));
      $this->add(new atkManyToOneRelation("targetgroup_id", "productportfolio.targetgroup", AF_PRIMARY));

      $this->setTable("productportfolio_product_targetgroup");
    }
  }

?>