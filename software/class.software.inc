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
     
  class software extends atkNode
  {  
    function software()  
    {    
      global $g_sessionManager;   
         
      $this->atkNode("software" ,NF_ADD_LINK);   
      $this->add(new atkAttribute("id", AF_AUTOKEY));
      //$this->setSecurityAlias("organization.organization");
      $this->add(new atkAttribute("name",AF_SEARCHABLE|AF_OBLIGATORY, 75));
      $this->add(new atkupdatestampattribute("last_activity",AF_READONLY));
      $this->add(new atkManyToOneRelation("buyer","organization.contact",AF_HIDE_LIST|AF_HIDE_SEARCH|AF_RELATION_AUTOLINK));
      $this->add(new atkManyToOneRelation("project_id", "project.project",AF_SEARCHABLE|AF_RELATION_AUTOLINK));
      $this->add(new atkEmailAttribute("renewal_notification_email_address", false,AF_HIDE_LIST, 85),"purchase_info");
      $this->add(new atkAttribute("po_number", AF_SEARCHABLE, 35), "purchase_info");
      $this->add(new atkdateattribute("po_issue_date", "F d Y", "d F Y", 0, 0, AF_HIDE),"purchase_info");      
      $this->add(new atkAttribute("licensed_version",AF_HIDE_SEARCH|AF_HIDE_LIST, 10),"purchase_info");
      $this->add(new atknumberattribute("manufacturer_part_num",AF_HIDE_SEARCH|AF_HIDE_LIST, 45),"purchase_info");
      $this->add(new atkAttribute("manufacturer",AF_HIDE_SEARCH|AF_HIDE_LIST, 35),"purchase_info");
      $this->add(new atkAttribute("cost_center",AF_HIDE_SEARCH|AF_HIDE_LIST, 35),"purchase_info");
      $this->add(new atknumberattribute("list_price",AF_HIDE_SEARCH|AF_HIDE_LIST, 10,2),"purchase_info");
      $this->add(new atkAttribute("pricing_metrics",AF_HIDE_SEARCH|AF_HIDE_LIST, 35),"purchase_info");
      $this->add(new atknumberattribute("unit_cost",AF_HIDE_SEARCH|AF_HIDE_LIST, 10,2),"purchase_info");
      $this->add(new atknumberattribute("po_quantity",AF_HIDE_SEARCH|AF_HIDE_LIST),"purchase_info");
      $this->add(new atkAttribute("vendor",AF_SEARCHABLE|AF_OBLIGATORY, 35));
      $this->add(new atklistattribute("upgrade_terms"		 ,array("no","yes"),"",AF_HIDE_LIST|AF_HIDE_SEARCH),"purchase_info");
      $this->add(new atklistattribute("taxable"		 ,array("no","yes"),"",AF_HIDE_LIST|AF_HIDE_SEARCH),"purchase_info");
      $this->add(new atklistattribute("shrink_wrap"		 ,array("no","yes"),"",AF_HIDE_LIST|AF_HIDE_SEARCH),"purchase_info");
      $this->add(new atkManyToOneRelation("manager","organization.contact",AF_RELATION_AUTOLINK|AF_HIDE_LIST|AF_HIDE_SEARCH|AF_HIDE_VIEW),"purchase_info");
      
      $this->add(new atkdateattribute("product_review_date"                  , "F d Y", "d F Y", 0, 0,AF_SEARCHABLE|AF_HIDE_LIST),"product_review"); 
      $this->add(new atkManyToOneRelation("product_reviewer","organization.contact",AF_HIDE_LIST|AF_HIDE_SEARCH|AF_RELATION_AUTOLINK),"product_review");
      $this->add(new atktextattribute("reviewer_review_notes",TEXT_LARGE|AF_HIDE_LIST|AF_HIDE_SEARCH),"product_review");
      $this->add(new atklistattribute("renew_based_on_product_review"		 ,array("no","yes"),"",AF_HIDE_LIST|AF_HIDE_SEARCH),"product_review");
      $this->add(new atklistattribute("home_use"		 ,array("no","yes"),"",AF_HIDE_LIST|AF_HIDE_SEARCH),"product_review");
      
      $this->add(new atkAttribute("license_category",AF_HIDE_LIST|AF_HIDE_SEARCH),"license_info");
      $this->add(new atkAttribute("license_type",AF_HIDE_LIST|AF_HIDE_SEARCH),"license_info");
      $this->add(new atkdateattribute("license_start_date"                  , "F d Y", "d F Y", 0, 0, AF_HIDE_LIST),"license_info"); 
      $this->add(new atkdateattribute("license_renewal_date"                  , "F d Y", "d F Y", 0, 0, AF_SEARCHABLE|AF_HIDE_LIST),"license_info"); 
      $this->add(new atktextattribute("license_notes",TEXT_LARGE|AF_HIDE_LIST|AF_HIDE_SEARCH, VARCHAR2, 2000),"license_info");
      
      $this->add(new atklistattribute("yearly_maint_included"		 ,array("no","yes"),"",AF_HIDE_LIST|AF_HIDE_SEARCH),"maint");
      $this->add(new atklistattribute("yearly_maint_required"		 ,array("no","yes"),"",AF_HIDE_LIST|AF_HIDE_SEARCH),"maint");
      $this->add(new atktextattribute("maint_notes","software",TEXT_LARGE|AF_HIDE_LIST),"maint");
      $this->add(new atkdateattribute("maint_renewal_date"                  , "F d Y", "d F Y", 0, 0,AF_SEARCHABLE|AF_HIDE_LIST),"maint"); 
      $this->add(new atktextattribute("license_terms",TEXT_LARGE|AF_HIDE_LIST|AF_HIDE_SEARCH|AF_HIDE_VIEW),"license_info");
      $this->add(new atklistattribute("first_year_tech_support_included"		 ,array("no","yes"),"",AF_HIDE_LIST|AF_HIDE_SEARCH),"support_info");
      $this->add(new atklistattribute("tech_support_active"		 ,array("no","yes"),"",AF_HIDE_LIST|AF_HIDE_SEARCH),"support_info");
      $this->add(new atklistattribute("tech_support_required"		 ,array("no","yes"),"",AF_HIDE_LIST|AF_HIDE_SEARCH),"support_info");
      $this->add(new atkdateattribute("tech_support_renewal_date"                  , "F d Y", "d F Y", 0, 0,AF_HIDE_LIST|AF_HIDE_SEARCH|AF_HIDE_VIEW),"support_info"); 
      $this->add(new atktextattribute("tech_support_notes",TEXT_LARGE|AF_HIDE_LIST|AF_HIDE_SEARCH|AF_HIDE_VIEW),"support_info");
      $this->add(new atktextattribute("tech_support_coverage_details",TEXT_LARGE|AF_HIDE_LIST|AF_HIDE_SEARCH),"support_info");
      //$states = array("no","yes"); 
      $this->setTable("software_software");  
    }
  }  
?>