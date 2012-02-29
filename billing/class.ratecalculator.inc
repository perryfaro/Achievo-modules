<?php

  atkimport("atk.utils.atktriggerlistener");

  class rateCalculator extends atkTriggerListener
  {
    function notify(&$trigger, $record, $mode='')
    {
      if (array_key_exists('id',$record) && is_numeric($record['id']) && $record['id'] > 0)
        $this->updateRate($record["id"]);
    }

    function updateRate($hours_id)
    {
      $db = &atkGetDb();

      $rn = &getNode("billing.billing_rate");
      $rate = $rn->getRateByTimeEntry($hours_id);

      $db->query("UPDATE hours SET billing_rate=".$rate." WHERE id=".$hours_id);
      $db->commit();
    }
  }

?>