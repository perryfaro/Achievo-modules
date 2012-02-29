<?php
  userelation('atkmanytoonerelation');
  userelation('atkonetoonerelation');

// this class is not for < php5..
  class workshop_scheduled extends atkNode
  {

    public function __construct() {
      

      $this->add(new atkAttribute('id', AF_AUTOKEY));
      $rel = $this->add(new atkManyToOneRelation('workshop_id', 'workshops.workshop', AF_OBLIGATORY));

      $this->add(new atkManyToOneRelation('leader','employee.employee', AF_OBLIGATORY));
      
      $rel->addDependee('leader');
      
      // integrate scheduler with a 1to1 in order to let the user manipulate values with the scheduler
      // this node (workshop_scheduled) is the slave and scheduler.scheduler is the master

      $this->add(new atkOneToOneRelation('scheduler_id', 'workshops.workshop_scheduler', '', AF_ONETOONE_INTEGRATE));
      
      
      // etcetera
      
      $this->setTable('workshop_scheduled');
      parent::__construct('workshop_scheduled', NF_ADD_LINK);
    }
    
    public function leader_edit($record, $prefix, $mode)
    {

      atkdebug("<h1>TEKNO: $mode</h1>");
      
      
      if ($mode=='add')
      {        
        $record['leader'] = $record['workshop_id']['default_leader'];
      }
      return $this->getAttribute('leader')->edit($record, $prefix, $mode);       
    }
    
    public function preAdd(&$record)
    {
      atk_var_dump($record, '<h1>RECORD START PREADD, 25 JULI</h1>');
      // set some scheduler record values "the workshop way"
      $this->setWorkshopValues(&$record['scheduler_id'], $record['workshop_id']);
      
      parent::preAdd(&$record);
      
//    atkdebug('<h1>preAdd</h1>', var_dump($record));
      return true;         
    }

    public function preUpdate(&$record)
    {
      atk_var_dump($record, '<h1>RECORD START PREUPDATE, 25 JULI</h1>');      
      // set some scheduler record values "the workshop way"      
      $this->setWorkshopValues(&$record['scheduler_id'],$record['workshop_id']);
      
      parent::preUpdate(&$record);
      return true;
    }   
    
    public function postDelete()
    {
      // To do:
      // When a record in the scheduler is deleted the record in workshop_scheduled should be deleted as well. Or does the 1-to-1 take care of this? Check!
      parent::postDelete();
      return true;
    }

    protected function getEndtime($starttime, $duration)
    {
      atk_var_dump($duration, '<h1>DURATION '.$duration.'25 juli</h1>');
      
      $endtime['seconds'] = $starttime['seconds'];
      $endtime['minutes'] = $starttime['minutes'];

      // currently we rely on whole hour durations (only 60 minute steps)
      // to do: fix this
      $nHours = $starttime['hours'] + $duration/60;
      
      // to do: what if the calculated endtime is after workhours or into the next day?
      $sHours = (string) $nHours;
      $sFormattedHours = str_pad($sHours, 2, '0', STR_PAD_LEFT);
      $endtime['hours'] = $sFormattedHours;
      
      return $endtime;
    }
    
    protected function setWorkshopValues($rec_scheduler, $rec_workshop)
    {
      // Workshops are one-day events: set the enddate to the same value as startdate, so it shows up correct in the scheduler views.

      // it seems that an an empty title cripples the dayview...
      $rec_scheduler['title'] = $rec_workshop['subject'];
      
      $rec_scheduler['description'] = '';
      $rec_scheduler['enddate'] = $rec_scheduler['startdate'];
      $rec_scheduler['endtime'] = $this->getEndtime($rec_scheduler['starttime'], $rec_workshop['duration']);
      $rec_scheduler['owner']['id'] = -1;
      $rec_scheduler['owner_attendee'] = 0;
      $rec_scheduler['recur'] = 'once';
      $rec_scheduler['private'] = 0;

      // to do: add the leader as invited, accepted?
      
      return $rec_scheduler;
    }

  }