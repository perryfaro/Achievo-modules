<?php

useattrib("atkNumberAttribute");
useattrib("atkColorPickerAttribute");
useattrib("atkDummyAttribute");

class severity extends atkNode 
{
  
  function severity()
  {
    $this->atkNode("severity");
    $this->add(new atkAttribute("severity_id",AF_AUTOKEY));
    $this->add(new atkAttribute("name",AF_OBLIGATORY));
    $this->add(new atkAttribute("description"));
    $this->add(new atkNumberAttribute("sort"));
    $this->add(new atkColorPickerAttribute("row_color","",AF_FORCE_LOAD));
    $this->add(new atkColorPickerAttribute("text_color","",AF_FORCE_LOAD));
    $this->add(new atkDummyAttribute("example"       , "", AF_HIDE_ADD));

    $this->setOrder("achilles_severity.sort,achilles_severity.name");
    $this->setTable("achilles_severity");  
  }
  
  function descriptor_def()
  {
    return "[name]"; 
  }
  
  function example_display($record)
  {
   $bgcolor = $record["row_color"];
   $fgcolor = $record["text_color"];

   $buff  = "<table cellpadding='0' cellspacing='0' border='0'><tr>";
   $buff .= "<td bgcolor='$bgcolor'><font face='verdana' size='-1' color='$fgcolor'>&nbsp;".text("severity_item")."&nbsp;</font></td>";
   $buff .= "</tr></table>";
   return $buff;
  }
  
  function example_edit($record)
  {
    return text("example").":".$this->example_display($record);
  }
}
?>