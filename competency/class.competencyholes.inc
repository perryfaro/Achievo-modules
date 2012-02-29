<?php

/**
 * This clas creates graphs that show what competences have problems
 *
 */class competencyholes extends atkNode
{
  function competencyholes()
  {
    $this->atkNode("competencyholes");
  }

  function action_stats(&$handler)
  {
    $domain = isset($_GET["domain"])?$_GET["domain"]:0;
    $profile = isset($_GET["profile"])?$_GET["profile"]:0;

    $ui = &$this->getUi();
    $page = &$this->getPage();
    $this->addStyle("style.css");
    $page->register_style(moduleDir("competency")."styles/style.css");
    //we say we want to use the datagrapattribute
    useattrib("competency.extendeddatagraphattribute");

    $page->register_script(moduleDir("competency")."javascript/competencyhole.js");
    $page->register_loadscript("modDir='". moduleDir("competency")."';");
    $page->register_loadscript("setObservers();");
    $output = $ui->render(moduleDir("competency") . "templates/holes.tpl",$padrams,"competency");
    $boxes = array($ui->renderBox(array("title"=>atkText("stats","competency")." ".atkText("competency voids","competency")." ". $this->m_username,"content"=>"<BR>".$this->_getProfileSelect()."<br>")),
    $ui->renderBox(array("title"=>atkText("stats","competency")." ".atkText("competency voids","competency")." ". $this->m_username,"content"=>"<div id='container'><BR>".$output."<br></div>")),
    );

    $actionpage = $this->renderActionPage("stats", $boxes);

    if ($domain > 0 && $profile>0)
    {
      $page->register_loadscript("changeGraph(".$profile.",".$domain.");");
    }

    $page->addContent($actionpage);
  }

  /**
   * create the select box for profiles
   *
   * @return string html string for the profile select
   */
  function _getProfileSelect()
  {
    $output ="";
    $db = &atkGetDb();
    //prepare the query for getting the data\
    $query = &$db->createQuery();
    //we add the table competences
    $query->addTable("competency_profile");
    $query->addField("name");
    $query->addField("id");

    $profiles = $db->getrows($query->buildSelect());

    $output ="<table><tr>";
    $output.="<td>".atkText("_profile","competency").":</td>";
    $output.="<td><Select id=\"profileChoser\" name=\"profiles\" class=\"profileChoser\"><option value=0>none</option>";

    foreach ($profiles as $profile)
    {
      $id = $profile["id"];
      $output.= "<option value=" .$id ;

      if ($id == $this->profile){
        $output.=" selected ";
      }
      $output.=">".$profile["name"]."</option>";

    }
    $output.= "</select>";
    $output.="</td></tr>";
    $output.="<td>".atkText("domain","competency").":</td>";
    $output.="<td><Select id=\"domainChoser\" name=\"domains\" class=\"domainChoser\"></select>";
    $output.="</td></tr>";
    $output.="</table";
    return $output;
  }

}
?>