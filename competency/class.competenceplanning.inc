<?php
useattrib("atktextattribute");
useattrib("atknumberattribute");
useattrib("atkdateattribute");

/**
 * Here we will have a overview of all schedules for the employees and it will be
 * possible to remove schedules for employees
 * 
 */
class competenceplanning extends atkNode
{
  /**
   * Constructor
   */
  public function competenceplanning()
  {
    $this->atkNode("competenceplanning");
    $this->add(new atkAttribute('id', AF_AUTOKEY));
    $this->add(new atkManyToOneRelation("competency_id", "competency.competency", AF_SEARCHABLE));
    $this->add(new atkManyToOneRelation("niveau_id", "competency.competencyniveau", AF_SEARCHABLE));
    $this->add(new atkManyToOneRelation("person_id", "employee.employee", AF_SEARCHABLE | AF_MANYTOONE_AUTOCOMPLETE));
    $this->add(new atkDateAttribute("planDate"));
    $this->add(new atkTextAttribute("remark", AF_HIDE_LIST));

    $this->setTable("competency_planning");
    $this->addFilter("[table].competency_id NOT IN 
                        (
                          SELECT competency_competences.competency_id
                          FROM competency_competences
                          WHERE niveau_id = [table].niveau_id
                            AND person_id = [table].person_id
                        )");

    // Manually call setAttribSizes(), so that after this we can make the searchbox larger.
    $this->setAttribSizes();
    $this->getAttribute('person_id')->setAttribSize(array(10, 10, 25));
  }

  /**
   * Retuns the descriptor for this node
   * 
   * @return string
   */
  public function descriptor_def()
  {
    return "[date]";
  }

}
?>
