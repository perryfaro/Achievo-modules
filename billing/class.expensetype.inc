<?php

  useattrib("atkboolattribute");
  useattrib("atkcurrencyattribute");

  class expensetype extends atkNode
  {
    function expensetype()
    {
      $this->atkNode("expensetype", NF_MRA);

      $this->add(new atkAttribute("id",AF_AUTOKEY));
      $this->add(new atkAttribute("name",AF_OBLIGATORY, 100));
      $this->add(new atkAttribute("description","", 100));
      $this->add(new atkBoolAttribute("remarkrequired"));
      $this->add(new atkBoolAttribute("km_specification"));
      $this->add(new atkCurrencyAttribute("km_price",AF_HIDE_LIST));

      $this->setTable("billing_expensetype");
      $this->setOrder("id");
      $this->setDescriptorTemplate("[name]");
    }
  }
?>