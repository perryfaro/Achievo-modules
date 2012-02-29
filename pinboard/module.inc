<?php

  class mod_pinboard extends achievoModule 
  {
    function getMenuItems()
    {
      menuitem("pinboard", dispatch_url("pinboard.pinboard", "admin"), "main",true,600);
    }
    
    function getNodes()
    {
      registerNode("pinboard.pinboard", array("admin","add","delete","edit"));
    }
    
    function getDescription()
    {
      return "Pinboard Module.";
    }
    
    function getPimItems()
    {
      include_once(moduleDir("pinboard")."pim.inc");
      $ret = pimPinBoard();
      if($ret=="") 
      { 
        return "";
      }
      else 
      {
        return array("pinboard"=>$ret);
      }
    }  
  }
?>
