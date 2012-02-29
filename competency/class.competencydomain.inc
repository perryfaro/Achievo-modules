<?php
useattrib("atktextattribute");
useattrib("atknumberattribute");
useattrib("atkdateattribute");
/**
 * Here we create the competency domains
 *
 */
class competencydomain extends atkNode
{

  function competencydomain()
  {
    $this->atkNode("competencydomain");
    $this->add(new atkAttribute("id", AF_PRIMARY|AF_HIDE|AF_AUTO_INCREMENT),null,1);
    $this->add(new atkAttribute("name",AF_OBLIGATORY|AF_UNIQUE),null,1);
    $this->add(new atkTextAttribute("description"));
    $this->setTable("competency_domain");
  }

  function preDelete($record)
  {
    atkgetNode('competency.competency')->deleteDb("domain_id=".$record['id']);
  }

  function recordActions($rec, &$actions, &$mraactions)
  {

    if(atkGetNode('competency.competences')->select(atkGetNode('competency.competences')->getTable().'.competency_id IN (SELECT id FROM competency WHERE domain_id='.$rec["id"].')')->rowCount() > 0)
    {
      unset($actions["delete"]);
    }
  }

  function descriptor_def()
  {
    return "[name]";
  }

}
?>