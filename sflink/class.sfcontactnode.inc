<?php

atkimport("atk.atkdatanode");
useattrib("atknumberattribute");
useattrib("atkdatetimeattribute");

class sfcontactnode extends atkDataNode
{
  private $m_client;
  
  public function __construct()
  {
    parent::__construct($type="sfcontactnode", $flags=0);

    $this->m_client = &atkGetNode("sflink.sflink")->m_client;

    $this->add(new atkAttribute('id'));
    $this->add(new atkAttribute('salesforce_id'));
    $this->add(new atkBoolAttribute('IsDeleted'));
    $this->add(new atkAttribute('MasterRecordId'));
    $this->add(new atkAttribute('lastmodifieddate'));
    $this->add(new atkAttribute('systemmodstamp'));
    $this->add(new atkAttribute('function'));
    $this->add(new atkAttribute('title'));
    $this->add(new atkAttribute('lastname'));
    $this->add(new atkAttribute('firstname'));
    $this->add(new atkAttribute('phone'));
    $this->add(new atkAttribute('fax'));
    $this->add(new atkAttribute('email'));
    $this->add(new atkAttribute('address'));
    $this->add(new atkAttribute('city'));
    $this->add(new atkAttribute('state'));
    $this->add(new atkAttribute('zipcode'));
    $this->add(new atkAttribute('country'));
    $this->add(new atkAttribute('cellular'));
    $this->add(new atkAttribute('remark'));
    $this->add(new atkAttribute('salesforce_id'));
    $this->add(new atkAttribute('role'));
//    $this->add(new atkAttribute('account'));
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

  private function findMerged($selector)
  { 
    $sfnode = &atkGetNode("sflink.sflink");

    $from =  date("c",$sfnode->asUTC($selector['from']));
    $to =  date("c",$sfnode->asUTC($selector['to']));
    
    $query = "SELECT Id,Achievo_ID__c,Salutation,Title,LastName,FirstName,Phone,Fax,Email,
      MailingStreet,MailingCity,MailingState,MailingPostalCode,MailingCountry,
      MobilePhone,Description,LastModifiedDate,SystemModstamp,IsDeleted,MasterRecordId
      FROM Contact WHERE isDeleted=true AND SystemModstamp >= $from AND SystemModstamp <= $to";
        
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

    $sfnode->handleDelete($sfnode->asUTC($selector['from']),$sfnode->asUTC($selector['to']),"organization.contact","Contact");
    
    return parent::findData($params);
  }
    
  private function findDeleted($selector)
  { 
    $sfnode = &atkGetNode("sflink.sflink");

    $from =  date("c",$sfnode->asUTC($selector['from']));
    $to =  date("c",$sfnode->asUTC($selector['to']));
    
    $query = "SELECT Id,Achievo_ID__c,Salutation,Title,LastName,FirstName,Phone,Fax,Email,
      MailingStreet,MailingCity,MailingState,MailingPostalCode,MailingCountry,
      MobilePhone,Description,LastModifiedDate,SystemModstamp,IsDeleted,MasterRecordId
      FROM Contact WHERE isDeleted=true AND SystemModstamp >= $from AND SystemModstamp <= $to";
        
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

  private function findUpdated($selector)
  {
    $sfnode = &atkGetNode("sflink.sflink");

    $from = $selector['from'];
    $to = $selector['to'];

    $getUpdateddResponse = $this->m_client->getUpdated(
      'Contact', 
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
            "Achievo_ID__c,Salutation,Title,LastName,FirstName,Phone,Fax,Email,
            MailingStreet,MailingCity,MailingState,MailingPostalCode,MailingCountry,
            MobilePhone,Description,LastModifiedDate,SystemModstamp,IsDeleted,MasterRecordId",
            "Contact",$c);
      
      $result = array();
      
      foreach ($list as $rez)
      {
        $c = $this->prepareRecord($rez);
        
        //encode from UTF-8 to ATK CHARSET
        $sfnode->strConvertSfToAtk($c);
  
        $result[] = $c;	
      }
    }
    $this->setData($result);

    return parent::findData($params);
  }
  
  private function prepareRecord($rez)
  {
    $c = array();
    
    $c['id'] = $rez->fields->Achievo_ID__c;
    $c['function'] = $rez->fields->Title;
    $c['title'] = $rez->fields->Salutation;
    $c['lastname'] = $rez->fields->LastName;
    $c['firstname'] = $rez->fields->FirstName;
    $c["phone"] = $rez->fields->Phone;
    $c["fax"] = $rez->fields->Fax;
    $c["email"] = $rez->fields->Email;
    $c["address"] = $rez->fields->MailingStreet;
    $c["city"] = $rez->fields->MailingCity;
    $c["state"] = $rez->fields->MailingState;
    $c["zipcode"] = $rez->fields->MailingPostalCode;
    $c["country"] = $rez->fields->MailingCountry;
    $c["cellular"] = $rez->fields->MobilePhone;
    $c["remark"] = $rez->fields->Description;
    $c["salesforce_id"] = $rez->Id;
    $c['lastmodifieddate'] = $rez->fields->LastModifiedDate;
    $c['systemmodstamp'] = $rez->fields->SystemModstamp;
    $c['IsDeleted'] = ($rez->fields->IsDeleted == 'true') ? 1 : 0; 
    $c['MasterRecordId'] = $rez->fields->MasterRecordId;
    $c['salesforce_id'] = $rez->Id;

    //relations - don't work at all
//    $c['companyid'] = $rez->fields->AccountId; 
    
    return $c;
  }
}
