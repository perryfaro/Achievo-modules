<?php

  userelation("atkmanytoonerelation");
  useattrib("atknumberattribute");
  useattrib("atkradioattribute");

  class rule extends atkNode
  {
  	function rule()
  	{
  		$this->atkNode("rule", NF_NO_DELETE|NF_ADD_LINK);
  	 	$this->add(new atkAttribute("id", AF_AUTOKEY));
  	 	$this->add(new atkManyToOneRelation("typeid", "holidayreport.leave_type", AF_OBLIGATORY|AF_READONLY_EDIT|AF_RELATION_AUTOLINK|AF_SEARCHABLE));
  	 	$this->add(new atkManyToOneRelation("employer_id","organization.organization",AF_OBLIGATORY|AF_LARGE));
  	 	$this->add(new atkNumberAttribute("age_min"));
  	 	$this->add(new atkNumberAttribute("age_max"));
  	 	$typeattr = &$this->add(new atkRadioAttribute("type", array("holidayreport_rule_year", "holidayreport_rule_beforemax", "holidayreport_rule_aftermax"),array("year", "month", "monthafter"), AF_OBLIGATORY|AF_DISPLAY_VERT|AF_HIDE_LIST));
  	 	$typeattr->addOnChangeHandler("valueUpdateResolution(newvalue);");
  	 	$this->add(new atkNumberAttribute("value", AF_OBLIGATORY,10,2));
  	 	$this->add(new atkNumberAttribute("weekhours", AF_OBLIGATORY,6,2));
  	 	$this->add(new atkAttribute("beginyear", AF_OBLIGATORY, 4));
  	 	$this->add(new atkAttribute("endyear", AF_HIDE_LIST, 4));

  	 	$this->setTable("holidayreport_rule");
  	 	$this->setOrder("holidayreport_rule.age_min");
    }

    function initial_values()
    {
      return array(
        "type" => "year",
      );
    }

    function value_edit($rec, $fieldprefix, $mode)
    {
      $res = $this->m_attribList["value"]->edit($rec, $fieldprefix, $mode);
      $res .= ' <span id="valueresolution">' . $this->text($rec["type"]=="year" ? "hoursperyear" : "hourspermonth") . '</span>';

      $page = &$this->getPage();
      $page->register_script(atkConfig("atkroot") . "atk/javascript/prototype.js");
      $page->register_scriptcode("
      function valueUpdateResolution(value)
      {
        $('valueresolution').innerHTML = (value=='year') ? '".$this->text("hoursperyear")."' : '".$this->text("hourspermonth")."';
      }
      ");
      return $res;
    }

    function age_min_display($rec, $mode)
    {
      $org = $this->m_attribList["age_min"]->display($rec, $mode);
      if ($org==0) $org = "";
      return $org;
    }

    function age_max_display($rec, $mode)
    {
      $org = $this->m_attribList["age_max"]->display($rec, $mode);
      if ($org==0) $org = "";
      return $org;
    }

    function descriptor_def()
    {
      return "[typeid.description], [value]";
    }

  }

?>
