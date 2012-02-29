<?php
  /**
   * taxrate node class file
   *
   * @package achievo_modules
   * @subpackage billing
   *
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2006 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing/ ATK open source license
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision: 2751 $
   * $Id: class.taxrate.inc 2751 2006-05-03 15:35:28Z guido $
   */

  useattrib("atknumberattribute");

  /**
   * The node class for taxrate.
   *
   * @author guido <guido@ibuildings.nl>
   * @package achievo_modules
   * @subpackage billing
   */
  class taxrate extends atkNode
  {
    /**
     * taxrate constructor
     */
    function taxrate()
    {
      $this->atkNode("taxrate");
      $this->add(new atkAttribute("id", AF_AUTOKEY));
      $this->add(new atkAttribute("name", AF_OBLIGATORY));
      $percentageattr = $this->add(new atkNumberAttribute("percentage", AF_FORCE_LOAD, 10, 1));
      $percentageattr->m_decimals = 1;

      $this->setTable("billing_taxrate");
      $this->setOrder("billing_taxrate.percentage DESC");
      $this->setDescriptorTemplate("[name] ([percentage] %)");
    }

    function percentage_display($record, $mode="")
    {
      if (isset($record["percentage"]) && $record["percentage"]!=="")
      {
        $displayvalue = atkNumberAttribute::translateSeparator(sprintf("%0.1f %%", $record["percentage"]));
      }
      else
      {
        $displayvalue = atktext("notax", "billing");
      }
      return $displayvalue;
    }
  }
?>