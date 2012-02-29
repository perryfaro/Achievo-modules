<?php
  
  class mod_crm extends achievoModule 
  {
    function getMenuItems()
    {
      $this->menuitem("crm", dispatch_url("crm.contact", "admin"), "main",true,100);

      $this->menuitem("activity",dispatch_url("crm.activity", "admin"), "crm",true,100);
      $this->menuitem("campaign",dispatch_url("crm.campaign", "admin"), "crm",true,100);
      $this->menuitem("account",dispatch_url("crm.account", "admin"), "crm",true,100);
      $this->menuitem("contact",dispatch_url("crm.contact", "admin"), "crm",true,100);
      $this->menuitem("lead",dispatch_url("crm.lead", "admin"), "crm",true,100);
      $this->menuitem("opportunity",dispatch_url("crm.opportunity", "admin"), "crm",true,100);
      $this->menuitem("-","", "crm",true,150);
      $this->menuitem("-","", "crm",true,150);
      $this->menuitem("activity_status",dispatch_url("crm.activity_status", "admin"), "crm",true,200);
      $this->menuitem("activity_type",dispatch_url("crm.activity_type", "admin"), "crm",true,200);
      $this->menuitem("campaign_status",dispatch_url("crm.campaign_status", "admin"), "crm",true,200);
      $this->menuitem("campaign_type",dispatch_url("crm.campaign_type", "admin"), "crm",true,200);
      $this->menuitem("account_crm_status",dispatch_url("crm.account_crm_status", "admin"), "crm",true,200);
      $this->menuitem("account_industry",dispatch_url("crm.account_industry", "admin"), "crm",true,200);
      $this->menuitem("account_source",dispatch_url("crm.account_source", "admin"), "crm",true,200);
      $this->menuitem("lead_source",dispatch_url("crm.lead_source", "admin"), "crm",true,200);
      $this->menuitem("lead_status",dispatch_url("crm.lead_status", "admin"), "crm",true,200);
      $this->menuitem("opportunity_status",dispatch_url("crm.opportunity_status", "admin"), "crm",true,200);
      $this->menuitem("opportunity_activity_status",dispatch_url("crm.opportunity_activity_status", "admin"), "crm",true,200);
      $this->menuitem("rate",dispatch_url("crm.rate", "admin"), "crm",true,200);
      $this->menuitem("relation_type",dispatch_url("crm.relation_type", "admin"), "crm",true,200);
      $this->menuitem("salutation",dispatch_url("crm.salutation", "admin"), "crm",true,200);
      $this->menuitem("-","", "crm",true,250);
      $this->menuitem("-","", "crm",true,250);
      $this->menuitem("account_stats", dispatch_url("crm.account_stats", "stats"), "crm",true,300);

      
    }
    
    function getNodes()
    {
      registerNode("crm.account", array("admin","add","delete","edit","stats"));
      registerNode("crm.contact", array("admin","add","delete","edit"));
      registerNode("crm.activities", array("admin","add","delete","edit"));
      registerNode("crm.campaigns", array("admin","add","delete","edit"));
      registerNode("crm.opportunity", array("admin","add","delete","edit"));
      registerNode("crm.files", array("admin","add","delete","edit"));
      registerNode("crm.reports", array("admin","add","delete","edit"));
      registerNode("crm.lead", array("admin","add","delete","edit","convert"));
      
    }
    
    function getDescription()
    {
      return "CRM Module.";
    }
    
    function getPimItems()
    {
    }  
    
  }
?>