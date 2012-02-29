<?php
/**
 * This class created graphs to show where there are problems with competences
 *
 */
class competencydomainholes extends atkNode
{
  function competencydomainholes()
  {

    $this->atkNode("competencydomainholes");
  }

  function action_stats(&$handler)
  {
    $ui = &$this->getUi();
    $page = &$this->getPage();
    $this->addStyle("style.css");

    //we say we want to use the datagrapattribute
    useattrib("competency.extendedDataGraphAttribute");

    $page->register_script(moduleDir("competency")."javascript/competencydomainhole.js");
    $page->register_loadscript("modDir ='".moduleDir("competency")."'");
    $page->register_loadscript("setObservers();");
    
    $output = $ui->render(moduleDir("competency") . "templates/domainHoles.tpl",$padrams,"competency");
    
    $boxes = array($ui->renderBox(array("title"=>atkText("stats","competency")." ".atkText("competency_voids","competency")." ". $this->m_username,"content"=>"<BR>".$this->getProfileSelect()."<br>")),
    $ui->renderBox(array("title"=>atkText("stats","competency")." ".atkText("competency_voids","competency")." ". $this->m_username,"content"=>"<div id='container'><BR>".$output."<br></div>")),
    );

    $actionpage = $this->renderActionPage("stats", $boxes);
    $page->addContent($actionpage);
  }

  /**
   * gets html code for a select wich contains all profiles
   *
   * @return string HTML sting with the code for the select
   */
  function getProfileSelect()
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
    $select = atkText("_profile","competency")."<Select id=\"profileChoser\" name=\"profiles\" class=\"profileChoser\"><option value=0>none</option>";

    foreach ($profiles as $profile)
    {
      $id = $profile["id"];
      $select.= "<option value=" .$id ;

      if ($id == $this->profile){
        $select.=" selected ";
      }
      $select.=">".$profile["name"]."</option>";

    }
    $select.= "</select>";
    return $select;
  }
}

?>