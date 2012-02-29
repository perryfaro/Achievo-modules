<?php
useattrib("atkattribute");
useattrib("atktextattribute");

class address extends atkNode
{
  function address()
  {
    $this->atkNode("address");
		
    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("title", AF_SEARCHABLE|AF_OBLIGATORY));
    $this->add(new atkAttribute("address", AF_SEARCHABLE|AF_HIDE_LIST, 100));
    $this->add(new atkAttribute("address2", AF_HIDE_LIST, 100));
    $this->add(new atkAttribute("zipcode", AF_HIDE_LIST, 20));
    $this->add(new atkAttribute("city", AF_HIDE_LIST, 100));
    $this->add(new atkAttribute("state", AF_HIDE_LIST, 100));
    $this->add(new atkAttribute("country", AF_HIDE_LIST, 100));

    $this->setTable("location_address");
  }

  function descriptor_def()
  {
    return "[title]";
/* If upgrading the first address line could be copied into title */
  }
}
?>
