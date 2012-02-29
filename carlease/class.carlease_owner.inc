<?php

  //1-N relation carlease - carlease_maintenance
  useattrib("atkdateattribute");
  useattrib("atknumberattribute");
  useattrib("atkcurrencyattribute");
  
  userelation("atkmanytoonerelation");

  class carlease_owner extends atkNode
  {
    function carlease_owner()
    {
      //create new node carlease
      $this->atkNode("carlease_owner", NF_ADD_LINK);
      //set id field as primary key
      $this->add(new atkAttribute("id",AF_AUTOKEY));
      $this->add(new atkManyToOneRelation("employee","employee.employee", AF_SEARCHABLE|AF_OBLIGATORY));
      $this->add(new atkDateAttribute("startdate"));
      $this->add(new atkDateAttribute("enddate"));
      $this->add(new atkDateAttribute("taxcontributionstartdate", AF_HIDE_LIST));
      $this->add(new atkDateAttribute("taxcontributionenddate", AF_HIDE_LIST));
      
      $this->add(new atkManyToOneRelation("car_id","carlease.carlease"));
      
      $this->setOrder("startdate");
      
      //$this->setIndex("startdate");
      
      $this->setTable("carlease_owner");
      


    }
    
    function descriptor_def()
    {
      //return "[licenseNumber]";
    }
  }

?>