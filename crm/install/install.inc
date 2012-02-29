<?php

  $setup->installNode("crm.activity");  
  $setup->installNode("crm.activity_status");  
  $setup->installNode("crm.activity_type");  
  
  $setup->installNode("crm.campaign");
  $setup->installNode("crm.campaign_status");
  $setup->installNode("crm.campaign_type");

  $setup->installNode("crm.account");  
  $setup->installNode("crm.account_crm_status");  
  $setup->installNode("crm.account_source");  
  $setup->installNode("crm.account_industry");  
  $setup->installNode("crm.account_relation");  
  $setup->installNode("crm.contact");  
  $setup->installNode("crm.former_name");
  $setup->installNode("crm.lead");  
  $setup->installNode("crm.lead_source"); 
  $setup->installNode("crm.lead_status"); 
  $setup->installNode("crm.opportunity");  
  $setup->installNode("crm.opportunity_status");   
  $setup->installNode("crm.opportunity_activity");   
  $setup->installNode("crm.opportunity_activity_status");   
    
  $setup->installNode("crm.rate");  
  $setup->installNode("crm.relation_type");  
  $setup->installNode("crm.salutation");  
  

  $setup->setVersion(1);

?>