<?php

  $modifiers[] = "quotation.quotation";

  class mod_qdocument extends achievoModule
  {    
    function quotation_quotation_modifier(&$node)
    {
      userelation("atkonetomanyrelation");
      $node->add(new atkOneToManyRelation("docs", "qdocument.qdocument", "quotation_id", AF_HIDE_LIST));
    }
    
    function getDescription()
    {
      return "Document support for the quotation module (needs quotation module)";
    }

  }
  
?>