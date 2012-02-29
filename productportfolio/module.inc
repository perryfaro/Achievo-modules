<?php

  /**
   * productportfolio module class file
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
   * $Id: module.inc 3322 2006-08-11 15:13:52Z guido $
   */

  /**
   * @internal Product type constants
   */
  define("PT_PRODUCT", 1);
  define("PT_PRODUCTGROUP", 2);

  /**
   * @internal Market area Constants
   */
  define("MA_COUNTRY", 1);
  define("MA_CONTINENTAL", 2);
  define("MA_GLOBAL", 3);


  /**
   * The module class for productportfolio.
   *
   * @author guido <guido@ibuildings.nl>
   * @package atk_modules
   * @subpackage productportfolio
   */
  class mod_productportfolio extends atkModule
  {

    /**
     * Register nodes and secured actions
     */
    function getNodes()
    {
      registerNode("productportfolio.product",        array("admin", "add", "edit", "delete"));
      registerNode("productportfolio.representation", array("admin", "add", "edit", "delete"));
      registerNode("productportfolio.targetgroup",    array("admin", "add", "edit", "delete"));
    }

    /**
     * Register menu items
     */
    function getMenuItems()
    {
      $this->menuitem("productportfolio","",                                                       "main",             array("productportfolio.product", "admin", "productportfolio.targetgroup", "admin"));
      $this->menuitem("products",        dispatch_url("productportfolio.product", "admin"),        "productportfolio", array("productportfolio.product", "admin"));
      $this->menuitem("targetgroups",    dispatch_url("productportfolio.targetgroup", "admin"),    "productportfolio", array("productportfolio.targetgroup", "admin"));
    }

    /**
     * Gets an associative array of producttypes
     *
     * @return array of producttypes (productkey => producttypename pairs)
     */
    function getProductTypes()
    {
      return array(PT_PRODUCT=>"product", PT_PRODUCTGROUP=>"productgroup");
    }

    /**
     * Gets an associative array of marketareas
     *
     * @return array of marketareas (marketareatkey => marketareaname pairs)
     */
    function getMarketAreas()
    {
      return array(MA_COUNTRY=>"country", MA_CONTINENTAL=>"continental", MA_GLOBAL=>"global");
    }

  }

?>