<?php
  /**
   * bill_perproject node file
   *
   * This node disables the add bill function in a project screen and may
   * eventually re-enable and implement a fixed add function in the future.
   *
   * @package achievo_modules
   * @subpackage billing
   *
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2006 Ibuildings.nl BV
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision: 2798 $
   * $Id: class.bill_perproject.inc 2798 2006-07-07 18:00:47Z guido $
   */

  // Load used classes
  atkimport("module.billing.bill");

  /**
   * bill class
   *
   * @author guido <guido@ibuildings.nl>
   * @package achievo_modules
   * @subpackage billing
   */
  class bill_perproject extends bill
  {

    /**
     * Constructor
     */
    function bill_perproject()
    {
      // Call the parent constructor
      $this->bill("bill_perproject", NF_NO_ADD);
      $this->setSecurityAlias("billing.bill");
    }
  }

?>