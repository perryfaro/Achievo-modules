<?php

useattrib("atkdateattribute");
useattrib("atktextattribute");
userelation("atkmanytoonerelation");

class note extends atkNode
{
  function note()
  {
    global $ATK_VARS;
    atk_var_dump($ATK_VARS," NOTE");
    $this->atkNode("note");

    $this->add(new atkAttribute("id", AF_AUTOKEY));
    //$this->add(new atkManyToOneRelation("projectid", "project.project",AF_OBLIGATORY|AF_SEARCHABLE|AF_READONLY));
    $this->add(new atkAttribute("foreignkey",AF_HIDE));
    $this->add(new atkAttribute("nodetype",AF_HIDE));
    $this->add(new atkAttribute("title", AF_OBLIGATORY|AF_SEARCHABLE, 50));    
    $this->add(new atkTextAttribute("description" , TEXT_LARGE, AF_OBLIGATORY|AF_HIDE_LIST));
    $this->add(new atkManyToOneRelation("owner", "employee.employee", AF_HIDE_ADD|AF_READONLY));
    $this->add(new atkDateAttribute("entrydate", "F d Y","d F Y", date("Ymd"), 0, AF_HIDE_EDIT|AF_HIDE_ADD|AF_OBLIGATORY));
    if($ATK_VARS["atknodetype"]=="crm.note")
    {
      $this->addFilter("crm_note.nodetype = '".$ATK_VARS["atksource"]."'");
    }
    else 
    {
      $this->addFilter("crm_note.nodetype = '".$ATK_VARS["atknodetype"]."'");
    }
    $this->setTable("crm_note");
    $this->setOrder("entrydate");

  }

  function initial_values()
  {
    global $g_user,$ATK_VARS;
    return array("nodetype"=>$ATK_VARS["atksource"],
                 "owner"=>array("id"=>$g_user["id"]),
                 "entrydate"=>array("year"=>date("Y"),
                                    "month"=>date("m"),
                                    "day"=>date("d")));
  }

  function descriptor_def()
  {
    return "[title]";
  }
}
?>
