<?php

class last_viewed_node extends atkNode
{
   function last_viewed_node($type, $flags=0)
   {
       $this->atkNode($type, $flags);
   }
   
   function dispatch($postvars, $fullpage=true)
   {
     $query = &$this->m_db->createQuery();
     if (in_array($postvars["atkaction"], array("admin", "view", "edit")))
     {
        $query->addTable("mcontact_last_viewed");
        $query->addField("atknodetype", $this->atkNodeType(), "", "", true);
        $query->addField("atkaction", $postvars["atkaction"], "", "", true);
        $query->addField("created", date("Y-m-d H:i:s"), "", "", true);

        if (!empty($postvars["atkselector"]))
        {
          $query->addField("atkselector", str_replace("'", "", $postvars["atkselector"]), "", "", true);
          $records = $this->selectDb($postvars["atkselector"]);

          foreach ($records as $record)
          {
            $query->addField("descriptor", $this->descriptor($record).(($record["contact_id"]["organization"]) ? " - ".$record["contact_id"]["organization"] : null), "", "", true);
          }
        }
        if (!$query->executeInsert())
        {
               atkdebug("executeInsert failed..");
        }
     }
     parent::dispatch($postvars, $fullpage);
   }
}
?>