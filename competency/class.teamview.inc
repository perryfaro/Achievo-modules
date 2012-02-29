<?php

/**
 * Here we create a view wheer we can see all the members of a certain team with there competences
 *
 */
class teamview extends atkNode
{

  var $m_user = "";

  function teamview()
  {
    $sessionManager = &atkGetSessionManager();
    $this->atkNode("teamview");
    $this->m_user = getUser();
  }

  function action_stats(&$handler)
  {
    $ui = &$this->getUi();
    $page = &$this->getPage();
    $this->addStyle("style.css");
    $page->register_style(moduleDir("competency") . "styles/web.css");
    $page->register_script(moduleDir("competency") . "javascript/teamView.js");
    $page->register_loadscript("modDir='" . moduleDir("competency") . "';");
    $page->register_loadscript("$('compover').toggle();");
    $output = "";
    if (isset($this->m_user['id']))
    {
      if ($this->userHasViewAll())
      {
        $output .= $this->getTeams();
        $page->register_loadscript("setListeners();");
      }
      $output .= "<div id=teamtable>" . $this->_renderAsTable() . "</div>";
      $output .= "<div class=compover id=compover></div>";
      $output = $ui->render(moduleDir("competency") . "templates/team.tpl", $params, "competency");
    }
    else
    {
      $output=atktext('this_page_cannot_be_viewed_as_administrator', 'competency');
    }
    $boxes = array(
      $ui->renderBox(array(
        "title" => atkText("stats", "competency") . " " . atkText("team_view", "project") ,
        "content" => $output)));
    $actionpage = $this->renderActionPage("stats", $boxes);
    $page->addContent($actionpage);
  }

  function _renderAsTable()
  {
    //open database
    $db = &atkGetDb();
    //prepare output
    $output = "";
    //prepare the query for getting the data
    $query = &$db->createQuery();
    //we add the table competences
    $query->addTable("competency_competences");
    $query->addJoin("competency", "", "competency.id=competency_competences.competency_id", true);
    $query->addJoin("competency_niveau", "", "competency_niveau.id=competency_competences.niveau_id", true);
    $query->addJoin("competency_domain", "", "competency.domain_id = competency_domain.id", true);
    $query->addJoin("person", "", "competency_competences.person_id=person.id", true);
    $query->addCondition("person_id IN (SELECT id FROM person WHERE id=" . $this->m_user["id"] . " OR supervisor =" . $this->m_user["id"] . ")");
    $query->addField("competency.name as cName");
    $query->addField("competency_domain.name as dName");
    $query->addField("competency_niveau.name as nName");
    $query->addField("competency_competences.person_id as pId");
    $query->addField("added");
    $query->addField("lastname");
    $query->addField("firstname");
    $query->addField("competency.id as cID");
    $query->addField("competency_domain.id as dID");
    $query->addField("competency_niveau.id as nID");
    $query->addField("max(level) as level");
    $query->addField("level as orglevel");
    $query->addGroupBy("competency_domain.id");
    $query->addGroupBy("competency.id");
    $query->addGroupBy("competency_competences.person_id");
    $competences = $db->getrows($query->buildSelect());

    $persons = atkgetNode('person.person')->select("person.id IN (SELECT id FROM person WHERE id=" . (int)$this->m_user["id"] . " OR supervisor =" . (int)$this->m_user["id"] . ")")->includes(array('lastname','firstname','id'))->allRows();

    $countPersons = count($persons);
    $table = "<table class=\"myTable\">";

    $views = array();

    foreach ($competences as $competency)
    {
      $views[$competency["dName"]][$competency["cName"] . "~#~" . $competency["cID"]][$competency["firstname"] . " " . $competency["lastname"]]["level"] = $competency["level"] != null ? $competency["level"] : 1;
      $views[$competency["dName"]][$competency["cName"] . "~#~" . $competency["cID"]][$competency["firstname"] . " " . $competency["lastname"]]["orglevel"] = $competency["orglevel"] != null ? $competency["orglevel"] : 1;
      $views[$competency["dName"]][$competency["cName"] . "~#~" . $competency["cID"]][$competency["firstname"] . " " . $competency["lastname"]]["id"] = $competency["nID"] != null ? $competency["cID"] : - 1;
      $views[$competency["dName"]][$competency["cName"] . "~#~" . $competency["cID"]][$competency["firstname"] . " " . $competency["lastname"]]["pId"] = $competency["pId"] != null ? $competency["pId"] : - 1;
    }
    $head = array();
    $head[] = "&#32;";
    foreach ($persons as $person)
    {
      $head[] = "<B>" . $person["firstname"] . " " . $person["lastname"] . "</B>";
    }
    $table .= $this->getRow($head);
    if(count($views) > 0)
    {
      foreach ($views as $viewKey => $view)
      {
        $values = array();
        $values[] = "<B>" . $viewKey . "</B>";
        $table .= $this->getRow($values);
        foreach ($view as $itemKey => $item)
        {
          $keydata = explode("~#~", $itemKey);
          $values = array();
          $mouseOvers = array();
          $values[] = $keydata[0];
          $mouseOvers[] = "-2";
          foreach ($persons as $person)
          {
            if (count($item[$person["firstname"] . " " . $person["lastname"]]) > 0)
            {
              $values[] = $item[$person["firstname"] . " " . $person["lastname"]]["level"] . '-' . $item[$person["firstname"] . " " . $person["lastname"]]["orglevel"];
              $mouseOvers[] = $item[$person["firstname"] . " " . $person["lastname"]]["id"] . "#" . $item[$person["firstname"] . " " . $person["lastname"]]["pId"];
            }
            else
            {
              $values[] = 0;
              $mouseOvers[] = $keydata[1] . "#" . $person["id"];
            }
          }
          $table .= $this->getRow($values, $mouseOvers);
        }
      }
    }
    $table .= "</table>";
    return $table;
  }

  function getRow($values, $mouseOver = array())
  {
    if (count($values) == count($mouseOver) || count($mouseOver) == 0)
    {
      $output = "<tr>";
      foreach ($values as $keyV => $value)
      {
        if (count($mouseOver) != 0)
        {
          $output .= $this->getCell($value, $mouseOver[$keyV]);
        }
        else
        {
          $output .= $this->getCell($value);
        }
      }
      $output .= "</tr>";
      return $output;
    }
    else
    {
      print("mouseover needs to have the same amount of values as values");
    }
  }

  /**
   * get the HTML for a cell back
   *
   * @param int $value value of the value to eb show in the cell
   * @param string $mouseOver data for the mouse over different date is separated by a #
   * @return html string with td data
   */
  function getCell($value, $mouseOver = -2)
  {
    if ($mouseOver == - 2)
    {
      return ("<TD>" . $value . "</TD>");
    }
    else
    {
      $values = explode("#", $mouseOver);
      return ("<TD onmouseover=mouseOverCell(event," . $values[0] . "," . $value . "," . $values[1] . ") onmouseout=mouseOutCell()>" . $value . "</TD>");
    }
  }

  private function getTeams()
  {
    $personNode = atkgetNode('person.person');
    $persons = $personNode->select("status='active' AND role='employee' AND id IN (SELECT distinct(supervisor) FROM person)")->includes(array('firstname','lastname','id'))->orderBy('lastname');

    //we make 2 arrays we can use in th elist attrubute
    foreach ($persons as $person)
    {
      $options[] = $person["firstname"] . " " . $person["lastname"];
      $values[] = $person["id"];
    }
    $teamAttr = new atkListAttribute("teams", $options, $values);
    $teamAttr->setFlags(AF_LIST_NO_NULL_ITEM);
    return $teamAttr->edit();
  }

  private function userHasViewAll()
  {
    $db = &atkGetDb();
    //prepare the query for getting the data\
    $query = &$db->createQuery();
    $query->addTable("person"); //id
    $query->addJoin("employeerole", "", "employee_id=id", person);
    $query->addJoin("accessright", "", "employeerole.role_id=accessright.role_id", employeerole);
    $query->addField("lastname", "", "person");
    $query->addField("id", "", "person");
    $query->addCondition("accessright.action ='showallteams'");
    $query->addCondition("accessright.node ='competency.teamview'");
    $query->addCondition("person.id =" . $this->m_user["id"]);
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
