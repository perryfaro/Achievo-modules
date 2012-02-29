<?php

atkimport("atk.atkmetanode");

class sf_dupe extends atkMetaNode 
{
  public $order = "id DESC"; //last record first
  
  function meta(&$policy)
  {
    /*@var $policy atkMetaPolicy*/
    $policy->setTypeAndParams("type","atk.attributes.atklistattribute",array(array("account","contact")));
    $policy->setTypeAndParams("direction","atk.attributes.atklistattribute",array(array("Achievo->SF","SF->Achievo","SF<->Achievo","Start update SF->achievo","End update SF->achievo","Achievo_Merge")));
    
    $policy->addFlags("type","type_id","name","name1","fields","message","statuscode","direction","created_at",AF_SEARCHABLE);
    $policy->removeFlags("id",AF_HIDE);
  }
}