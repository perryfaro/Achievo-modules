<?php

class mod_location extends atkModule
{
  function getNodes()
  {
    registerNode("location.address",array("admin","add","edit","delete"));
  }

  function getMenuItems()
  {
    menuitem("location", dispatch_url("location.address","admin"),"main");
  }

  function getDescription()
  {
    return "An extensible location module"; 
  }
}

?>
