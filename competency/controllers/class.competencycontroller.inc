<?php
atkimport('atk.front.atkfrontcontroller');

class competencycontroller extends atkFrontController
{

  private $user = "";

  private $data = "";

  private $allowed = "nothing";

  private $m_userhasviewall = "";

  private $profile = 0;

  private $showExtra = false;

  private $acl;

  /**
   * Takes care of loading stylesheets and javascripts and sets some variables
   * before we do the actual action
   * @return void
   */
  public function preFilter()
  {
    $competencyNode = atkGetNode('competency.competencymatch');
    if ($competencyNode->allowed('show_all_users'))
    {
      $this->allowed = "all";
    }
    elseif($competencyNode->allowed('show_own_team'))
    {
      $this->allowed = "team";
    }
    $this->user = empty($this->request->userId)
                ? atkGetUser()
                : array('id' => $this->request->userId);
    $this->acl = atkNew('module.competency.utils.competencyacl');
    $this->allowAdd = $this->acl->allowCompetencyChange($this->user['id']);
    $this->allowSetProfile = $this->acl->allowSetProfile($this->user['id']);
    $this->date = $_GET['date'];
    $page = atkPage::getInstance();
    $page->register_style(moduleDir("competency") . "styles/web.css");
    $page->register_style(moduleDir("competency") . "styles/proto.menu.0.6.css");
    $page->register_style(atkconfig("atkroot") . "atk/themes/default/styles/atkdateattribute.css");
    $page->register_style("style.css");

    //we register the necessary scripts and style sheets
    //we register our script
    $page->register_script(moduleDir("competency") . "javascript/competency.js");
    //we need to set some vars so menus will show in correct language
    $page->register_scriptcode("addMenu('" . atkText("obtained", "competency") . "','" . atkText("obtained_on", "competency") . "','" . atkText("target", "competency") . "','" . atkText("delete", "competency") . "...');");
    $page->register_script(moduleDir("competency") . "javascript/proto.menu.0.6.js");
    $page->register_script(atkconfig("atkroot") . "atk/javascript/calendar/calendar.js");
    $page->register_script(atkconfig("atkroot") . "atk/javascript/calendar/calendar-runner.js");
    $page->register_script(atkconfig("atkroot") . "atk/javascript/calendar/lang/calendar-" . atkconfig("language") . ".js");
    $page->register_script(atkconfig("atkroot") . "atk/javascript/window/window.js");
    $page->register_scriptcode("setUser(" . $this->user["id"] . ");");
    $page->register_scriptcode("deleteMessage ='" . atkText("are_you_sure_you_want_to_delete", "competency") . "';");

    $page->register_loadscript("allowed='" . $this->allowed . "';");
    $page->register_loadscript("allowedAdd=" .intval($this->allowAdd) . ";");
    $page->register_loadscript("userhasviewall=" .(int)$competencyNode->allowed('show_all_users') . ";");

    $page->register_style(atkconfig("atkroot") . "atk/javascript/window/themes/alphacube.css");
    //We add some load scripts
    $page->register_loadscript("modDir = '" . moduleDir("competency") . "';");
    $page->register_loadscript("setObserv();");
    //Because even after the ajax call we want to make sure user can open the context menus
    if ($this->allowAdd)
    {
      $page->register_loadscript("initContextMenus();");
    }
    $page->register_loadscript("initLegenda();");
    $page->register_loadscript("toggleLegenda();");
    $page->register_loadscript("closeComment();");
    $page->register_loadscript("lng='" . atkLanguage::getUserLanguage() . "';");
    $page->register_loadscript("$('compover').toggle();");
    $page->register_loadscript("$('isset').hide();");
    $page->register_script(atkconfig("atkroot")."atk/javascript/newwindow.js");
  }

  /**
   * Schedules the profile for  the selected person
   *
   */
  function schedule()
  {
    $item = explode("!", $_GET['compId']);
    $table = $item[0];
    $prodid = $item[1];
    $compNiveau = $this->getCompetencyNiveau($prodid);
    switch ($table)
    {
      case "competency":
        $niveau = 0;
        $competency = $prodid;
        break;
      case "niveau":
        $niveau = $prodid;
        $competency = $compNiveau["id"];
        break;
      default:
        break;
    }
    $db = &atkGetDb();
    $query = $db->createQuery();
    $query->addTable("competency_planning");
    $query->addField("competency_id", $competency);
    $query->addField("niveau_id", $niveau);
    $query->addField("person_id", $this->user['id']);
    $query->addField("planDate", $_GET['schedule']);
    $query->addField("remark", $_GET['message']);
    $db->query($query->buildInsert());
  }

  /**
   * Loads all data we need to render the screen we will show to the user where the
   * user can see the users/competences and profiles and switch between them.
   * Then the show template will actualy render the data we collected here
   * @return void
   */
  public function showAction()
  {
    $this->mythemedir = moduleDir('competency') . 'themes/default/';
    $this->profileId = $this->getProfile();
    $this->getcomp();
    $this->fillPers();
    $this->fillProfiles();
    $this->getTodos();
  }

  /**
   * Retuns a list of competences grouped by their domains and have the levels
   * in a tree we can easy use to parse our competency structure
   * @return void
   */
  public function getcomp()
  {
    $domain = $this->structurizeData();
    $this->competences = $this->getActiveCompetences();
    $this->profiles = $this->getCompetencesInProfile($this->getProfile());
    $this->planned = $this->getPlannedCompetences();
    $this->allowed = $allowed;
    $competencyNode = atkGetNode('competency.competencymatch');
    if ($competencyNode->allowed('show_all_users'))
    {
      $params["allowed"] = "team";
    }
    $this->domain = $domain;
  }

  /**
   * reloads the competences after a profile switch
   * @return void
   */
  public function loadCompetencesAction()
  {
    if (isset($this->request->profile))
    {
      $this->profile = $this->request->profile;
    }
    //if the user wants to show the extra competences we set this so we can load
    //the extra competeces from the database
    if (isset($this->request->showExtra) && $this->request->showExtra==1)
      $this->showExtra = true;

    $this->allowAdd = intval($this->acl->allowCompetencyChange($this->user['id']));
    $this->fillProfiles();
    $this->getcomp();
  }

  /**
   * returns the todos a user has
   * @return void
   */
  private function getTodos()
  {
    $competencyNode = atkGetNode('competency.competencymatch');
    //we first check if the user may see this
    if ($this->allowed == "team" || $competencyNode->allowed('show_all_users'))
    {
      $db = atkGetDb();
      $query = $db->createQuery();
      $query->addTable("competency_planning");
      $query->addCondition("person_id=" . (int) $this->request->userId);
      $query->addCondition("competency_planning.niveau_id not in (Select niveau_id from competency_competences WHERE person_id=" . $this->user["id"] . ")");
      $query->addJoin("competency", "", "competency.id=competency_planning.competency_id", true);
      $query->addJoin("competency_niveau", "", "competency_niveau.id=competency_planning.niveau_id", true);
      $query->addJoin("competency_domain", "", "competency.domain_id=competency_domain.id", true);
      $query->addField("competency_domain.name as domName");
      $query->addField("planDate");
      $query->addField("remark");
      $query->addField("competency_niveau.name as nivName");
      $query->addField("competency.name as compName");
      $planDates = $db->getRows($query->buildSelect());
      $todos = array();
      //we make a pretty array of all we can use later in our template
      foreach ($planDates as $date)
      {
        $todos[$date["domName"]][$date["compName"]][$date["nivName"]][$date["planDate"]] = $date["remark"];
      }
      $query = $db->createQuery();
      $query->addTable("competency_comments");
      $query->addCondition("person_id=" . $this->user["id"]);
      $query->addJoin("competency", "", "competency.id=competency_comments.competency_id", true);
      $query->addJoin("competency_domain", "", "competency.domain_id=competency_domain.id", true);
      $query->addField("competency_domain.name as domName");
      $query->addField("comment");
      $query->addField("competency.name as compName");
      $comments = $db->getRows($query->buildSelect());
      foreach ($comments as $coment)
      {
        if (! isset($todos[$coment["domName"]][$coment["compName"]]))
        {
          $todos[$coment["domName"]][$coment["compName"]] = "";
        }
        $comments[$coment["domName"]][$coment["compName"]]["comp_comment"] = $coment["comment"];
      }
      $this->todos = $todos;
      $this->comments = $comments;
    }
  }


  function fillList()
  {
    $domain = $this->structurizeData();
    //the array we use to see if  a person already has a competence
    $competences = $this->getActiveCompetences();
    $profiles = $this->getCompetencesInProfile($this->getProfile());
    $ui = $this->getUi();
    $params["domain"] = $domain;
    $params["competences"] = $competences;
    $params["profiles"] = $profiles;
    $params["allowed"] = $this->allowed;
    $competencyNode = atkGetNode('competency.competencymatch');
    if ($competencyNode->allowed('show_all_users'))
    {
      $params["allowed"] = "team";
    }
    return $output . $ui->render(moduleDir("competency") . "templates/competencymatch.tpl", $params, "competency");
  }

  /**
   * Gets all the data to fill the user dropdown
   *
   */
  public function fillPers()
  {
    //Select al people that we just made a select criterium for but are  active and are an employee
    $this->pers = $this->user['id'];
    $this->persons = $this->acl->getSelectableEmployees(array('id', 'firstname', 'lastname'));
    $this->comment = $this->getComment();
  }

  /**
   * fills the profile tabs with the profiles a person has matched to him
   *
   * @return string the display html string
   */
  function fillProfiles()
  {
    $db = &atkGetDb();
    $this->profileId = $this->getProfile();
    $profiles = atkGetNode('competency.competencyprofile')->select()->includes(array('name','id'))->allRows();
    $this->listprofiles = $profiles;
  }

  /**
   * Retuns the competences this person already has
   *
   * @return Array of competences
   */
  function getActiveCompetences()
  {
    $competencesNode = atkgetNode('competency.competences');
    $competences = $competencesNode->select("person_id=" . $this->user['id'])->includes(array('niveau_id','competency_id','person_id'));
    $level = array();
    $comp = array();
    foreach ($competences as $competency)
    {
      $level[] = $competency["niveau_id"]['id'];
      $comp[] = $competency["competency_id"]['id'];
    }
    $output = array(
      "level" => $level ,
      "comp" => $comp);
    return ($output);
  }

  function getPlannedCompetences()
  {
       $competencesNode = atkgetNode('competency.competenceplanning');
    $competences = $competencesNode->select("person_id=" . $this->user['id'])->includes(array('niveau_id','competency_id','person_id'));
    $level = array();
    $comp = array();
    foreach ($competences as $competency)
    {
      $level[] = $competency["niveau_id"]['id'];
      $comp[] = $competency["competency_id"]['id'];
    }
    $output = array(
      "level" => $level ,
      "comp" => $comp);
    return ($output);

  }

  /**
   * We get all the data out of the database and format them in an array so we can later
   * use this array to create a neat structure of divs to display the competences in
   *
   * @return Array[domain][competence][niveau]=> id/level/name
   */
  function structurizeData()
  {
    $profile = $this->getProfile();
    $compsinprofile = $this->getCompetencesForShow($profile);

    if (count($compsinprofile) > 0)
    {
      $filter = 'competency_id.id in (' . implode(',',$compsinprofile) . ')';
    }

    $niveauNode = atkGetNode('competency.competencyniveau');
    $niveauNode->addFilter($filter);

    $competences = $niveauNode->select()->allRows();
    $domain = array();
    //we put the data in a handy array we can use to write away the divs
     foreach ($competences as $competence)
    {
      $domain[$competence["competency_id"]['domain_id']['name']][$competence["competency_id"]['name'] . "!" . $competence["competency_id"]['id']][$competence["name"]] = $competence["id"];
    }

    if (count($compsinprofile) > 0)
    {
      $compfilter = ' AND competency.id in (' . implode(',',$compsinprofile) . ')';
    }

    $compNode = atkGetNode('competency.competency');
    $competences = $compNode->select("competency.id NOT IN (SELECT competency_id FROM competency_niveau)".$compfilter)->allRows();
    //we add these competences to the same list
    foreach ($competences as $competence)
    {
      $domain[$competence["domain_id"]['name']][$competence["name"] . "!" . $competence["id"]]["id"] = $competence["id"];
    }
    return $domain;
  }

  /**
   * we want the id of the profile that was last selected for this person
   *
   * @return integer if the person has a profile return that number else 0 will be returned
   */
  function getProfile()
  {

    if ($this->profile > 0)
    {
      return $this->profile;
    }
    else
    {
      $db = &atkGetDb();
      $query = &$db->createQuery();
      //we add the table competences
      $query->addTable("competency_profile_person");
      $query->addField("profile_id");
      $query->addCondition("person_id=" . $this->user['id']);
      $profiles = $db->getrows($query->buildSelect());
      if (count($profiles) > 0)
      {
        $this->profile = $profiles[0]['profile_id'];
        return ($profiles[0]['profile_id']);
      }
      else
      {
        return 0;
      }
    }
  }

  /**
   * We select all competences we want to show in our match screen this competences can be
   * Competences needed for the profile
   * Competences that are selected as always show for the profile
   * And when show extra competences is selected also the extra competences for the profile
   *
   * @param int $profile The profile we want to select the competences for
   * @param boolean $showExtra Do we want to show the extra competences or not ?
   * @return array An array containing all the Competency id's we want to show in this screen.
   */
  function getCompetencesForShow($profile)
  {
    $comps = atkGetNode('competency.competencyprofilecomp')->select("profile_id=$profile")->distinct(true)->includes('competency_id')->allRows();
    $comps2 = atkGetNode('competency.alwaysshow')->select("profile_id=$profile")->distinct(true)->includes('competency_id')->allRows();

    if($this->showExtra)
    {
       $extraComps = atkGetNode('competency.competencyextra')->select("profile_id=$profile")->distinct(true)->includes('competency_id')->allRows();
    }

    $comps = array_merge($comps,$comps2);

    if(is_array($extraComps) && count($extraComps) >0)
    {
      $comps = array_merge($comps, $extraComps);
    }

    $compsArray = array();
    //we make the array so we can use it later in a sql select
    foreach ($comps as $comp)
    {
      if(!empty($comp['competency_id']['id']))
        $compsArray[] = $comp['competency_id']['id'];
    }
    return $compsArray;
  }

  /**
   * Returns the competences in this profile
   *
   * @param int $profile
   * @return Array of competences in the profile
   */
  function getCompetencesInProfile($profile)
  {
    $db = atkGetDb();
    $query = $db->createQuery();    $query = $db->createQuery();
    //we add the table competences    //we add the table competences
    $query->addTable("competency_profile_comp");
    $query->addField("competency_id");
    $query->addField("niveau_id");
    $query->addCondition("profile_id=" . $profile);
    $competences = $db->getrows($query->buildSelect());

    $level = array();
    $comp = array();
    foreach ($competences as $competency)
    {
      $level[] = $competency["niveau_id"];
      $comp[] = $competency["competency_id"];
    }
    $output = array(
      "level" => $level ,
      "comp" => $comp);
    return $output;
  }

  /**
   * Returns code for the legenda
   *
   * @return string HTML teh html string that is used to show the legenda
   */
  function getLegenda()
  {
    $ui = $this->getUi();
    $params["modDir"] = moduleDir("competency");
    $output = $ui->render(moduleDir("competency") . "templates/legendaCompetency.tpl", $params, "competency");
    return $output;
  }

  function getComment()
  {
    $textAtr = new atkTextAttribute("comments");
    $output = "<div class=comment id=comment>";
    $output .= $textAtr->edit();
    $output .= "<input type=hidden id=comp>";
    $output .= "<br><input type=button name=test value=save onclick='sendComment();'>";
    $output .= "<input type=button name=test value=cancel onclick='closeComment();'>";
    $output .= "</div>";
    return $output;
  }

  /**
   * @todo see if this function is actualy used and what it does
   * @return void
   */
  function printTodoAction()
  {
    atk_var_dump('test');
  }

  /**
   * Adds a competency to a user
   * @return void
   */
  function addCompetencesAction()
  {
    $item = explode("!", $_GET['compId']);
    $table = $item[0];
    $prodid = $item[1];
    //we check if we got a competency id and if we got one we check in the database if the competency already exists for this user
    if (isset($_GET['compId']) && $this->checkDatabase($table, $prodid))
    {
      $this->addCompetency($table, $prodid);
    }
  }

  /**
   * Cheacks the database if a competency/niveau is already added to a user
   * @param  string $table in what table should we look
   * @param  int $prodid id of the competency/niveau
   * @return bool was the cometency already added
   */
  function checkDatabase($table, $prodid)
  {
    echo ($table . "-" . $prodid);
    $selectTable = "";
    $selectRow = "";
    switch ($table)
    {
      case "competency":
        $selectRow   = "competency_id";
        break;
      case "niveau":
        $selectRow   = "niveau_id";
        break;
      default:
        $selectRow   = "nee";
        break;
    }
    $db = &atkGetDb();
    //prepare the query for getting the data\
    $query = &$db->createQuery();
    //we add the table competences
    $query->addTable("competency_competences");
    $query->addCondition("person_id=" . $_GET['userId']);
    $query->addCondition($selectRow . "=" . $prodid);
    $query->addField("competency_id");
    $competences = $db->getrows($query->buildSelect());
    if (count($competences))
    {
      return false;
    }
    return true;
  }

  /**
   * Gets the data of the niveau we are working with
   *
   * @param integer $id id of the niveau in teh database
   * @return array with [id] and ["level"]
   */
  function getCompetencyNiveau($id)
  {
    $db = &atkGetDb();
    //prepare the query for getting the data\
    $query = &$db->createQuery();
    $query->addTable("competency_niveau");
    $query->addCondition("id=" . $id);
    $query->addField("competency_id");
    $query->addField("level");
    $competences = $db->getrows($query->buildSelect());
    return array(
      "id" => $competences[0]["competency_id"] ,
      "level" => $competences[0]["level"]);
  }

  /**
   * We add the competency to the database here
   * first we check if all levels that came before this competency
   * are added as well(if we add a level that is)
   *
   */
  function addCompetency($table, $prodid)
  {
    $db = atkGetDb();
    //before we do anything we will check if this user realy may delete items otherwise we will do nothing
    $node = atkGetNode('competency.competencymatch');
    $userId = $_GET['userId'];
    if ($this->acl->allowCompetencyChange($userId))
    {
      $niveau = "";
      $competency = "";
      $compNiveau = $this->getCompetencyNiveau($prodid);
      switch ($table)
      {
        case "competency":
          $niveau = - 1;
          $competency = $prodid;
          break;
        case "niveau":
          $niveau = $prodid;
          $competency = $compNiveau["id"];
          break;
        default:
          break;
      }
      //we check if we added a competency with a different date then today
      if ($this->date == "undefined")
      {
        $date = date("Y-m-d");
      }
      else
      {
        $date = $this->date;
      }
      $node = getNode('competency.competences');
      $node->getAttribute("person_id");
      $node->getAttribute("niveau_id");
      $node->getAttribute("added");
      $node->getAttribute("competency_id");
      if ($table == "niveau")
      {
        $query = $db->createQuery();
        $query->addTable("competency_niveau");
        $query->addCondition("level <" . $compNiveau["level"]);
        $query->addCondition("competency_id =" . $compNiveau["id"]);
        $query->addCondition("id NOT IN(SELECT niveau_id FROM competency_competences WHERE competency_id='" . $db->escapeSQL($competency) . "' AND person_id='" . $db->escapeSQL($userId) . "')");
        $query->addField("id");
        $niveaus = $db->getrows($query->buildselect());
        foreach ($niveaus as $niv)
        {
          $dataRec['person_id'] = $userId;
          $dataRec["competency_id"] = $competency;
          $dataRec["niveau_id"] = $niv[id];
          $dataRec["added"] = $date;
          $node->validate($dataRec, 'insert');
          $node->addDb($dataRec);
          atkGetDB()->commit();
        }
      }
      $dataRec['person_id'] = $userId;
      $dataRec["competency_id"] = $competency;
      $dataRec["niveau_id"] = $niveau;
      $dataRec["added"] = $date;
      $node->validate($dataRec, 'insert');
      $node->addDb($dataRec);
      atkGetDB()->commit();
    }
  }

  /**
   * delete a competency from a person
   *
   */
  function clearDataAction()
  {
    $db = &atkGetDb();
    //before we do anything we will check if this user realy may delete items otherwise we will do nothing
    $node = atkGetNode('competency.competencymatch');
    $userId = $_GET['userId'];
    if ($this->acl->allowCompetencyChange($userId))
    {
      $item = split("!", $_GET['compId']);
      $table = $item[0];
      $prodId = $item[1];
      $compNiveau = $this->getCompetencyNiveau($prodId);
      $selectRow = "";
      //we check what to delete
      switch ($table)
      {
        case "competency":
          $selectTable = "competency";
          $selectRow = "competency_id";
          break;
        case "niveau":
          $selectTable = "competency_niveau";
          $selectRow = "niveau_id";
          break;
        default:
          break;
      }
      if ($table == "niveau")
      {
        $query = &$db->createQuery();
        $query->addTable("competency_niveau");
        $query->addCondition("level >" . $compNiveau["level"]);
        $query->addCondition("competency_id =" . $compNiveau["id"]);
        $query->addCondition("id NOT IN(SELECT niveau_id FROM competency_competences WHERE competency_id='" . $db->escapeSQL($prodId) . "' AND person_id='" . $db->escapeSQL($userId) . "')");
        $query->addField("id");
        $niveaus = $db->getrows($query->buildselect());
        //we walk trough all the levels higher then the one we want to delete
        foreach ($niveaus as $niv)
        {
          $query = $db->createQuery();
          $query->addTable("competency_competences");
          $query->addCondition("person_id=" . $userId);
          $query->addCondition("niveau_id=" . $niv["id"]);
          $db->query($query->buildDelete());
        }
      }
      $query = $db->createQuery();
      $query->addTable("competency_competences");
      $query->addCondition("person_id=" . $userId);
      $query->addCondition($selectRow . " = " . $prodId);
      $db->query($query->buildDelete());
    }
  }

  /**
   * When we change of profile we want to save this change to the database
   *
   */
  function changeProfileAction()
  {
    $this->profile = $_GET['profile'];
    if (!$this->acl->allowSetProfile($this->user['id']))
    {
      return;
    }

    $db = atkGetDb();
    $query = $db->createQuery();
    $query->addTable("competency_profile_person");
    $query->addField("person_id");
    $query->addCondition("person_id=" . $this->user['id']);
    $profiles = $db->getrows($query->buildSelect());
    if (count($profiles) > 0)
    {
      $query = $db->createQuery();
      $query->addTable("competency_profile_person");
      $query->addField("profile_id", $this->profile);
      $query->addCondition("person_id =" . $this->user['id']);
      $db->query($query->buildUpdate());
    }
    else
    {
      $query = $db->createQuery();
      $query->addTable("competency_profile_person");
      $query->addField("profile_id", $this->profile);
      $query->addField("person_id", $this->user['id']);
      $db->query($query->buildInsert());
    }
  }

  /**
   * When the mouse is over a competence this function will be called to show extra information about that competence
   *
   */
  function mouseOverAction()
  {
    $item = explode("!", $_GET['compId']);
    $table = $item[0];
    $prodid = $item[1];
    $compNiveau = $this->getCompetencyNiveau($prodid);
    $params["table"] = $table;
    switch ($table)
    {
      case "competency":
        $niveau = 0;
        $competency = $prodid;
        break;
      case "niveau":
        $niveau = $prodid;
        $competency = $compNiveau["id"];
        break;
      default:
        break;
    }
    $db = &atkGetDb();
    $query = &$db->createQuery();
    if ($table == "competency")
    {
      $query->addTable("competency");
      $query->addJoin("competency_domain", "", "competency.domain_id=competency_domain.id");
      $query->addField("competency.name as competency");
      $query->addField("competency_domain.name as cDomain");
      $query->addCondition("competency.id =" . $competency);
      $competences = $db->getrows($query->buildSelect());
      $params["competences"] = $competences;
    }
    else
    {
      //if we show a level we need to show the information of this level
      $query->addTable("competency_niveau");
      $query->addJoin("competency", "", "competency_niveau.competency_id=competency.id");
      $query->addJoin("competency_domain", "", "competency.domain_id=competency_domain.id");
      $query->addField("competency_niveau.description as name");
      $query->addField("competency.name as competency");
      $query->addField("competency_domain.name as cDomain");
      $query->addField("level");
      $query->addCondition("competency_niveau.id =" . $niveau);
      $competences = $db->getrows($query->buildSelect());
      $params["competences"] = $competences;
    }
    // We can be sure that when we show this data that the competency is acquired
    //So for that we exectute a query to get that date and display it if the query is 0 we will show the planned dates
    $query3 = &$db->createQuery();
    $query3->addTable("competency_competences");
    $query3->addField("added");
    $query3->addCondition("competency_id=" . $competency);
    $query3->addCondition("niveau_id=" . $niveau);
    $query3->addCondition("person_id=" . $this->user['id']);
    $comps = $db->getrows($query3->buildSelect());
    $params["comps"] = $comps;
    if (count($comps) == 0)
    {
      //We want to check if this person had the competency planned
      //it is possible that the competency has been planned several times (in case the first date was not made)
      //so its possible we have to show differnt dates
      $query2 = &$db->createQuery();
      $query2->addTable("competency_planning");
      $query2->addField("plandate");
      $query2->addField("person_id");
      $query2->addCondition("niveau_id=" . $niveau);
      $query2->addCondition("person_id=" . $this->user['id']);
      $query2->addOrderBy("plandate desc");
      $schedules = $db->getrows($query2->buildSelect());
      $params["schedules"] = $schedules;
    }
    $params["lng"] = $this->lng;
    $ui = new atkUI();
    $html = $ui->render(moduleDir("competency") . "templates/mouseoverCompetency.tpl", $params, "competency");
    print($html);
  }

  /**
   * Adds a comment to a competency for a user
   * @return void
   */
  function sendCommentAction()
  {
    $comment = $_GET['comment'];
    $person_id = $_GET['userId'];
    $competency_id = $_GET['competency'];
    $db = &atkGetDb();
    $query = $db->createQuery();
    $query->addTable("competency_comments");
    $query->addCondition("competency_id =" . $competency_id);
    $query->addCondition("person_id =" . $person_id);
    $query->addField("comment");
    $comments = $db->getRows($query->buildSelect());
    $node = getNode('competency.competencycomments');
    $node->getAttribute("person_id");
    $node->getAttribute("competency_id");
    $node->getAttribute("comment");
    $dataRec = $node->select("person_id= " . $person_id . " AND competency_id=" . $competency_id)->firstRow();
    if (count($comments) > 0)
    {
      $dataRec["comment"] = $comment;
      $node->validate($dataRec, 'update');
      $node->updateDb($dataRec);
    }
    else
    {
      $dataRec['person_id'] = $person_id;
      $dataRec["competency_id"] = $competency_id;
      $dataRec["comment"] = $comment;
      $node->validate($dataRec, 'insert');
      $node->addDb($dataRec);
    }
    atkGetDB()->commit();
  }

  /**
   * loads all the data and html to show a todo
   * @return void
   */
  function getTodosAction()
  {
    $db = atkGetDb();

    $user = (int) $this->request->userId;
    empty($user)?$user=0:$user=$user;

    $query = $db->createQuery();
    $query->addTable("competency_planning");
    $query->addCondition("person_id=" . $user);
    $query->addCondition("competency_planning.niveau_id not in (Select niveau_id from competency_competences WHERE person_id=" . $user . ")");
    $query->addJoin("competency", "", "competency.id=competency_planning.competency_id", true);
    $query->addJoin("competency_niveau", "", "competency_niveau.id=competency_planning.niveau_id", true);
    $query->addJoin("competency_domain", "", "competency.domain_id=competency_domain.id", true);
    $query->addField("competency_domain.name as domName");
    $query->addField("planDate");
    $query->addField("remark");
    $query->addField("competency_niveau.name as nivName");
    $query->addField("competency.name as compName");
    $planDates = $db->getRows($query->buildSelect());
    $todos = array();
    //we make a pretty array of all we can use later in our template
    foreach ($planDates as $date)
    {
      $todos[$date["domName"]][$date["compName"]][$date["nivName"]][$date["planDate"]] = $date["remark"];
    }
    $query = $db->createQuery();
    $query->addTable("competency_comments");
    $query->addCondition("person_id=" . $user);
    $query->addJoin("competency", "", "competency.id=competency_comments.competency_id", true);
    $query->addJoin("competency_domain", "", "competency.domain_id=competency_domain.id", true);
    $query->addField("competency_domain.name as domName");
    $query->addField("comment");
    $query->addField("competency.name as compName");
    $comments = $db->getRows($query->buildSelect());
    foreach ($comments as $coment)
    {
      if (! isset($todos[$coment["domName"]][$coment["compName"]]))
      {
        $todos[$coment["domName"]][$coment["compName"]] = "";
      }
      $comments[$coment["domName"]][$coment["compName"]]["comp_comment"] = $coment["comment"];
    }
    $this->todos = $todos;
    $this->comments = $comments;
    $this->renderPartial('getTodo');
  }

  function getCommentValue()
  {
    $competency_id = $_GET['competency'];
    $person_id = $_GET['userId'];
    $db = &atkGetDb();
    $query = $db->createQuery();
    $query->addTable("competency_comments");
    $query->addCondition("competency_id =" . $competency_id);
    $query->addCondition("person_id =" . $person_id);
    $query->addField("comment");
    $comments = $db->getRows($query->buildSelect());
    print($comments[0]["comment"]);
    die();
  }

  //the functuin we use to delegate the update actions to the right functions
  //if we dont use this we will have to make an (empty) template for every action that
  //doesnt give feedback.
  function updateActionsAction()
  {
    if (isset($_GET['schedule']))
    {
      $this->schedule();
    }
    else if (isset($_GET['over']))
    {
      $this->mouseOverAction();
    }
    else if (isset($_GET['comment']))
    {
      print($_GET['comment']);
      $this->sendCommentAction();
    }
    else if (isset($_GET['type']))
    {
      if ($_GET['type'] == 'getcomment')
      {
        $this->getCommentValue();
      }
      else if ($_GET['type'] == 'gettodo')
      {
        $this->getTodosAction();
      }
      else
      {
        $this->changeProfileAction();
      }
    }
    else
    {
      if (isset($_GET['clearProduct']))
      {
        $this->clearDataAction();
      }
      else
      {
        $this->addCompetencesAction();
      }
    }
  }
}
?>
