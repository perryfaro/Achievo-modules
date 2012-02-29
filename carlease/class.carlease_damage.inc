<?php

  //1-N relation carlease - carlease_maintenance
  useattrib("atkdateattribute");
  useattrib("atknumberattribute");
  useattrib("atkcurrencyattribute");
  useattrib("atktextattribute");
  
  userelation("atkmanytoonerelation");

  class carlease_damage extends atkNode
  {
    function carlease_damage()
    {
      //create new node carlease
      $this->atkNode("carlease_damage", NF_ADD_LINK);
      //set id field as primary key
      $this->add(new atkAttribute("id",AF_AUTOKEY));
      $this->add(new atkAttribute("description", AF_SEARCHABLE));
      $this->add(new atkDateAttribute("date",AF_OBLIGATORY));
      $this->add(new atkCurrencyAttribute("amount",0, 10, "&euro;"));
      $this->add(new atkTextAttribute("remark", 10, AF_HIDE_LIST));
      $this->add(new atkManyToOneRelation("car_id","carlease.carlease", AF_HIDE));
      
      $this->setOrder("date");
      
      //$this->setIndex("date");
      
      $this->setTable("carlease_damage");
    }
    
    function descriptor_def()
    {
      //return "[licenseNumber]";
    }
  }

?>