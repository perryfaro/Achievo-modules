<?php
require_once (atkconfig("atkroot") . "atk/utils/adodb-time.inc.php");

/**
 * This class will generate graphs to show the growth of competences
 *
 */
class competencegrowthteam extends atkNode
{

  var $m_viewstart = "";

  var $m_viewend = "";

  var $m_team = "";

  var $m_legenda = "";

  var $m_profile = "";

  function competencegrowthteam()
  {
    $this->atkNode("competencegrowthteam");
    $sessionManager = &atkGetSessionManager();
    $this->m_viewstart = $sessionManager->stackVar("viewstart");
    if ($this->m_viewstart == "")
    {
      $this->m_viewstart = date("Y-m-d", adodb_mktime(0, 0, 0, date("m") - 1, date("d"), date("Y")));
    }
    else if (is_array($this->m_viewstart))
    {
      $this->m_viewstart = sprintf("%04d-%02d-%02d", $this->m_viewstart["year"], $this->m_viewstart["month"], $this->m_viewstart["day"]);
    }
    $this->m_viewend = $sessionManager->stackVar("viewend");
    if ($this->m_viewend == "")
    {
      $this->m_viewend = date("Y-m-d");
    }
    else if (is_array($this->m_viewend))
    {
      $this->m_viewend = sprintf("%04d-%02d-%02d", $this->m_viewend["year"], $this->m_viewend["month"], $this->m_viewend["day"]);
    }
    $this->m_team = $sessionManager->stackVar("team");
    if ($this->m_team == "")
    {
      $this->m_team = 0;
    }
    $this->m_profile = $sessionManager->stackVar("profile");
    if ($this->m_profile == "")
    {
      $this->m_profile = 0;
    }
    if (isset($_GET["showLegenda"]))
    {
      $this->m_legenda = $_GET["showLegenda"];
    }
    else
    {
      $this->m_legenda = 0;
    }
  }

  function action_stats(&$handler)
  {
    $ui = &$this->getUi();
    $page = &$this->getPage();
    $page->register_script(moduleDir("competency") . "javascript/competencygrowth.js");
    $page->register_scriptcode("setObservers();");
    $page->register_style(moduleDir("competency") . "styles/legenda.css");
    $this->addStyle("style.css");
    if ($this->m_legenda != 0)
    {
      $page->register_loadscript("toggleLegenda()");
    }
    $page->register_loadscript("initLegenda()");
    //we say we want to use the datagrapattribute
    useattrib("competency.extendedDataGraphAttribute");
    $params["resolution"] = $this->m_resolution;
    $params["user"] = $this->m_user;
    $params = array();
    $params["startdate"] = $this->m_viewstart;
    $params["enddate"] = $this->m_viewend;
    $params["team"] = $this->m_team;
    $params["profile"] = $this->m_profile;
    //we want to see graphs for every domain in the profile of the person we chose
    $db = &atkGetDb();
    //prepare output
    $output = "";
    //prepare the query for getting the data
    $query = &$db->createQuery();
    //we add the table competences
    $query->addTable("competency");
    $query->addJoin("competency_domain", "", "competency_domain.id = competency.domain_id");
    //if we search for a team we ad an extra condition
    if ($params["team"] > 0)
    {
      $query->addCondition("competency.id IN (SELECT distinct(competency_id) FROM competency_competences WHERE person_id in(SELECT id FROM person where person_id=" . $params["team"] . "  OR supervisor =" . $params["team"] . "))");
      //$query->addCondition("competency.id IN (SELECT competency_id FROM competency_profile_comp WHERE profile_id in(SELECT profile_id from competency_profile_person where person_id in(SELECT id FROM person where person_id=" . $params["team"] . "  OR supervisor =" . $params["team"] . ")))");
    }
    else
    {
      $query->addCondition("competency.id IN (SELECT distinct(competency_id) FROM competency_competences)");
    }
    $query->addField("distinct(competency.domain_id) as domain");
    $query->addField("competency_domain.name as dName");
    $domains = $db->getrows($query->buildSelect());
    foreach ($domains as $domain)
    {
      $params["domain"] = $domain["domain"];
      $dname = str_replace(" ", "_", $domain["dName"]);
      $graph = &new extendedDataGraphAttribute("Teamview", "growthTeam", $params, "combBar");
      $graph->setSource("competency.competency");
      $graphString .= chr(13) . "<div class=\"domain\">" . chr(13);
      $graphString .= chr(13) . "<img id='img_" . $dname . "' align'=\"left\" src=\"./atk/images/minus.gif\" onclick=\"expand('bin" . $dname . "','img_" . $dname . "')\"> " . $domain["dName"];
      $graphString .= "<br><div id=bin" . $dname . ">" . $graph->showGraph() . "</div>";
      $graphString .= "</div>";
    }
    $user =  atkGetUser();
    if (isset($user['id']))
    {
      $boxes = array(
        $ui->renderBox(array(
          "title" => atkText("select", "competency") . " " . atkText("growth", "competency") . " " . $this->m_username ,
          "content" => "<BR>" . $this->navigator() . "<br>")) ,
        $ui->renderBox(array(
          "title" => atkText("overview", "competency") . " " . atkText("growth", "competency") . " " . $this->m_username ,
          "content" => "<BR>" . $graphString . "<br>" . $this->getLegenda())));
    }
    else
    {
      $boxes = array(
        $ui->renderBox(array(
          "title" => atkText("overview", "competency") . " " . atkText("growth", "competency") . " " . $this->m_username ,
          "content" => atktext('this_page_cannot_be_viewed_as_administrator', 'competency'))));
    }
    $actionpage = $this->renderActionPage("stats", $boxes);
    $page->addContent($actionpage);
  }

  /**
   * Creates the navifgator menu on top of the page to select what start/end date we want and if we want to see what team
   *
   * @return html string for navigator
   */
  function navigator()
  {
    $user = getUser();
    useattrib("atkdateattribute");
    $startdate = $this->m_viewstart;
    $enddate = $this->m_viewend;
    $list = $this->m_team;
	atk_var_dump($list);
    $prof = $this->m_profile;
    $dummyrec = array(
      "viewstart" => array(
        "year" => substr($startdate, 0, 4) ,
        "month" => substr($startdate, 5, 2) ,
        "day" => substr($startdate, 8, 2)) ,
      "viewend" => array(
        "year" => substr($enddate, 0, 4) ,
        "month" => substr($enddate, 5, 2) ,
        "day" => substr($enddate, 8, 2)));
    $startdateatt = new atkDateAttribute("viewstart", "", "", 0, date("Ymd"));
    $enddateatt = new atkDateAttribute("viewend", "", "", 0, date("Ymd"));
    $showLegendaAtrr = new atkBoolAttribute("showLegenda");
    $db = atkGetDb();
    //prepare the query for getting the data\
    $query = $db->createQuery();
    //we add the table competences
    $query->addTable("person");
    $query->addField("lastname");
    $query->addField("firstname");
    $query->addField("id");
    $query->addCondition('status="active"');
    $query->addCondition("role=\"employee\"");
    //create the dropdown
    if ($this->allowed('show_all_users'))
    {
      $query->addCondition("id IN(SELECT distinct(supervisor) fROM person)");
    }
    else if ($this->allowed('show_own_team'))
    {
      $query->addCondition("id =" . $user["id"]);
    }
    else
    {
      $query->addCondition("id =0");
    }
    $persons = $db->getrows($query->buildSelect());
    $valuesArray = array();
    $optionsArray = array();
    foreach ($persons as $person)
    {
      $valuesArray[] = $person["id"];
      $optionsArray[] = $person["firstname"] . " " . $person["lastname"];
    }
    $query = $db->createQuery();
    $query->addTable("competency_profile");
    $query->addField("id");
    $query->addField("name");
    $profiles = $db->getrows($query->buildSelect());
    //we make two arrays we can use for the list attribute
    foreach ($profiles as $profile)
    {
      $profValues[] = $profile["id"];
      $profOptions[] = $profile["name"];
    }
    $profileatt = new atkListAttribute("profile", $profOptions, $profValues);
    $profileatt->setInitialValue($prof);
    $profileatt->setEmptyValue(0);
    $listAtt = new atkListAttribute("team", $optionsArray, $valuesArray);
    $listAtt->setInitialValue($list);
    $listAtt->setEmptyValue(0);
    $output = '<form action="' . getDispatchFile() . '" method="get">';
    $output .= session_form();
    $output .= '<input type="hidden" name="atknodetype" value="' . $this->atkNodeType() . '">';
    $output .= '<input type="hidden" name="atkaction" value="' . $this->m_action . '">';
    //now we create the table for a better layout
    $output .= "<table>";
    $output .= "<tr>";
    $output .= "<td>" . atkText("start date", "competency") . ':</td>';
    $output .= "<td>" . $startdateatt->edit($dummyrec) . '</td>';
    $output .= "</tr><tr>";
    $output .= "<td>" . atkText("end date", "competency") . ':</td>';
    $output .= "<td>" . $enddateatt->edit($dummyrec) . "</td>";
    $output .= "</tr><tr>";
    $output .= "<td>" . atkText("coordinator", "competency") . ":</td>";
    $output .= "<td>" . $listAtt->edit(array('team'=>$list)) . "</td>";
    $output .= "</tr><tr>";
    $output .= "<td>" . atkText("_profile", "competency") . ":</td>";
    $output .= "<td>" . $profileatt->edit(array(
      "profile" => $this->m_profile)) . "</td>";
    $output .= "</tr><tr>";
    $output .= "<td>" . atkText("show_legenda", "competency") . ":</td>";
    $output .= "<td>" . $showLegendaAtrr->edit(array(
      "showLegenda" => $this->m_legenda)) . "</td>";
    $output .= "</tr>";
    $output .= "</tr>";
    $output .= "</table>";
    $output .= ' &nbsp;<br><input type="submit" value="' . atkText("refresh", "competency") . '">';
    $output .= '</form>';
    return $output;
  }

  /**
   * Checks if the current user has the right to view all users
   *
   * @return Boolean
   */
  function userHasViewAll()
  {
    $user = getUser();
    $db = &atkGetDb();
    //prepare the query for getting the data\
    $query = &$db->createQuery();
    $query->addTable("person"); //id
    $query->addJoin("employeerole", "", "employee_id=id", person);
    $query->addJoin("accessright", "", "employeerole.role_id=accessright.role_id", employeerole);
    $query->addField("lastname", "", "person");
    $query->addField("id", "", "person");
    $query->addCondition("accessright.action ='show_all_users'");
    $query->addCondition("accessright.node ='competency.competenceteamgrowth'");
    if ($user[id])
    {
      $query->addCondition("person.id =" . $user[id]);
    }
    $rights = $db->getrows($query->buildSelect());
    if (count($rights) > 0)
    {
      return true;
    }
    return false;
  }

  function getLegenda()
  {
    $output = "<div class=legenda id=legenda>";
    $output .= "Legenda";
    $output .= "<div id=green class=green>" . atktext("more_then_in_profile", "competency") . "</div>";
    $output .= "<div id=blue class=blue>" . atktext("currentely_acquired", "competency") . "</div>";
    $output .= "<div id=red class=red>" . atktext("in_profile_but_not acquired", "competency") . "</div>";
    $output .= "</div>";
    $output .= "<script type=\"text/javascript\">toggleLegenda()</script>";
    return $output;
  }

  function userHasViewTeam()
  {
    $user = getUser();
    $db = &atkGetDb();
    //prepare the query for getting the data\
    $query = &$db->createQuery();
    $query->addTable("person"); //id
    $query->addJoin("employeerole", "", "employee_id=id", person);
    $query->addJoin("accessright", "", "employeerole.role_id=accessright.role_id", employeerole);
    $query->addField("lastname", "", "person");
    $query->addField("id", "", "person");
    $query->addCondition("accessright.action ='show_own_team'");
    $query->addCondition("accessright.node ='competency.competencegrowthteam'");
    if ($user[id])
    {
      $query->addCondition("person.id =" . $user[id]);
    }
    $rights = $db->getrows($query->buildSelect());
    if (count($rights) > 0)
    {
      return true;
    }
    return false;
  }
}
?>
