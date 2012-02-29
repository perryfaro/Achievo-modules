<?php

  userelation("atkmanytoonerelation");     
  useattrib("atkcurrencyattribute");

 /**
  * The node class for handout.
  */
 class order_line extends atkNode
 {
   function order_line()
   {
     $this->atkNode("order_line");
     $this->add(new atkAttribute("id", AF_AUTOKEY));
     $this->add(new atkManyToOneRelation("order_id", "purchase.order", AF_HIDE));
     
     $this->add(new atkAttribute("description", AF_OBLIGATORY, AF_SEARCHABLE));     
     
     $rel = &$this->add(new atkManyToOneRelation("phase_id", "project.phase"));
     
     // todo goeie filter
     //$rel->setDestinationFilter("projectid='[order_id.project_id.id]'");
     
     $this->setSecurityAlias("purchase.order");
     
     $this->add(new atkCurrencyAttribute("brutoprice", AF_TOTAL));
     $this->add(new atkCurrencyAttribute("nettoprice", AF_TOTAL));
     $this->add(new atkCurrencyAttribute("estimatedprice", AF_TOTAL));
                                 
     $this->setOrder("id");
     $this->setTable("purchase_order_line");
   }
      
   /**
    * Return record descriptor (for use in relations etc.)
    * @return String A template containing fieldnames between brackets.
    */
   function descriptor_def()
   {
     // @todo: replace this with proper descriptor fields.
     return "[description]";
   }
 }

?>