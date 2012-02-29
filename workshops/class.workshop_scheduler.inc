<?php
atkimport('modules.scheduler.scheduler');
userelation('atkonetoonerelation');
// this class is not for < php5..

  class workshop_scheduler extends scheduler
  {
    protected $workshops;
    
    public function __construct()
    {
      parent::__construct('workshop_scheduler'); 
      $this->setSecurityAlias("workshops.workshop_scheduled");

      // to do: remove some unused attribs?

      // Set dependees
//      $aDependees = array('starttime' => 'endtime');
//
//      foreach ($aDependees as $sMaster => $sDependee) {
//       // $this->getAttribute($sDependee)->addFlag(AF_READONLY);
//        $this->getAttribute($sMaster)->addDependee($sDependee);
//      }

      // hide some values
      $aHidden = array('enddate');
      
      // to do: hide title, description, private, priority, owner_attendee, owner..

      foreach ($aHidden as $sHidden) {
        $this->getAttribute($sHidden)->addFlag(AF_HIDE);
      }      
    }
    
    // to do?: initial values

  }