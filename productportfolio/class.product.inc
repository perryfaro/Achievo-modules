<?php

  /**
   * product node class file
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
   * $Id: class.product.inc 3322 2006-08-11 15:13:52Z guido $
   */

  /**
   * @internal includes
   */
  atkimport("atk.atktreenode");

  /**
   * The node class for product.
   *
   * @author guido <guido@ibuildings.nl>
   * @package atk_modules
   * @subpackage productportfolio
   */
  class product extends atkTreeNode
  {
    /**
     * product constructor
     */
    function product()
    {
      $this->atkTreeNode("product", NF_EDITAFTERADD);

      useattrib("atkAttribute");
      useattrib("atkBoolAttribute");
      useattrib("atkListAttribute");
      useattrib("atkNumberAttribute");
      useattrib("atkFckAttribute");
      userelation("atkManyToOneTreeRelation");
      userelation("atkOneToManyRelation");
      userelation("atkShuttleRelation");

      $module = &getModule("productportfolio");
      $producttypes = $module->getProductTypes();
      $marketareas = $module->getMarketAreas();

      $this->add(new atkNumberAttribute("id", AF_AUTOKEY));
      $this->add(new atkAttribute("name", AF_SEARCHABLE|AF_OBLIGATORY));
      $this->add(new atkManyToOneTreeRelation("superproduct_id", "productportfolio.product", AF_HIDE_ADD|AF_PARENT));
      $this->add(new atkFckAttribute("description", '', AF_HIDE_LIST|AF_HIDE_ADD));
      $this->add(new atkAttribute("tags", AF_SEARCHABLE|AF_HIDE_ADD));
      $this->add(new atkFckAttribute("usps", '', AF_HIDE_LIST|AF_HIDE_ADD), "usps");
      $this->add(new atkShuttleRelation("targetgroups", "productportfolio.product_targetgroup", "productportfolio.targetgroup", AF_HIDE_LIST|AF_HIDE_ADD));
      $this->add(new atkFckAttribute("conditions", '', AF_HIDE_LIST|AF_HIDE_ADD), "conditions");
      $this->add(new atkFckAttribute("costsandparameters", '', AF_HIDE_LIST|AF_HIDE_ADD), "costsandparameters");
      $this->add(new atkListAttribute("marketarea", array_values($marketareas), array_keys($marketareas), AF_HIDE_LIST|AF_HIDE_ADD));
      $subprods = &$this->add(new atkOneToManyRelation("subproducts", "productportfolio.product", "superproduct_id", AF_HIDE_LIST|AF_HIDE_ADD));
      $subprods->setDestinationFilter("productportfolio_product.uppselproduct != 1");
      $uppselprods = &$this->add(new atkOneToManyRelation("uppselproducts", "productportfolio.product", "superproduct_id", AF_HIDE_LIST|AF_HIDE_ADD));
      $uppselprods->setDestinationFilter("productportfolio_product.uppselproduct = 1");
      $this->add(new atkOneToManyRelation("representations", "productportfolio.representation", "product_id", AF_HIDE_LIST|AF_HIDE_ADD));
      $this->add(new atkBoolAttribute("uppselproduct", AF_HIDE_LIST|AF_HIDE_ADD));

      $this->setTable("productportfolio_product");
      $this->setOrder("productportfolio_product.name ASC");
      $this->setIndex("name");
      $this->setDescriptorTemplate("[name]");
    }

    function tags_edit($record="", $fieldprefix="", $mode="")
    {
      $tagsattr = &$this->getAttribute("tags");
      $result = $this->text("enter_a_comma_seperated_list_of_tags_here") . ":<br>";
      $result.= $tagsattr->edit($record, $fieldprefix, $mode);
      return $result;
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