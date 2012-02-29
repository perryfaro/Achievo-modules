<?php
  
  include_once("achievotools.inc");
  useattrib("atknumberattribute");
  useattrib("atktextattribute");
  useattrib("atkdateattribute");
  useattrib("atkAttribute");
  useattrib("atklistattribute");
  useattrib("atkemailattribute");
  useattrib("atkupdatestampattribute");
 
  userelation("atkmanytoonerelation");
     
  class software_search extends atkNode
  {   

    function software_search()  
    {    
      global $g_sessionManager;   
         
      $this->atkNode("software_search" ,NF_NO_ADD|NF_NO_DELETE|NF_NO_EDIT);
      $this->add(new atkAttribute("id", AF_AUTOKEY));
      $this->add(new atkupdatestampattribute("last_activity",AF_READONLY|AF_HIDE_LIST));
      $this->add(new atkAttribute("purch_product_name",AF_SEARCHABLE|AF_READONLY, 75));
      $this->add(new atkManyToOneRelation("buyer","organization.contact",AF_SEARCHABLE|AF_READONLY|AF_RELATION_AUTOLINK));
      $this->add(new atkManyToOneRelation("project_id", "project.project",AF_SEARCHABLE|AF_READONLY|AF_RELATION_AUTOLINK));
      $this->add(new atknumberattribute("po_num", AF_READONLY|AF_HIDE_LIST|AF_SEARCHABLE));
        //$states = array("no","yes"); 
      $this->setTable("software_software");  
      $this->setSecurityAlias("software.software");
    }
  }  
?>