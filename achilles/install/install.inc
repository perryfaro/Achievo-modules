<?php

  $setup->installNode("achilles.attachment");
  $setup->installNode("achilles.bug");
  $setup->installNode("achilles.bug_cc");
  $setup->installNode("achilles.bug_dependency");
  $setup->installNode("achilles.bug_block");
  $setup->installNode("achilles.bug_history");
  $setup->installNode("achilles.comment");
  $setup->installNode("achilles.component");
  $setup->installNode("achilles.os");
  $setup->installNode("achilles.resolution");
  $setup->installNode("achilles.severity");
  //$setup->installNode("achilles.stats");
  $setup->installNode("achilles.status");
  $setup->installNode("achilles.version");
  $setup->installNode("achilles.milestone");
    

  $setup->setVersion(2);

?>