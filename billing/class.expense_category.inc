<?php

  userelation("atkmanytoonerelation");

  class expense_category extends atkNode
  {
    function expense_category()
    {
      $this->atkNode("expense_category");
      
      $this->add(new atkAttribute("id", AF_AUTOKEY));
      $rel = &$this->add(new atkManyToOneRelation("pid","billing.expense_category",AF_SEARCHABLE));
      $rel->addDestinationFilter("pid IS NULL");
      $this->add(new atkAttribute("category", AF_OBLIGATORY));

      $this->setTable("billing_expense_category");
      $this->setDescriptorTemplate("[category]");
      $this->setSecurityAlias("billing.taxrate");
    }

  }
?>