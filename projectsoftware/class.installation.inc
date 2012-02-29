<?php

   userelation("atkmanytoonerelation");

  class installation extends atkNode
  {
    function installation()
    {
      $this->atkNode("installation");
      $this->add(new atkAttribute("id", AF_AUTOKEY));
      $this->add(new atkManyToOneRelation("project_id", "project.project", AF_SEARCHABLE)); 
      $this->add(new atkManyToOneRelation("software_id", "projectsoftware.software", AF_SEARCHABLE|AF_RELATION_AUTOLINK));
      $this->add(new atkManyToOneRelation("maintainer_id", "employee.employee", AF_SEARCHABLE));

      $this->setTable("prjsw_installation");
    }
  }

?>
