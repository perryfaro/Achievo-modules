<?php
useattrib("atktextattribute");
useattrib("atknumberattribute");
useattrib("atkdateattribute");

/**
 * This class is used by the installer to add a table to the database
 *
 */
class competencyextra extends atkNode
{

  function competencyextra()
  {
    $this->atkNode("competencyextra");
    $this->add(new atkManyToOneRelation("competency_id", "competency.competency", AF_OBLIGATORY | AF_PRIMARY));
    $this->add(new atkManyToOneRelation("profile_id", "competency.competencyprofile", AF_OBLIGATORY | AF_PRIMARY));
    $this->setTable("competency_extra_competence");
  }
}
?>