<?php
  /**
   * Installation file for weekschedule module
   *
   * @package achievo_modules
   * @subpackage weekschedule
   *
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2005 Ibuildings.nl BV
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision: 3665 $
   * $Id: install.inc 3665 2006-05-22 23:51:50Z guido $
   */

  // Install the used node tables upon first detection while running setup
  $setup->installNode("weekplanning.weekplanning");
  $setup->installNode("weekplanning.weekplanningconflictlog");

  // Add weekplanning_employee_budget column to project table
  $setup->addColumn("project", "weekplanning_employee_budget", "decimal(13,5)", true);

  // Add weekplanning_employee_budget column to project table
  if (moduleExists("billing"))
  {
    $setup->addColumn("billing_projecttype", "weekplanning_avaragerate", "decimal(10,2)", true);
  }

  // Set the weekschedule version numer in the versioninfo table
  $setup->setVersion(7);

?>