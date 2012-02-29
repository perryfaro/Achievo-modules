<?php

$modifiers[] = "employee.functionlevel";
$modifiers[] = "employee.employee";
$modifiers[] = "project.projectselector";


atkimport("atk.atklanguage");
$al = &atkLanguage::getInstance();
$al->addOverrideModule("advancedsecurity");

class mod_advancedsecurity extends atkModule
{
  function getNodes()
  {
    registerNode("reports.weekreport",array("report","view_all","view_managed","view_level"));
    registerNode("reports.hoursurvey",array("report","view_all","view_managed","view_level"));
  }

  function functionlevel_modifier(&$node)
  {
    atkdebug("Functionlevel modifier loaded!");

    useattrib("atknumberattribute");

    $node->remove('name');
    $node->add(new atkNumberAttribute("name",AF_OBLIGATORY|AF_SEARCHABLE),null,90);

    $attrib = &$node->getAttribute('description');
    $attrib->removeFlag(AF_HIDE_LIST);
    $attrib->addFlag(AF_FORCE_LOAD);

    $node->setOrder('name DESC');
  }

  function employee_modifier(&$node)
  {
    $attrib = &$node->getAttribute('functionlevel');
    $attrib->addFlag(AF_OBLIGATORY);
    $attrib->removeFlag(AF_HIDE_ADD);
  }

  function projectselector_modifier(&$node)
  {
    useattrib("advancedsecurity.advanced_projectfilter");
    $node->add(new advanced_projectFilter());
  }

  function phase_modifier(&$node)
  {
    useattrib("advancedsecurity.advanced_phasefilter");
    $node->add(new advanced_phaseFilter());
  }

  function getFuncLevelDropDown($params)
  {
    $selected_level_id = $params['selected_level_id'];
    $lowerlevels = $params['lower_levels'];

    require_once(moduleDir('advancedsecurity').'utils/class.functionlevelsutils.inc');

    $flutils = &atknew('advancedsecurity.utils.functionlevelutils');

    $functionlevels = $flutils->getFunctionlevels();

    $dropdown = $flutils->getFunctionLevelsDropdown($functionlevels, $selected_level_id);

    if (is_null($dropdown))
    {
      $dropdown = atktext('no_levels', 'advancedsecurity');

      return $dropdown;
    }

    return $dropdown.' '.$flutils->getCheckbox('lowerlevels', $lowerlevels);
  }

  function getEmpDropDown()
  {
    require_once(moduleDir('advancedsecurity').'utils/class.functionlevelsutils.inc');

    $flutils = &atknew('advancedsecurity.utils.functionlevelutils');

    $employees = $flutils->getEmployees();

    $dropdown = $flutils->getEmployeesDropDown($employees);

    if (is_null($dropdown))
      return '';

    return $dropdown;
  }

  function getEmpArray($empArray = '')
  {
    require_once(moduleDir('advancedsecurity').'utils/class.functionlevelsutils.inc');

    $flutils = &atknew('advancedsecurity.utils.functionlevelutils');

    $employees = $flutils->getEmployees();

    if (is_array($employees) && count($employees) > 0)
      return $employees;

    return null;
  }
}

?>