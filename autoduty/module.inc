<?php

class mod_autoduty extends atkModule
{
	function getNodes()
	{
		registerNode("autoduty.job",array("admin","add","edit","delete"));
		registerNode("autoduty.joblog",array("admin","edit", "delete"));		
	}
	
	function getMenuItems()
	{
		$this->menuitem("autoduty","","main", array("autoduty.joblog", "admin"));
		$this->menuitem("jobadmin", dispatch_url("autoduty.job","admin"),"autoduty");
		$this->menuitem("-","","autoduty");
		$this->menuitem("joblog", dispatch_url("autoduty.joblog","admin"),"autoduty");      				
	}
	
	function getDescription()
	{
	  return "A simple duty planner"; 
	}
}

?>
