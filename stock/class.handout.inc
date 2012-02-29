<?php

  userelation("atkmanytoonerelation");
  useattrib("atkupdatedbyattribute");
  useattrib("atkupdatestampattribute");
  useattrib("project.projectphaseattrib");
  useattrib("atknumberattribute");

 /**
  * The node class for handout.
  */
 class handout extends atkNode
 {
   function handout()
   {
     $this->atkNode("handout");
     $this->add(new atkAttribute("id", AF_AUTOKEY));
     
     $this->add(new atkManyToOneRelation("product_id", "stock.product", AF_SEARCHABLE));
     $this->add(new atkNumberAttribute("amount"));
     
     // todo, ajaxy thingee, eerst project ook selecteren
     $this->add(new atkManyToOneRelation("phase_id", "project.phase"));
     $this->add(new atkUpdatedByAttribute("user_id", AF_HIDE_ADD));
     $this->add(new atkUpdateStampAttribute("entrydate", AF_HIDE_ADD));
     
     
     $this->setOrder("entrydate DESC");
     $this->setTable("stock_handout");
   }
   
   /**
    * Set up default values for this node
    * @return array Associative array where the key is
    *               the attribute name, and value is the
    *               default value.
    */
   function initial_values()
   {    
     return array("user_id"=>getUser());
   }
   
   /**
    * Return record descriptor (for use in relations etc.)
    * @return String A template containing fieldnames between brackets.
    */
   function descriptor_def()
   {
     // @todo: replace this with proper descriptor fields.
     return "[id]";
   }
 }

?>