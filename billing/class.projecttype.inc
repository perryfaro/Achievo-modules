<?php
  /**
   * projecttype class file
   *
   * @package achievo_modules
   * @subpackage billing
   *
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2005 Ibuildings.nl BV
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision: 2725 $
   * $Id: class.projecttype.inc 2725 2005-11-22 20:16:00Z ivo $
   */

  // Load the used attribute classes
  useattrib("atkcurrencyattribute");

  /**
   * projectype class
   *
   * @author guido <guido@ibuildings.nl>
   * @package achievo_modules
   * @subpackage billing
   */
  class projecttype extends atkNode {

    /**
     * projecttype class constructor
     */
    function projecttype()
    {
      $this->atkNode("projecttype");

      $this->add(new atkAttribute("id", AF_AUTOKEY));
      $this->add(new atkAttribute("description", AF_OBLIGATORY));

      $this->setOrder("description");
      $this->setTable("billing_projecttype");
    }

    /**
     * Defines the display template for records
     *
     * @return String Description template used when displaying the record in een manytoonerelation
     */
    function descriptor_def()
    {
      return "[description]";
    }

  }
?>