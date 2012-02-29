<?php
atkimport('atk.front.atkfrontcontroller');

class holescontroller extends atkFrontcontroller
{

  /**
   * this class handles all the ajax requests for the competency holes page
   *
   **/
  function competencyHoleAjax($profileId, $type, $domainId)
  {
    switch ($type)
    {
      case "graph":
        $this->showGraph($profileId, $domainId);
        break;
      case "domainBox":
        $this->fillDomain($profileId);
        break;
    }
  }

  function showAction()
  {
     $this->mythemedir = moduleDir('competency') . 'themes/default/';
  }
  /**
   * here we create the graph and show it in a frame we use a frame here because we want to be able to use CSIM images what wont work
   * if we use a image tag.
   *
   * @param int $profileId id of the profile we want to create the graph for
   * @param int $domain id of the domain we want to create the graph for
   * @return void;
   */
  function showGraphAction()
  {
    $this->showGraph($_GET['profileId'], $_GET['domainId']);
  }

    /**
     * Takes care of collecting all the data for the graph
     * @param  int $profileId what profile we want to render the graph for
     * @param  int $domain id of teh domain we want to show the graph for
     * @return bool
     */
  function showGraph($profileId, $domain)
  {
    //first we check if the profile is higher then 0 if this isnt the case we know
    //the user selected the option none so we dont have to do all the big work we just print a string and stop the rest
    if ($profileId == 0)
    {
      print("Select a profile to see graph");
      return true;
    }
    $params = array();
    $params["profile"] = $profileId;
    $params["domain"] = $domain;
    $params["reach"] = 10;
    useattrib("competency.extendeddatagraphattribute");
    $graph = &new extendedDataGraphAttribute("Competency_Void", "competencyHoles", $params, "radar");
    $graph->setSource("competency.competency");
    $graphwidth = atkconfig("graph_width", 550);
    $graphheight = round($graphwidth / 650 * 250);
    $this->output .= " <IFRAME align=top width=" . ($graphwidth + 20) . " height=" . ($graphheight + 20) . " scrolling=NO frameborder=0 src=\"" . $graph->getSRc() . "\"></iframe>";
    $this->output .= " <BR><BR>" . $this->renderAsTable($profileId, $domain);
  }

  /**
   * fills the domain list with available domains
   *
   * @param int $profileId id of the profile we want to see the available domains for
   */
  function fillBoxAction()
  {
    $profileId = $_GET["profileId"];
    $db = &atkGetDb();
    //prepare output
    $output = "";
    //prepare the query for getting the data
    $query = &$db->createQuery();
    //we add the table competences
    $query->addTable("competency_domain");
    $query->addField("id");
    $query->addField("name");
    $query->addCondition("id IN (SELECT competency.domain_id FROM competency_profile_comp , competency WHERE competency.id = competency_profile_comp.competency_id AND competency_profile_comp.profile_id =" . $profileId . ")");
    $domains = $db->getrows($query->buildSelect());
    atkimport('atk.utils.atkjson');
    $json = atkJSON::encode($domains);
    print($json);
    die();
  }

  /**
   * This functions is for the output of the table we want to see with this graph
   *
   * @param int $profile profile we want to see the data of
   * @param int  $domain domain we want to see the data of
   * @return string html string to render the table data
   */
  function renderAsTable($profile, $domain)
  {
    $db = &atkGetDb();
    // first we select all persons that have this profile
    // we need this part to know what competences and nivues we need to show in the table without this
    //we could not see competences that has no perosns yet
    $query = &$db->createQuery();
    //we add the table competences
    $query->addTable("competency_profile_comp");
    $query->addJoin("competency_niveau", "", "competency_niveau.id = competency_profile_comp.niveau_id", true);
    $query->addJoin("competency", "", "competency.id = competency_profile_comp.competency_id", true);
    $query->addCondition("competency_profile_comp.profile_id=" . $profile);
    $query->addCondition("competency.domain_id = " . $domain);
    $query->addField("competency.id as comp");
    $query->addField("competency.name as compname");
    $query->addField("level");
    $dataSet = $db->getrows($query->buildSelect());
    $container = array();
    //we will put all this data in an array so with a next query we can make calculations
    foreach ($dataSet as $item)
    {
      $container[$item["compname"]][$item["level"]] = 0;
    }
    $personsQuery = &$db->createQuery();
    $personsQuery->addTable("competency_profile_person");
    $personsQuery->addField("count(distinct(person_id)) as persons");
    $personsQuery->addCondition("profile_id=" . $profile);
    $persons = $db->getrows($personsQuery->buildSelect());
    $numberOfPersons = $persons[0]["persons"];
    $competencesQuery = &$db->createQuery();
    $competencesQuery->addTable("competency_competences");
    $competencesQuery->addJoin("competency_niveau", "", "competency_niveau.id = competency_competences.niveau_id", true);
    $competencesQuery->addJoin("competency", "", "competency.id = competency_competences.competency_id", true);
    $competencesQuery->addCondition("person_id IN (SELECT person_id from competency_profile_person WHERE profile_id =" . $profile . ")");
    $competencesQuery->addCondition("competency_competences.competency_id IN (SELECT id from competency WHERE domain_id=" . $domain . ")");
    $competencesQuery->addField("competency_competences.competency_id as comp");
    $competencesQuery->addField("competency.name as compname");
    $competencesQuery->addField("max(level) as level");
    $competencesQuery->addGroupBy("competency_competences.competency_id");
    $competencesQuery->addGroupBy("competency_competences.person_id");
    $competencesDataSet = $db->getrows($competencesQuery->buildSelect());
    //now we add what we just counted to the list
    foreach ($competencesDataSet as $item)
    {
      if ($item["level"] != "")
      {
        $container[$item["compname"]][$item["level"]] += 1;
      }
      else
      {
        $container[$item["compname"]][1] += 1;
      }
    }
    //now we check for people that should have the competency but dont and add them to the no lvl list
    foreach ($container as $itemKey => $item)
    {
      $lvls = 0;
      foreach ($item as $lvl)
      {
        $lvls += $lvl;
      }
      $dif = ($numberOfPersons - $lvls);
      //if we have a positive number we add this to the nolevel
      if ($dif > 0)
      {
        $container[$itemKey]["nolvl"] = $dif;
      }
      else
      {
        $container[$itemKey]["nolvl"] = 0;
      }
    }
    //we want to be able to show what competences/levels belong to the profile so we can give a differnt color to that cell
    $profileQuery = &$db->createQuery();
    $profileQuery->addTable("competency_profile_comp");
    $profileQuery->addField("max(level) as level");
    $profileQuery->addFIeld("competency.name as comp");
    $profileQuery->addJoin("competency", "", "competency.id = competency_profile_comp.competency_id", true);
    $profileQuery->addJoin("competency_niveau", "", "competency_niveau.id = competency_profile_comp.niveau_id", true);
    $profileQuery->addCondition("profile_id = " . $profile);
    $profileQuery->addCondition("domain_id=" . $domain);
    $profileQuery->addGroupBy("competency_profile_comp.competency_id");
    $levels = $db->getrows($profileQuery->buildSelect());
    $profiles = array();
    //we put the profiles in an array we can use
    foreach ($levels as $level)
    {
      $profiles[$level["comp"]] = $level["level"] != null ? $level["level"] : 1;
    }
    //simple query to get the max number of levels
    $levelQuery = &$db->createQuery();
    $levelQuery->addTable("competency_niveau");
    $levelQuery->addField("max(level) as level");
    $maxLevel = $db->getrows($levelQuery->buildSelect());
    //we create the header
    $head = array(
      "Competency" ,
      atkText("no lvl", "project"));
    for ($i = 0; $i < $maxLevel[0][level]; $i ++)
    {
      $head[] = "lvl " . ($i + 1);
    }
    $data[] = $head;
    //now we make the data so we can add it to the table
    foreach ($container as $itemKey => $item)
    {
      $row = array();
      $row[] = array(
        "data" => $itemKey);
      $row[] = array(
        "data" => $item["nolvl"]);
      for ($i = 0; $i < $maxLevel[0][level]; $i ++)
      {
        $tData = array();
        $tData["data"] = isset($item[$i + 1]) ? $item[$i + 1] : 0;
        if ($i + 1 == $profiles[$itemKey])
        {
          $tData["color"] = "#c2e158";
        }
        $row[] = $tData;
      }
      $data[] = $row;
    }
    $classname = getClassName(moduleDir('competency').'class.atkextendedtablerenderer.inc');
    atkimport(moduleDir('competency').'class.atkextendedtablerenderer.inc',true,true);

    $tbl = new atkExtendedTableRenderer();
    $output .= $tbl->render($data, TBL_DATA);
    return $output;
  }
}