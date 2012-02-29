<?php

/**
 * Module Project
 * Class document
 *
 * This class makes it possible to add documents to a project and upload this document to the webserver.
 *
 * $Id: class.qdocument.inc 3430 2004-01-02 19:27:32Z ivo $
 * @author L. Willems  <lineke@ibuildings.nl>
 * @version $Revision: 3430 $
 *
 */

  useattrib ("atkFileAttribute");
  useattrib ("atkDateAttribute");
  useattrib ("atklistattribute");
  useattrib ("atknumberattribute");
  userelation ("atkmanytoonerelation");

  class qdocument extends atkNode
 
  {
    function qdocument()
    {
      $this->atkNode("qdocument");

      $this->add(new atkAttribute("id", AF_AUTOKEY));
      $this->add(new atkAttribute("name", AF_OBLIGATORY, 35));
      $this->add(new atkFileAttribute("filename",atkconfig("qdocumentpath"),AF_FILE_POPUP|AF_FILE_NO_SELECT|AF_OBLIGATORY|AF_FILE_PHYSICAL_DELETE));
      $this->add(new atkAttribute("owner",AF_READONLY|AF_HIDE_ADD|AF_HIDE_EDIT));
      $this->add(new atkNumberAttribute("quotation_id", AF_HIDE));
      $this->add(new atkDateAttribute("entrydate",AF_READONLY|AF_HIDE_ADD|AF_HIDE_EDIT));
      $this->add(new atkAttribute("version", AF_OBLIGATORY, 25));
      $this->add(new atkListAttribute("status", array("concept", "internal", "accepted")));
      
      $this->setSecurityAlias("quotation.quotation");
      $this->setTable("qdocument");
      $this->setOrder("qdocument.id desc");
    }

    function initial_values()
    {
      global $g_user;
      return array("owner"=>$g_user["name"],
                 "entrydate"=>array("year"=>date("Y"),
                                    "month"=>date("m"),
                                    "day"=>date("d")));
    }

  }


?>
