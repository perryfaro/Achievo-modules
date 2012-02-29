<?php
  /**
   * casetype class file
   *
   * @package achievo_modules
   * @subpackage legal
   *
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2005 Ibuildings.nl BV
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision: 3236 $
   * $Id: class.casetype.inc 3236 2006-07-25 09:21:26Z guido $
   */

  /**
   * casetype class
   *
   * @author guido <guido@ibuildings.nl>
   * @package achievo_modules
   * @subpackage legal
   */
  class casetype extends atkNode {

    /**
     * casetype class constructor
     */
    function casetype()
    {
      $this->atkNode("casetype", NF_TRACK_CHANGES);

      $this->add(new atkAttribute("id", AF_AUTOKEY));
      $this->add(new atkAttribute("number", AF_OBLIGATORY));
      $this->add(new atkAttribute("description", AF_OBLIGATORY));

      atkimport("module.legal.updatecasetypelistener");
      $this->addListener(new updateCasetypeListener(array("update")));

      $this->setOrder("legal_casetype.number ASC");
      $this->setTable("legal_casetype");
    }

    /**
     * Defines the display template for records
     *
     * @return String Description template used when displaying the record in een manytoonerelation
     */
    function descriptor_def()
    {
      return "[number]. [description]";
    }

  }
?>