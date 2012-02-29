<?php


// So we specify which nodes to install... Only nodes that have their own table?

  $setup->installNode('workshops.workshop');
  $setup->installNode('workshops.workshop_scheduled');
  $setup->installNode('workshops.workshop_scheduler'); // this node is a subclass that defines some extra logic... should it be in here?
  
  $setup->setVersion(1);