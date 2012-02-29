<?php
useattrib("atktextattribute");
useattrib("atknumberattribute");
useattrib("atkdateattribute");
/**
 * Here competency groups are made
 *
 */
class competencygroup extends atkNode
{

  function competencygroup()
  {
    $this->atkNode("competencygroup");
    $this->add(new atkAttribute("id", AF_PRIMARY|
    AF_HIDE|
    AF_AUTO_INCREMENT));
    $this->add(new atkAttribute("name",AF_OBLIGATORY|AF_UNIQUE));
    $this->add(new atkTextAttribute("description"));
    $this->add(new atkManyToOneRelation("domain_id","competency.competencydomain",AF_OBLIGATORY|AF_SEARCHABLE));
    $this->setTable("competency_group");
  }

  function descriptor_def()
  {
    return "[name]";
  }

}
?>