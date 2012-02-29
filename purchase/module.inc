<?php

  class mod_purchase extends achievoModule 
  {
    function getMenuItems()
    {
      $this->menuitem("purchasing");
      $this->menuitem("orders", dispatch_url("purchase.order", "admin"), "purchasing", array("purchase.order", "admin"));      
      $this->menuitem("order_report", dispatch_url("purchase.order_report", "report"), "reports", array("purchase.order_report", "report"));
    }
    
    function getNodes()
    {
      registerNode("purchase.order", array("admin", "add", "edit", "delete"));      
      registerNode("purchase.order_report",array("report"));
    }
  }
  
?>