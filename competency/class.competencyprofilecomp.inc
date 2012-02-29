<?php
useattrib("atktextattribute");
useattrib("atknumberattribute");
/**
 * This class is used by the installer to add a table to the database
 *
 */
class competencyprofilecomp extends atkNode
{

  function competencyprofilecomp()
  {
    $this->atkNode("competencyprofilecomp");
    $this->add(new atkManyToOneRelation("profile_id","competency.competencyprofile",AF_RELATION_AUTOLINK|AF_PRIMARY|AF_OBLIGATORY|AF_SEARCHABLE));
    $this->add(new atkManyToOneRelation("competency_id","competency.competency",AF_RELATION_AUTOLINK|AF_RELATION_AUTOLINK|AF_PRIMARY|AF_OBLIGATORY|AF_SEARCHABLE));
    $this->add(new atkManyToOneRelation("niveau_id","competency.competency",AF_RELATION_AUTOLINK|AF_RELATION_AUTOLINK|AF_PRIMARY|AF_OBLIGATORY|AF_SEARCHABLE));
    $this->setTable("competency_profile_comp","competency_profile_comp");
  }
}
?>