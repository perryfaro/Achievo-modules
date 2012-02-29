<?php

  userelation("atkmanytoonerelation");
  useattrib("atknumberattribute");

  class correction extends atkNode
  {
  	function correction()
  	{
  		$this->atkNode("correction");
  	 	$this->add(new atkAttribute("id", AF_AUTOKEY));
  	 	$this->add(new atkAttribute("year", AF_OBLIGATORY, 10));
  	 	$this->add(new atkAttribute("description", AF_SEARCHABLE));
  	 	$this->add(new atkNumberAttribute("correction", AF_OBLIGATORY, 10,2));
  	 	$this->add(new atkManyToOneRelation("userid", "employee.employee", AF_OBLIGATORY|AF_SEARCHABLE));
  	 	$this->add(new atkAttribute("type", AF_HIDE));

  	 	$this->setOrder("holidayreport_correction.year DESC, userid.lastname");
  	 	$this->setTable("holidayreport_correction");
    }

    function initial_values()
    {
    	return array("type"=>"correction");
    }

    function action_admin(&$handler)
    {
      $this->addFilter("holidayreport_correction.type='correction'");
      return $handler->action_admin();
    }

    function descriptor_def()
    {
      return "[year] - [userid.lastname], [userid.firstname]";
    }
  }

?>