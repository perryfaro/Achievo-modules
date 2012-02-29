<?php

//php5-only version

// in php5 atk classes are autoloaded, but not for achievo :-)

  useattrib('atktextattribute');
  //useattrib('atknumberattribute');
  useattrib('atkdurationattribute');
  userelation('atkmanytoonerelation');

  
  class workshop extends atkNode
  {
    public function __construct() {
      parent::__construct('workshop');
      //    onderwerp, omschrijving, lengte en een (optionele) standaard instructeur
//    subject, description, duration?, default leader

      $this->add(new atkAttribute('id', AF_AUTOKEY));
      
      /* to do: enforce that subject and length make a unique combination for every record
      Subject on its own does not need to be unique.
      This implies the following.
      We could have both refactoring/ 1 hour/ this is our quick course; and refactoring/ 7 hours/ this is our comprehensive course.
      But not refactoring/ 7 hour/ a lengthy but fabulous presentation and refactoring/7 hour/ do a real project under our supervision.
      */
      
      $this->add(new atkAttribute('subject', AF_OBLIGATORY|AF_SEARCHABLE));
      // multi line edit box for description
      $this->add(new atkTextAttribute('description', AF_OBLIGATORY|AF_SEARCHABLE));
      $this->add(new atkDurationAttribute('duration', '15m', '10h', AF_OBLIGATORY));
      // to do: find out wether a lazy many2one is a good choice here
      $this->add(new atkManyToOneRelation('default_leader','employee.employee'));
      $this->setOrder('subject');
      $this->setTable('workshop');
    }
    
    public function descriptor_def() {
      return '[subject]';
    }
  }