<?php

  /**
   * Leaves can be calculated in three different ways:
   * LC_HOURS            Only time registrations will affect the amount of leave taken (default)
   * LC_REQUESTS         Only approved requests will affect the amount of leave taken
   * LC_HOURSANDREQUESTS Past time registrations and future approved requests will be combined when calculating the leave taken
   */
  $config["leavecalculation"] = LC_HOURS;

  /**
   * Number of hours that goes into a fulltime week (40 by default)
   */
  $config["fulltime"] = 40;

  /**
   * Do we round the amount of leave given per employee in favour of the employee (true by default)
   */
  $config["round"] = true;

?>