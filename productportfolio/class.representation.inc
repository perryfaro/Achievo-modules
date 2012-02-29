<?php

  /**
   * representation node class file
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
   * $Id: class.representation.inc 3322 2006-08-11 15:13:52Z guido $
   */

  /**
   * The node class for representation.
   *
   * @author guido <guido@ibuildings.nl>
   * @package atk_modules
   * @subpackage productportfolio
   */
  class representation extends atkNode
  {
    /**
     * representation constructor
     */
    function representation()
    {
      $this->atkNode("representation", NF_ADD_LINK);

      useattrib("atkAttribute");
      useattrib("atkListAttribute");
      useattrib("atkNumberAttribute");
      useattrib("atkFckAttribute");
      useattrib("atkUrlAttribute");
      userelation("atkManyToOneRelation");
      userelation("atkShuttleRelation");

      $module = &getModule("productportfolio");
      $marketareas = $module->getMarketAreas();

      $this->add(new atkNumberAttribute("id", AF_AUTOKEY));
      $this->add(new atkManyToOneRelation("product_id", "productportfolio.product", AF_OBLIGATORY));
      $this->add(new atkAttribute("name", AF_OBLIGATORY));
      $this->add(new atkFckAttribute("description", '', AF_HIDE_LIST));
      $this->add(new atkUrlAttribute("file_url"));
      $this->add(new atkListAttribute("marketarea", array_values($marketareas), array_keys($marketareas)));
      $this->add(new atkUrlAttribute("product_url"));
      $this->add(new atkShuttleRelation("targetgroups", "productportfolio.representation_targetgroup", "productportfolio.targetgroup"));

      $this->setTable("productportfolio_representation");
      $this->setOrder("productportfolio_representation.name ASC");
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