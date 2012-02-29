<?php
useattrib("atktextattribute");
useattrib("atknumberattribute");
useattrib("atkdateattribute");

/**
 * This class is used by the installer to add a table to the database
 *
 */
class competencycomments extends atkNode
{
  function competencycomments()
  {
    $this->atkNode("competencycomments");
    $this->add(new atkManyToOneRelation("competency_id","competency.competency",AF_OBLIGATORY|AF_PRIMARY));
    $this->add(new atkManyToOneRelation("person_id","person.person",AF_OBLIGATORY|AF_PRIMARY));
    $this->add(new atkTextAttribute("comment"));
    $this->setTable("competency_comments");
  }

}
?>