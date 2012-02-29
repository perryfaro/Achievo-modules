<?php

  /**
   * @internal includes
   */
  atkimport("atk.atktreenode");
  userelation("atkmanytoonetreerelation");
  userelation("atkonetomanyrelation");

  /**
   * The node class for productgroup.
   */
  class productgroup extends atkTreeNode
  {
    function productgroup()
    {
      $this->atkTreeNode("productgroup", NF_ADD_LINK);
      $this->add(new atkAttribute("id", AF_AUTOKEY));
      $this->add(new atkAttribute("name", AF_SEARCHABLE, 100));
      $this->add(new atkManyToOneTreeRelation("superproductgroup_id", "stock.productgroup", AF_HIDE_ADD|AF_PARENT));
      $this->add(new atkOneToManyRelation("subproductgroups", "stock.productgroup", "superproductgroup_id", AF_HIDE_LIST|AF_HIDE_ADD));
      $this->add(new atkAttribute("code", AF_SEARCHABLE, 15));      
      
      $this->setIndex("name");
      $this->setOrder("name");
      $this->setTable("stock_productgroup");
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