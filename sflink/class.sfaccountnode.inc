<?php

atkimport("atk.atkdatanode");
useattrib("atknumberattribute");
useattrib("atkboolattribute");
useattrib("atkdatetimeattribute");

class sfaccountnode extends atkDataNode
{
  private $m_client;
  
  public function __construct()
  {
    parent::__construct($type="sfaccountnode", $flags=0);

    $this->m_client = &atkGetNode("sflink.sflink")->m_client;

    $this->add(new atkAttribute('id'));
    $this->add(new atkAttribute('salesforce_id'));
    $this->add(new atkBoolAttribute('IsDeleted'));
    $this->add(new atkAttribute('MasterRecordId'));
    $this->add(new atkAttribute('name'));
    $this->add(new atkAttribute('lastmodifieddate'));
    $this->add(new atkAttribute('systemmodstamp'));
    $this->add(new atkAttribute('invoice_address'));
    $this->add(new atkAttribute('invoice_zipcode'));
    $this->add(new atkAttribute('invoice_city'));
    $this->add(new atkAttribute('invoice_state'));
    $this->add(new atkAttribute('invoice_country'));
    $this->add(new atkAttribute('phone'));
    $this->add(new atkAttribute('fax'));
    $this->add(new atkAttribute('remark'));
    $this->add(new atkAttribute('website'));
    $this->add(new atkAttribute('type'));
    $this->add(new atkAttribute('visit_address'));
    $this->add(new atkAttribute('visit_zipcode'));
    $this->add(new atkAttribute('visit_city'));
    $this->add(new atkAttribute('visit_state'));
    $this->add(new atkAttribute('visit_country'));
    $this->add(new atkAttribute('salesforce_id'));
    $this->add(new atkNumberAttribute('revenue'));
    $this->add(new atkNumberAttribute('employees'));
  }

  protected function findData($params=array())
  {
    $selector = @$params['selector'] ? $params['selector'] : '';

    if($selector['what'] == 'deleted_records')
    {
      return $this->findDeleted($selector);
    }
    elseif($selector['what'] == 'merge_records') 
    {
      return $this->findMerged($selector);
    }
    else 
    {
      return $this->findUpdated($selector);
    }
  }

  protected function findMerged($selector)
  { 
    $sfnode = &atkGetNode("sflink.sflink");

    $from =  date("c",$sfnode->asUTC($selector['from']));
    $to =  date("c",$sfnode->asUTC($selector['to']));
    
    $query = "SELECT Id,Name, Achievo_ID__c,BillingStreet,BillingPostalCode,BillingCity,BillingState,BillingCountry,Phone,
        Fax,Description,Website,Type,ShippingStreet,ShippingPostalCode,ShippingCity,ShippingState,
        ShippingCountry,OwnerId,LastModifiedDate,SystemModstamp,IsDeleted,MasterRecordId 
        FROM Account WHERE isDeleted=true AND SystemModstamp >= $from AND SystemModstamp <= $to";
        
    $sfnode->_iniQueryAll($query);
    $rez = $sfnode->_getQueryMore();
    while($rez)
    {
      $r = $this->prepareRecord($rez);

      $sfnode->strConvertSfToAtk($r);
      
      $result[] = $r;	
      $rez = $sfnode->_getQueryMore();
    }
    $this->setData($result);
    
    $sfnode->handleDelete($sfnode->asUTC($selector['from']),$sfnode->asUTC($selector['to']),"organization.organization","Account");

    return parent::findData($params);
  }  
  
  protected function findDeleted($selector)
  { 
    $sfnode = &atkGetNode("sflink.sflink");

    $from =  date("c",$sfnode->asUTC($selector['from']));
    $to =  date("c",$sfnode->asUTC($selector['to']));
    
    $query = "SELECT Id,Name, Achievo_ID__c,BillingStreet,BillingPostalCode,BillingCity,BillingState,BillingCountry,Phone,
        Fax,Description,Website,Type,ShippingStreet,ShippingPostalCode,ShippingCity,ShippingState,
        ShippingCountry,OwnerId,LastModifiedDate,SystemModstamp,IsDeleted,MasterRecordId 
        FROM Account WHERE isDeleted=true AND SystemModstamp >= $from AND SystemModstamp <= $to";
        
    $sfnode->_iniQueryAll($query);
    $rez = $sfnode->_getQueryMore();
    while($rez)
    {
      $r = $this->prepareRecord($rez);

      $sfnode->strConvertSfToAtk($r);
      
      $result[] = $r;	
      $rez = $sfnode->_getQueryMore();
    }
    $this->setData($result);
    
    return parent::findData($params);
  }
    
  protected function findUpdated($selector)
  {
    $sfnode = &atkGetNode("sflink.sflink");
    
    $from = $selector['from'];
    $to = $selector['to'];

    $getUpdateddResponse = $this->m_client->getUpdated(
      'Account', 
      $sfnode->asUTC($from),
      atkDateTimeAttribute::arrayToDateTime($to));

    $ids = array();
    if(isset($getUpdateddResponse->ids))
    {
      if(is_array($getUpdateddResponse->ids))
      {
        $ids = $getUpdateddResponse->ids;
      }
      else
      {
        $ids = array($getUpdateddResponse->ids);
      }
    }

    $chunks = array_chunk($ids,2000);
    $result = array();
    
    foreach ($chunks as $c)
    {
      $list = $this->m_client->retrieve(
        "Name, Achievo_ID__c,BillingStreet,BillingPostalCode,BillingCity,BillingState,BillingCountry,Phone,
        Fax,Description,Website,Type,ShippingStreet,ShippingPostalCode,ShippingCity,ShippingState,
        ShippingCountry,OwnerId,LastModifiedDate,SystemModstamp,IsDeleted,MasterRecordId","Account",$c);
      
      foreach ($list as $rez)
      {
        $r = $this->prepareRecord($rez);
        $sfnode->strConvertSfToAtk($r);
        $result[] = $r;	
      }
    }
    $this->setData($result);
    
    return parent::findData($params);
  }
  
  protected function prepareRecord($rez)
  {
    $r = array();
    
    $r['id'] = $rez->fields->Achievo_ID__c;
    $r['name'] = $rez->fields->Name;

    $r['invoice_address'] = $rez->fields->BillingStreet;
    $r['invoice_zipcode'] = $rez->fields->BillingPostalCode;
    $r['invoice_city'] = $rez->fields->BillingCity;
    $r['invoice_state'] = $rez->fields->BillingState;
    $r['invoice_country'] = $rez->fields->BillingCountry;
    $r['phone'] = $rez->fields->Phone;
    $r['fax'] = $rez->fields->Fax;
    $r['remark'] = $rez->fields->Description;
    $r['website'] = $rez->fields->Website;
    $r['type'] = $rez->fields->Type;
    $r['visit_address'] = $rez->fields->ShippingStreet;
    $r['visit_zipcode'] = $rez->fields->ShippingPostalCode;
    $r['visit_city'] = $rez->fields->ShippingCity;
    $r['visit_state'] = $rez->fields->ShippingState;
    $r['visit_country'] = $rez->fields->ShippingCountry;
    $r['salesforce_id'] = $rez->Id;
    $r['revenue'] = $rez->fields->AnnualRevenue;
    $r['employees'] = $rez->fields->NumberOfEmployees;
    $r['lastmodifieddate'] = $rez->fields->LastModifiedDate;
    $r['systemmodstamp'] = $rez->fields->SystemModstamp;
    $r['IsDeleted'] = ($rez->fields->IsDeleted == 'true') ? 1 : 0; 
    $r['MasterRecordId'] = $rez->fields->MasterRecordId;
    $r['salesforce_id'] = $rez->Id;
    
    return $r;
  }
}