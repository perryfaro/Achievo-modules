<?php
useattrib("atktextattribute");
useattrib("atknumberattribute");
userelation("atkmanytoonerelation");
useattrib("atklistattribute");

/**
 * This class is used by the installer to add a table to the database
 *
 */
class competencyprofileperson extends atkNode
{

  function competencyprofileperson()
  {
    $this->atkNode("competencyprofileperson");

    //$this->competencyprofile("competencyprofilecomp",NF_EDITAFTER|NF_IMPORT);
    $prof = &new atkManyToOneRelation("profile_id","competency.competencyprofile",AF_RELATION_AUTOLINK|AF_PRIMARY|AF_OBLIGATORY|AF_SEARCHABLE);

    $this->add(new atkManyToOneRelation("person_id","employee.employee",AF_RELATION_AUTOLINK|AF_RELATION_AUTOLINK|AF_PRIMARY|AF_OBLIGATORY|AF_SEARCHABLE));

    $this->add($prof);
    $this->setTable("competency_profile_person");
  }
}
?>