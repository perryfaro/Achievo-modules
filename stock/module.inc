<?php

  class mod_stock extends achievoModule 
  {
    function getMenuItems()
    {
      $this->menuitem("stock");
      $this->menuitem("products", dispatch_url("stock.product", "admin"), "stock", array("stock.product", "admin"));
      $this->menuitem("handout", dispatch_url("stock.handout", "admin"), "stock", array("stock.handout", "admin"));
      $this->menuitem("productgroup", dispatch_url("stock.productgroup", "admin"), "stock", array("stock.productgroup", "admin"));
    }
    
    function getNodes()
    {
      registerNode("stock.product", array("admin", "add", "edit", "delete"));      
      registerNode("stock.handout", array("admin", "add", "edit", "delete"));
      registerNode("stock.productgroup", array("admin", "add", "edit", "delete"));
    }
  }
  
?>