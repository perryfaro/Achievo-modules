<?php
atkimport('atk.front.atkfrontcontroller');

class teamController extends atkFrontController
{

  var $id;

  var $level;

  var $person;

  var $m_user;

   /**
    * renders the screen
    * @return voidr
    */
  public function showAction()
  {
    $this->mythemedir = moduleDir('competency') . 'themes/default/';
    $this->m_user = getUser();

    $competencyNode = atkGetNode('competency.teamview');

    if ($competencyNode->allowed('showallteams'))
    {
      $page = atkPage::getInstance();
      $this['teams'] = $this->getTeams();
      $this->script = "setListeners();";
    }
    $this->mythemedir = moduleDir('competency') . 'themes/default/';
    $this->renderAsTableAction();
  }

    /**
     * sets some styles and scripts.
     * @return void
     */
  public function preFilter()
  {
    $page = atkPage::getInstance();
    $page->register_style(moduleDir("competency") . "styles/legenda.css");
    $page->register_loadscript("setObserv();");
    $page->register_loadscript("initLegenda();");
  }

  /**
   * prints the text we want to show in our mouseover div
   *
   */
  public function getTextAction()
  {
    $this->id = $_GET['id'];
    $this->m_user = getUser();
    $this->level = $_GET['level'];
    $this->person = $_GET['user'];
    $this->comp = $_GET['comp'];
    if ($this->id == - 1)
    {
      print(atkText("no_level_data_available", "competency", "", $this->user["lng"]) . "<BR>" . atkText("this_means_this_is_a_competency_without_levels", "competency", "", $this->user["lng"]));
      print($shouldHave = $this->printShouldHave($this->comp));
    }
    else if ($this->level == 0)
    {
      $shouldHave = $this->printShouldHave();
      if ($shouldHave != "")
      {
        print("<b>" . atktext("there_are_no_acquired_levels-for_this_competency", "competency", "", $this->user["lng"]) . "</b>");
        print $shouldHave;
      }
      else
      {
        print(atktext("this_competency_is_not_nessesary", "competency", "", $this->user["lng"]));
      }
    }
    else
    {
      $db = &atkGetDb();
      //prepare output
      //prepare the query for getting the data
      $query = &$db->createQuery();
      //we add the table competences
      $query->addTable("competency_niveau");
      $query->addField("name");
      $query->addField("level");
      $query->addField("competency_id");
      $query->addField("id");
      $query->addCondition("competency_id =" . $this->id);
      $query->addCondition("level =" . $this->level);
      $competences = $db->getrows($query->buildSelect());
      //since we get the info by id we are sure its only 1 record at most so we dont need any loops
      $name = $competences[0]["name"];
      $level = $competences[0]["level"];
      $comp = $competences[0]["competency_id"];
      $query = &$db->createQuery();
      //we add the table competences
      $query->addTable("competency_niveau");
      $query->addField("max(level) as level");
      $query->addCondition("competency_id =" . $comp);
      $competences = $db->getrows($query->buildSelect());
      $max = $competences[0]["level"];
      print("<b>" . atktext("has", "competency", "", $this->user["lng"]) . "</b><br><b>" . $name . "</b><br><b>Level</b> " . $level . " of " . $max);
      $shouldHave = $this->printShouldHave();
      if ($shouldHave != "")
      {
        print $shouldHave;
      }
    }
  }

    /**
     * retunst html output with should haves
     * @param int $comp competency id
     * @return string
     */
  function printShouldHave($comp=0)
  {
    $db = &atkGetDb();
    $output = "";
    $query = &$db->createQuery();
    if($comp==0)
    {
      //we add the table competences
      $query->addTable("competency_profile_person");
      $query->addJoin("competency_profile_comp", "", "competency_profile_person.profile_id= competency_profile_comp.profile_id", true);
      $query->addJoin("competency_profile", "", "competency_profile.id= competency_profile_person.profile_id", true);
      $query->addJoin("competency", "", "competency_profile_comp.competency_id = competency.id", true);
      $query->addJoin("competency_niveau", "", "competency_profile_comp.niveau_id = competency_niveau.id", true);
      $query->addCondition("competency_profile_person.person_id =" . $this->person);
      $query->addField("competency_niveau.name as lName");
      $query->addField("max(level) as level");
      $query->addField("competency_profile_comp.competency_id");
      $query->addCondition("competency_profile_comp.competency_id =" . $this->id);
      $query->addGroupBy("competency_profile_comp.competency_id");
      $query->addCondition("level IN (SELECT MAX(level) FROM competency_niveau WHERE competency_niveau.id IN (SELECT competency_profile_comp.niveau_id FROM competency_profile_comp WHERE competency_profile_comp.competency_id =" . $this->id . " AND profile_id in (SELECT profile_id from competency_profile_person WHERE person_id =" . $this->person . ")) GROUP BY competency_niveau.competency_id)");
    }
    else
    {
      //we add the table competences
      $query->addTable("competency_profile_person");
      $query->addJoin("competency_profile_comp", "", "competency_profile_person.profile_id= competency_profile_comp.profile_id", true);
      $query->addJoin("competency_profile", "", "competency_profile.id= competency_profile_person.profile_id", true);
      $query->addCondition("competency_profile_person.person_id =" . $this->person);
      $query->addCondition("competency_profile_comp.competency_id =" . $this->comp);
      $query->addField("competency_profile_comp.competency_id");
    }
    $goals = $db->getrows($query->buildSelect());
    //we know we get at most only one record as we only select only one compeetncy and that can have only 1 max
    //so we check if we have records at all to see what we have to display
    if (count($goals) > 0)
    {
      if($comp==0)
      {
        $output = "<br><br><b>" . atktext("should_have", "competency", "", $this->user["lng"]) . "</b><br>";
        $output .= $goals[0]["lName"] . "<br>Level" . $goals[0]["level"];
      }
      else
      {
        $output = "<br><br><b>" . atktext("person_should_have", "competency", "", $this->user["lng"]) . "</b><br>";
      }

    }
    return $output;
  }

   /**
    * renders the data we have as a table
    * @return void
    */
  function tableAction()
  {
    $this->renderAsTableAction();
  }

    /**
     * the actual construction of the table we want to render to the screen
     * @return void
     */
  function renderAsTableAction()
  {
    $user = getUser();
    if (isset($_GET['id']))
    {
      $id = $_GET['id'];
    }
    else
    {
      $id = $user['id'];
    }
    $session = atkSessionManager::getSession();
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
    $query->addCondition("person_id IN (SELECT id FROM person WHERE id=" . $id . " OR supervisor =" . $id . ")");
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
    //we add the table competences
    $personNode = atkgetNode('person.person');
    $persons = $personNode->select("status='active' AND role='employee' AND id IN (SELECT id FROM person WHERE id=" . $id . " OR supervisor =" . $id . ")")->includes(array('firstname','lastname','id'))->orderBy('lastname');

//    $countPersons = count($persons);
    $table = "<table class=\"myTable\">";
    foreach ($competences as $competency)
    {
      $views[$competency["dName"]][$competency["cName"] . "~#~" . $competency["cID"]][$competency["firstname"] . " " . $competency["lastname"]]["level"] = $competency["level"] != null ? $competency["level"] : 1;
      $views[$competency["dName"]][$competency["cName"] . "~#~" . $competency["cID"]][$competency["firstname"] . " " . $competency["lastname"]]["orglevel"] = $competency["orglevel"] != null ? $competency["orglevel"] : 1;
      $views[$competency["dName"]][$competency["cName"] . "~#~" . $competency["cID"]][$competency["firstname"] . " " . $competency["lastname"]]["id"] = $competency["nID"] != null ? $competency["cID"] : - 1;
      $views[$competency["dName"]][$competency["cName"] . "~#~" . $competency["cID"]][$competency["firstname"] . " " . $competency["lastname"]]["cId"] = $competency["nID"] == null ? $competency["cID"] : - 1;
      $views[$competency["dName"]][$competency["cName"] . "~#~" . $competency["cID"]][$competency["firstname"] . " " . $competency["lastname"]]["pId"] = $competency["pId"] != null ? $competency["pId"] : - 1;
    }

    $views = $this->getGoals($views);
    $head = array();
    $head[] = "&#32;";
    foreach ($persons as $person)
    {
      $head[] = "<B>" . $person["firstname"] . " " . $person["lastname"] . "</B>";
    }
    $table .= $this->getRow($head);
    if (count($views) > 0)
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
              if(isset($item[$person["firstname"] . " " . $person["lastname"]]["level"]))
              {
                $values[] = array('level'=>$item[$person["firstname"] . " " . $person["lastname"]]["level"],'orglevel'=>$item[$person["firstname"] . " " . $person["lastname"]]["shouldlevel"]);
              }
              else
              {
                $values[] = array('level'=>0,'orglevel'=>$item[$person["firstname"] . " " . $person["lastname"]]["shouldlevel"]);
              }
              $mouseOvers[] = $item[$person["firstname"] . " " . $person["lastname"]]["id"] . "#" . $item[$person["firstname"] . " " . $person["lastname"]]["pId"] . "#" . $item[$person["firstname"] . " " . $person["lastname"]]["cId"];
            }
            else
            {
              $values[] = array('level'=>0);
              $mouseOvers[] = $keydata[1] . "#" . $person["id"];
            }
          }
          $table .= $this->getRow($values, $mouseOvers);
        }
      }
    }

    $table .= "</table>";
    $this->table = $table;
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

    if(is_array($value))
    {
      $orgLvl = $value['orglevel'];
      $value= $value['level'];

      if($value < $orgLvl)
      {
        $tdstyle='class="toLow"';
      }elseif ($value==$orgLvl)
      {
        $tdstyle='class="good"';
      }
      elseif ($value>$orgLvl)
      {
        $tdstyle='class="best"';
      }
    }

    if ($mouseOver == - 2)
    {
      return ("<TD>" . $value . "</TD>");
    }
    else
    {
      $values = explode("#", $mouseOver);
      return ("<TD $tdstyle onmouseover=mouseOverCell(event," . $values[0] . "," . $value . "," . $values[1] . "," . (int)$values[2]. ") onmouseout=mouseOutCell()>" . $value . "</TD>");
    }
  }

    /**
     * gets the available teams in the company
     * @return string html sting for a edit.
     */
  private function getTeams()
  {
    $personNode = atkgetNode('person.person');
    $persons = $personNode->select("status='active' AND role='employee' AND id IN (SELECT distinct(supervisor) FROM person)")->includes(array('firstname','lastname','id'))->orderBy('lastname');

    //we make 2 arrays we can use in the list attribute
    foreach ($persons as $person)
    {
      $options[] = $person["firstname"] . " " . $person["lastname"];
      $values[] = $person["id"];
    }
    $teamAttr = new atkListAttribute("teams", $options, $values);
    $teamAttr->setFlags(AF_LIST_NO_NULL_ITEM);
    return $teamAttr->edit();
  }

   /**
    * Retuns the goals that members of this team have set
    * @param  $views
    * @return
    */
  private function getGoals($views)
  {
    $user = getUser();
    if (isset($_GET['id']))
    {
      $id = $_GET['id'];
    }
    else
    {
      $id = $user['id'];
    }

    $db = &atkGetDb();
    $output = "";
    $query = &$db->createQuery();
    //we add the table competences
    $query->addTable("competency_profile_person");
    $query->addJoin("competency_profile_comp", "", "competency_profile_person.profile_id= competency_profile_comp.profile_id", true);
    $query->addJoin("competency_profile", "", "competency_profile.id= competency_profile_person.profile_id", true);
    $query->addJoin("competency", "", "competency_profile_comp.competency_id = competency.id", true);
    $query->addJoin("competency_niveau", "", "competency_profile_comp.niveau_id = competency_niveau.id", true);
    $query->addJoin("competency_domain", "", "competency.domain_id = competency_domain.id", true);
    $query->addJoin("person", "", "person.id=competency_profile_person.person_id", true);
    $query->addCondition("person_id IN (SELECT id FROM person WHERE id=" . $id . " OR supervisor =" . $id . ")");
    $query->addField("competency_domain.name as dName");
    $query->addField("competency.name as cName");
    $query->addField("lastname");
    $query->addField("firstname");
    $query->addField("person.id as pId");
    $query->addField("competency_niveau.name as lName");
    $query->addField("competency.domain_id as dID");
    $query->addField("competency_niveau.id as nID");
    $query->addField("competency.id as cID");
    $query->addField("max(level) as level");
    $query->addField("competency_profile_comp.competency_id");

    $query->addGroupBy("competency.domain_id");
    $query->addGroupBy("competency.id");
    $query->addGroupBy("person_id");

    $goals = $db->getrows($query->buildSelect());

      $table = "<table class=\"myTable\">";
    foreach ($goals as $competency)
    {
      if(!isset($views[$competency["dName"]][$competency["cName"] . "~#~" . $competency["cID"]][$competency["firstname"] . " " . $competency["lastname"]]))
      {
        $views[$competency["dName"]][$competency["cName"] . "~#~" . $competency["cID"]][$competency["firstname"] . " " . $competency["lastname"]]["id"] = !is_null($competency["nID"]) ? $competency["cID"] : - 1;
        $views[$competency["dName"]][$competency["cName"] . "~#~" . $competency["cID"]][$competency["firstname"] . " " . $competency["lastname"]]["pId"] = !is_null($competency["pId"]) ? $competency["pId"] : - 1;
        $views[$competency["dName"]][$competency["cName"] . "~#~" . $competency["cID"]][$competency["firstname"] . " " . $competency["lastname"]]["cId"] = is_null($competency["nID"]) ? $competency["cID"] : - 1;
      }
       $views[$competency["dName"]][$competency["cName"] . "~#~" . $competency["cID"]][$competency["firstname"] . " " . $competency["lastname"]]["shouldlevel"] = $competency["level"] != null ? $competency["level"] : 1;
    }

    return $views;
  }
}
?>