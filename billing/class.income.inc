<?php

  useattrib("atkdateattribute");
  useattrib("atkcurrencyattribute");
  userelation("atkmanytoonerelation");

  /**
   * The node class for income.
   */
  class income extends atkNode
  {
    function income()
    {
      $this->atkNode("income");
      $this->add(new atkAttribute("id", AF_AUTOKEY));

      $this->add(new atkManyToOneRelation("fund_id", "billing.fund", AF_RELATION_AUTOLINK));
      $this->add(new atkAttribute("remark", 0, 100));
      $this->add(new atkCurrencyAttribute("amount", 0, 7, "EUR"));

      $this->add(new atkManyToOneRelation("project_id", "project.project", AF_HIDE|AF_OBLIGATORY));

      // Tracking
      $this->add(new atkManyToOneRelation("userid","employee.employee", AF_HIDE|AF_FORCE_LOAD|AF_OBLIGATORY|AF_READONLY));
      $this->add(new atkDateAttribute("entrydate",AF_HIDE_ADD|AF_READONLY|AF_OBLIGATORY));

      $this->setOrder("entrydate");
      $this->setTable("billing_income");
    }

    /**
     * Set up default values for this node
     * @return array Associative array where the key is
     *               the attribute name, and value is the
     *               default value.
     */
    function initial_values()
    {
      $user = &getUser();
      return array("userid"=>array("id"=>$user["id"]),
                   "entrydate"=>array("year"=>date("Y"),
                     "month"=>date("m"),
                     "day"=>date("d")));
    }

    /**
     * Return record descriptor (for use in relations etc.)
     * @return String A template containing fieldnames between brackets.
     */
    function descriptor_def()
    {
      /** @todo: replace this with proper descriptor fields. */
      return "[remark]";
    }
  }

?>