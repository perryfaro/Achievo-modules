<?php

  useattrib("atkupdatedbyattribute");
  useattrib("atkupdatestampattribute");
  useattrib("atklistattribute");
  userelation("atkmanytoonerelation");     
  userelation("atkonetomanyrelation");

  

  /**
   * The node class for product.
   */
  class order extends atkNode
  {
    function order()
    {
      $this->atkNode("order", NF_ADD_LINK|NF_EDITAFTERADD);
      $this->add(new atkAttribute("id", AF_AUTOKEY));
                              
      $this->add(new atkManyToOneRelation("project_id", "project.project"));
      $this->add(new atkManyToOneRelation("supplier_id", "organization.organization"));
      
      $this->add(new atkAttribute("bill_no", AF_SEARCHABLE|AF_HIDE_ADD, 15));      
      
      $this->add(new atkListAttribute("status", array("order", "received", "booked"), "", AF_SEARCHABLE|AF_HIDE_ADD));
      
      $this->add(new atkUpdatedByAttribute("user_id", AF_HIDE_ADD|AF_HIDE_LIST));
      $this->add(new atkUpdateStampAttribute("entrydate", AF_HIDE_ADD));            
      
      $this->add(new atkOneToManyRelation("orderlines", "purchase.order_line", "order_id", AF_HIDE_LIST));
            
      $this->setOrder("entrydate");
      $this->setTable("purchase_order");
    }
    
    function initial_values()
    {
      return array("status"=>"order");
    }
            
    /**
     * Return record descriptor (for use in relations etc.)
     * @return String A template containing fieldnames between brackets.
     */
    function descriptor_def()
    {
      // @todo: replace this with proper descriptor fields.
      return "[bill_no]";
    }
  }

?>