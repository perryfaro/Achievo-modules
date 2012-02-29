<?php

  class test_getFullContractDays extends UnitTestCase
  {
    function test_leaveHours()
    {
      require_once(moduleDir("holidayreport")."module.inc");
      
      $contracts = array();
      $contracts[] = array("startdate"=>"2007-01-01","enddate"=>"2007-12-31","uc_hours"=>40);
      $year = '2007';

      $this->assertEqual($this->getLeaveHours(getFullContractDays($contracts,$year.'0101',$year.'1231')), 200.00);

      $contracts = array();
      $contracts[] = array("startdate"=>"2007-06-01","enddate"=>"2007-12-31","uc_hours"=>40);
      $year = '2007';
      
      $this->assertEqual($this->getLeaveHours(getFullContractDays($contracts,$year.'0101',$year.'1231')), 117.26);

      $contracts = array();
      $contracts[] = array("startdate"=>"2007-06-01","enddate"=>"2008-06-01","uc_hours"=>24);
      $contracts[] = array("startdate"=>"2007-08-13","enddate"=>"2007-10-02","uc_hours"=>16);

      $this->assertEqual($this->getLeaveHours(getFullContractDays($contracts,$year.'0101',$year.'1231')), 81.53);

      $contracts[] = array("startdate"=>"2007-01-10","enddate"=>"2007-04-20","uc_hours"=>40);

      $this->assertEqual($this->getLeaveHours(getFullContractDays($contracts,$year.'0101',$year.'1231')), 136.88);
      
     $contracts[2] = array("startdate"=>"2007-05-07","enddate"=>"2007-07-31","uc_hours"=>8);

     $this->assertEqual($this->getLeaveHours(getFullContractDays($contracts,$year.'0101',$year.'1231')), 90.96);
    }
    
    function getLeaveHours($days)
    {
      return number_format($days/365 * 200,2,".","");
    }
  }

?>