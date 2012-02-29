<?php

  class mod_workshops extends achievoModule
  {
  	
    public function getMenuItems() {
      $this->menuitem('workshops',dispatch_url('workshops.workshop', 'admin'),'main',array('workshops.workshop', 'admin'),400);
      $this->menuitem('workshop definitions', dispatch_url('workshops.workshop', 'admin'),'workshops');
      $this->menuitem('scheduled workshop', dispatch_url('workshops.workshop_scheduled', 'admin'),'workshops');
      
      // testing:
      // $this->menuitem('direct and simple schedule, no one-to-one', dispatch_url('workshops.workshop_scheduler', 'admin'),'workshops');
    }

    public function getNodes() {
      registerNode('workshops.workshop', array('admin', 'add', 'edit', 'delete'));
      registerNode('workshops.workshop_scheduled', array('admin', 'add', 'edit', 'delete'));
      
      // workshop_scheduler has a security alias in the node, therefor commented out the following line
      // registerNode('workshops.workshop_scheduler', array('admin', 'add', 'edit', 'delete'));
    }

  }