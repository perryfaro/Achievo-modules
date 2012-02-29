<?php

  $modifiers[] = "employee.employee";
  $modifiers[] = "organization.organization";
  $modifiers[] = "organization.contact";

  class mod_sflink extends achievoModule
  {
    function getNodes()
    {
      registerNode("sflink.sflink", array("accountexport","contactexport","getupdated"));
      registerNode("sflink.accountnode", array("admin","add","edit","delete"));
      registerNode("sflink.sf_dupe", array("admin"));
      registerNode("sflink.sflinkcheckupdate", array("report"));
    }

    function getMenuItems()
    {
      $this->menuitem("sflink","","main",array("sflink.sflink", "accountexport"),750);

      $this->menuitem("employeelink",dispatch_url("sflink.sflink", "employeelink"),"sflink",array("sflink.sflink", "contactexport"),755);
      $this->menuitem("accountexport",dispatch_url("sflink.sflink", "accountexport"),"sflink",array("sflink.sflink", "accountexport"),760);
      $this->menuitem("clearaccount",dispatch_url("sflink.sflink", "clearsfid"),"sflink",array("sflink.sflink", "accountexport"),770);
      $this->menuitem("contactexport",dispatch_url("sflink.sflink", "contactexport"),"sflink",array("sflink.sflink", "contactexport"),780);
      $this->menuitem("clearcontact",dispatch_url("sflink.sflink", "clearcontactsfid"),"sflink",array("sflink.sflink", "accountexport"),790);
      $this->menuitem("accountimport",dispatch_url("sflink.sflink", "Accountimport"),"sflink",array("sflink.sflink", "accountexport"),800);
      $this->menuitem("contactimport",dispatch_url("sflink.sflink", "Contactimport"),"sflink",array("sflink.sflink", "contactexport"),810);
      //$this->menuitem("getUpdated",dispatch_url("sflink.sflink", "getupdated"),"sflink",array("sflink.sflink", "getupdated"),820);
      $this->menuitem("getAccount",dispatch_url("sflink.sflink", "getupdatedaccount"),"sflink",array("sflink.sflink", "getupdated"),830);
      $this->menuitem("getContact",dispatch_url("sflink.sflink", "getupdatedcontact"),"sflink",array("sflink.sflink", "getupdated"),840);
      $this->menuitem("checkUpdated",dispatch_url("sflink.sflinkcheckupdate", "report"),"sflink",array("sflink.sflinkcheckupdate", "report"),850);
      $this->menuitem("sf_dupe",dispatch_url("sflink.sf_dupe", "admin"),"sflink",array("sflink.sf_dupe", "admin"),860);
      $this->menuitem("check",dispatch_url("sflink.sflink", "check"),"sflink",array("sflink.sflink", "contactexport"),870);
      $this->menuitem("fix",dispatch_url("sflink.sflink", "fix"),"sflink",array("sflink.sflink", "contactexport"),880);
      $this->menuitem("fixcontact",dispatch_url("sflink.sflink", "fixcontact"),"sflink",array("sflink.sflink", "contactexport"),890);
      
      $this->menuitem("account_correct",dispatch_url("sflink.sflink", "correctaccount"),"sflink",array("sflink.sflink", "getupdated"),880);
      $this->menuitem("contact_correct",dispatch_url("sflink.sflink", "correctcontact"),"sflink",array("sflink.sflink", "getupdated"),880);
    }

    function organization_modifier(&$node)
    {
      $node->addFlag(NF_READONLY);
      $node->add(new atkAttribute("salesforce_id",AF_READONLY|AF_HIDE_LIST|AF_HIDE_SEARCH));
    }

    function contact_modifier(&$node)
    {
      $node->addFlag(NF_READONLY);
      $node->add(new atkAttribute("salesforce_id",AF_READONLY|AF_HIDE_LIST|AF_HIDE_SEARCH));
    }
    
    function employee_modifier(&$node)
    {
      $node->add(new atkAttribute("salesforce_id",AF_READONLY|AF_HIDE_LIST|AF_HIDE_SEARCH));
    }
    
    /**
     * This function check, do these two records can be megred
     *
     * @param array $params - parameter list:
     *  $sflink - sfnode instanse
     *  $node - organization/contact instance
     *  $id - id of slave record
     *  $masterId - id of master record
     *  $master - master record
     *  $slave - slave record
     * @return integer - SF_NOCONFLICT - no conflict, SF_CONFLICT - conflict take place
     */
    public function checkMerge($params)
    {
      $ret = SF_NOCONFLICT;
      list($sflink,$node,$id,$masterId,$master,$slave) = $params; 
      
      $commonField = $sflink->getNotMergeFieldList($node->getType());
      foreach ($node->m_attribList as $name=>$p_attr)
      {
        /**
         * In SF ShippingStreet = 'visit_address' . 'visit_address2',
         * so visit_address2 is common field too, we have not merge id... or not
         * The same question about other _address2 fields and regarding mail_address
         * created_by,created_on,last_modified_by,last_modified_on?
         * Clear this later, when we will stragggggle with conflict records
         */
        
        //is current field not a common field?
        if(!in_array($name,$commonField)) 
        {
          /**
           * - common fields; no intelligence, accept whatever salesforce has in the master
           * record
           * 
           * - achievo-specific fields; if master value = empty and slave value isn't, then
           * use slave value. If both have values and they are not the same, Achievo must
           * not merge the record yet and flag it as 'conflict record' in sfdupe so that
           * we can manually adjust it.
           * 
           * - relations; same as achievo-specific fields -> if slave has projects and 
           * master has no projects, move projects to master record. If both have 
           * projects -> conflict record. Same for other relations (but not for contact
           * persons, these will come from the master record in salesforce).
           * 
           * - relation have to handle by owns module
           */
          
          if (!is_a($p_attr,"atkonetomanyrelation") &&  !$p_attr->isEmpty($slave) && !$p_attr->isEmpty($master) && !$p_attr->equal($master,$slave))
          {
            $sflink->logConflict($node->getType(),$master,$slave,$p_attr);
            $ret = SF_CONFLICT;
          }
        }
      }
      return $ret;
    }
    
    public function doMerge($params)
    {
      $updateMaster = false;
      list($sflink,$node,$id,$masterId,$master,$slave) = $params; 
      
      $commonField = $sflink->getNotMergeFieldList($node->getType());
      foreach ($node->m_attribList as $name=>$p_attr)
      {
        //current field not a common field?
        
        /**
         * In SF ShippingStreet = 'visit_address' . 'visit_address2',
         * so visit_address2 is common field too, we have not merge id... or not
         * The same question about other _address2 fields and regarding mail_address
         * created_by,created_on,last_modified_by,last_modified_on?
         * Clear this later, when we will stragggggle with conflict records
         */
        
        if(!in_array($name,$commonField))
        {
          /**
           * - common fields; no intelligence, accept whatever salesforce has in the master
           * record
           * 
           * - achievo-specific fields; if master value = empty and slave value isn't, then
           * use slave value. If both have values and they are not the same, Achievo must
           * not merge the record yet and flag it as 'conflict record' in sfdupe so that
           * we can manually adjust it.
           * 
           * - relations; same as achievo-specific fields -> if slave has projects and 
           * master has no projects, move projects to master record. If both have 
           * projects -> conflict record. Same for other relations (but not for contact
           * persons, these will come from the master record in salesforce).
           * 
           * - relation have to handle by owns module
           */
          
          /**
           * We check possible conflict in previous step, so we simply update from slave to master
           * We update only master empty fields, and use only non-empty value from slave
           */
          
          if(!is_a($p_attr,"atkonetomanyrelation") && $p_attr->isEmpty($master) && !$p_attr->isEmpty($slave))
          {
            $master[$name] = $slave[$name];
            $updateMaster = true;
          }
        }
      }
      /**
       * Do we have update master records?
       */
      if($updateMaster)
      {
        atkimport("atk.attributes.atkdatetimeattribute");

        $master['last_modified_by'] = atkGetUser();
        $master['last_modified_on'] = atkDateTimeAttribute::datetimeArray();
        
        $node->updateDb($master);
      }
    }

  }