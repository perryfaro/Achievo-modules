<?php

  /**
   * The node class for fund.
   */
  class fund extends atkNode
  {
    function fund()
    {
      $this->atkNode("fund");
      $this->add(new atkAttribute("id", AF_AUTOKEY));
      $this->add(new atkAttribute("name", AF_OBLIGATORY, 50));

      $this->setOrder("name");
      $this->setTable("billing_fund");
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