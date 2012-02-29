<?php

  class software extends atkNode
  { 
    function software()
    {
      $this->atkNode("software");

      $this->add(new atkAttribute("id", AF_AUTOKEY));
      $this->add(new atkAttribute("name", AF_SEARCHABLE, 40));

      $this->setTable("prjsw_software");
      $this->setOrder("name");
    }

    function descriptor_def()
    {
      return "[name]";
    }
  }
