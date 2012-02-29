<?php
/**
 * Todo
 * - Create folder
 * - Create file
 * - Download / Delete link
 * - Header met locatie (up knop eventueel)s
 * - Footer met:
 * Files: 2   Used Space: 102.8KB   
 * Total Files: 17
 */


class filemanager extends atkNode 
{

  function filemanager()
  {
    $this->atkNode("contact", NF_ADD_LINK);    
    $this->add(new atkAttribute("id" ,AF_AUTOKEY)); 
    $this->add(new atkAttribute("parent_id"));
    $this->add(new atkAttribute("filename"));
    $this->add(new atkAttribute("mimetype"));
    $this->add(new atkAttribute("size"));
    $this->add(new atkAttribute("created"));
    $this->add(new atkAttribute("modified"));
    $this->add(new atkAttribute("created_by"));
    $this->add(new atkAttribute("modified_by"));
    
    $this->add(new atkTextAttribute("comment"));
    $this->setTable("filemanager");
  }
  
  function display_mimetype()
  {
     
  }
  
  function display_filename()
  {
    // if dir, show dir icon 
  }
  
}
?>