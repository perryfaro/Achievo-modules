<?php

  //1-N relation carlease - carlease_maintenance
  useattrib("atkdateattribute");
  useattrib("atknumberattribute");
  useattrib("atkcurrencyattribute");
  
  userelation("atkmanytoonerelation");

  class carlease_maintenance extends atkNode
  {
    function carlease_maintenance()
    {
      //create new node carlease
      $this->atkNode("carlease_maintenance", NF_ADD_LINK);
      //set id field as primary key
      $this->add(new atkAttribute("id",AF_AUTOKEY));
      $this->add(new atkAttribute("description", AF_SEARCHABLE));
      $this->add(new atkNumberAttribute("km", AF_SEARCHABLE|AF_OBLIGATORY));
      
      $this->add(new atkManyToOneRelation("car_id","carlease.carlease", AF_HIDE));
      
      $this->setOrder("km");
      
      //$this->setIndex("km");
      
      $this->setTable("carlease_maintenance");
    }
  }

?>