<?php

  useattrib("atkcurrencyattribute");

  /**
   * The node class for product.
   */
  class product extends atkNode
  {
    function product()
    {
      $this->atkNode("product", NF_ADD_LINK);
      $this->add(new atkAttribute("id", AF_AUTOKEY));
            
      $this->add(new atkAttribute("name", AF_SEARCHABLE, 100));
      $this->add(new atkAttribute("code", AF_SEARCHABLE, 15));
      $this->add(new atkCurrencyAttribute("unitprice"));
      
      $this->setIndex("name");
      $this->setOrder("name");
      $this->setTable("stock_product");
    }
    
    /**
     * Set up default values for this node
     * @return array Associative array where the key is
     *               the attribute name, and value is the
     *               default value.
     */
    function initial_values()
    {    
      return array();
    }
    
    /**
     * Return record descriptor (for use in relations etc.)
     * @return String A template containing fieldnames between brackets.
     */
    function descriptor_def()
    {
      // @todo: replace this with proper descriptor fields.
      return "[name]";
    }
  }

?>