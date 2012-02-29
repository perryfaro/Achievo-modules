<?php
atkimport('atk.front.atkfrontcontroller');

class profilecontroller extends atkFrontController
{

  var $user = "";

  var $competency = "";

  var $prodId = 0;

  var $lng;

  var $profile = 0;

    /**
     * Differnt actions we exectute trough ajax the get tells us what we want to do
     * update: update the screen
     * addExtra: Add an extra competence to the profile
     * removeExtra: removes a extra competence from the profile
     * addAlwaysshow: Makes a competency always show for this profile
     * removealwaysshow: removes the always show flag
     * clearproduct: removes a competency from the profile
     *
     * if none of these are given it wil try to add a competence.
     * @return void
     */
  function updateActionsAction()
  {
    if (isset($_GET['update']))
    {
      $this->update();
      //session_unregister('bin');
    }
    else if (isset($_GET['over']))
    {
      $this->over();
    }
    else if ($this->request->type == 'addExtra')
    {
      $this->addExtra();
    }
    else if ($this->request->type == 'removeExtra')
    {
      $this->removeExtra();
    }
    else if ($_GET['type'] == 'addAlwaysShow')
    {
      $this->addAlwaysShow();
    }
    else if ($_GET['type'] == 'removeAlwaysShow')
    {
      $this->removeAlwaysShow();
    }
    else
    {
      if (isset($_GET['clearProduct']))
      {
        $this->clearData();
      }
      else
      {
        $this->createCompetency();
      }
    }
  }

    /**
     * Makes a competence always show for a certain profile
     * @return void
     */
  private function addAlwaysShow()
  {
    if (isset($_GET['compId']) && $this->checkDatabaseAlways($_GET['compId'], $_GET['profile']))
    {
      $node = getNode('competency.alwaysshow');
      $node->getAttribute("profile_id");
      $node->getAttribute("competency_id");
      $dataRec["competency_id"] = $_GET['compId'];
      $dataRec["profile_id"] = $_GET['profile'];
      if($node->validate($dataRec, 'insert'))
      {
        $node->addDb($dataRec);
       $node->getDb()->commit();
      }
    }
  }

  /**
   * removes the alwaysshow flag from a competence for a certain profile
   * @return void
   */
  private function removeAlwaysShow()
  {
    $db = &atkGetDb();
    $node = getNode('competency.alwaysshow');
    $node->deleteDb('competency_id=' . $_GET['compId'] . ' AND profile_id=' . $_GET['profile']);
    atkGetDB()->commit();
  }

   /**
    * Adds a extra competence to the selected profile
    * @return void
    */
  private function addExtra()
  {
    if (isset($this->request->compId))
    {
      $node = getNode('competency.competencyextra');
      $node->getAttribute("profile_id");
      $node->getAttribute("competency_id");
      $dataRec["competency_id"] = $this->request->compId;
      $dataRec["profile_id"] = $this->request->profile;
      if($node->validate($dataRec, 'insert'))
      {
        $node->addDb($dataRec);
        $node->getDb()->commit();
      }
    }
  }

  /**
   * Removes an extra competence from the selected profile
   * @return void
  */
  private function removeExtra()
  {
    $db = &atkGetDb();
    $node = getNode('competency.competencyextra');
    $node->deleteDb('competency_id=' . $_GET['compId'] . ' AND profile_id=' . $_GET['profile']);
    atkGetDB()->commit();
  }


    /**
     * The prefilter takes care of setting some values and including javascripts
     * and stylesheets before we do an action.
     * @return void
     */
  public function prefilter()
  {
    $sessionManager = &atkGetSessionManager();
    if (isset($this->request->profile))
    {
      $this->profile = $this->request->profile;
    }
    $page = atkPage::getInstance();
    //we register the necessary scripts and style sheets
    //first we remove the old prototype and scriptaculous so we wont have any conflicts
    //we need this for ie it seems ie cant handle 2 the same scripts and then chose the good one of those.
    //then we insert the new prototype and scriptaculous before the prototype-ext.js
    $page->register_style(atkconfig("atkroot") . "atk/themes/default/styles/atkdateattribute.css");
    $page->register_script(moduleDir("competency") . "javascript/profile.js");
    $page->register_style(moduleDir("competency") . "styles/webProfile.css");
    $page->register_script(moduleDir("competency") . "javascript/proto.menu.0.6.js");
    $page->register_style(moduleDir("competency") . "styles/proto.menu.0.6.css");
    $page->register_scriptcode("deleteMessage ='" . atkText("are_you_sure_you_want_to_delete", "competency") . "';");
    $page->register_scriptcode("addMenu('" . atkText("competency_profile_match_add", "competency") . "','" . atkText("delete", "competency") . "...');");
    //We add some load script
    $page->register_loadscript("modDir = '" . moduleDir("competency") . "';");
    $page->register_loadscript("initLegenda();");
    $page->register_loadscript("toggleLegenda();");
    $page->register_loadscript("lng='" . $this->user["lng"] . "';");
    $profileNode = atkGetNode('competency.competency_profile_match');
    $page->register_loadscript("allowedAdd = " . (int)$profileNode->allowed('add') . ";");
	if($profileNode->allowed('add'))
    {
      $page->register_loadscript("initContextMenus();");
	}
    $page->register_loadscript("setObserv();");
    $page->register_loadscript("$('compover').toggle();");
    $page->register_style("style.css");
  }

  public function showAction()
  {
    $this->mythemedir = moduleDir('competency') . 'themes/default/';
    $this->fillProfiles();
    if($this->profile!=0)
      $this->loadCompetencesAction();
  }

  /**
   * Adds a competency to the database
   *
   */
  function createCompetency()
  {
    $item = split("!", $_GET['compId']);
    $table = $item[0];
    $prodid = $item[1];
    //we check if we got a competency id and if we got one we check in the database if the competency already exists for this user
    if (isset($_GET['compId']) && $this->checkDatabase($table, $prodid))
    {
      $this->addCompetency($table, $prodid);
    }
  }

  /**
   * We add the competency to the database here
   * first we check if all levels that came before this competency
   * are added as well(if we add a level that is)
   *
   */
  function addCompetency($table, $prodid)
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
    $db = &atkGetDb();
    $node = getNode('competency.competencyprofilecomp');
    $node->getAttribute("niveau_id");
    $node->getAttribute("profile_id");
    $node->getAttribute("competency_id");
    if ($table == "niveau")
    {
      $query = &$db->createQuery();
      $query->addTable("competency_niveau");
      $query->addCondition("level <" . $compNiveau["level"]);
      $query->addCondition("competency_id =" . $compNiveau["id"]);
      $query->addCondition("id NOT IN(SELECT niveau_id FROM competency_profile_comp WHERE competency_id =" . $competency . " AND profile_id=" . $this->profile . ")");
      $query->addField("id");
      $niveaus = $db->getrows($query->buildselect());
      foreach ($niveaus as $niv)
      {
        $dataRec["competency_id"] = $competency;
        $dataRec["niveau_id"] = $niv[id];
        $dataRec["profile_id"] = $this->profile;
        $node->validate($dataRec, 'insert');
        $node->addDb($dataRec);
        atkGetDB()->commit();
      }
    }
    $dataRec["competency_id"] = $competency;
    $dataRec["niveau_id"] = $niveau;
    $dataRec["profile_id"] = $this->profile;
    $node->validate($dataRec, 'insert');
    $node->addDb($dataRec);
    atkGetDB()->commit();
  }

  /**
   * Return the competency numbers that always have to be show for the current selected profile
   *
   * @return array with competency id's
   */
  private function getAlwaysShow()
  {
    $competences = atkGetNode('competency.alwaysshow')->select("profile_id=" . $this->profile)->includes(array('profile_id','competency_id'))->allRows();
    $comp = array();
    foreach ($competences as $competency)
    {
      $comp[] = $competency["competency_id"]['id'];
    }
    return $comp;
  }

  /**
   * Returns the competency numbers that are extre so those don't need to be show all the time but
   * do belong to the current profile
   *
   * @return array with competency id's
   */
  private function getExtra()
  {
    $competences = atkGetNode('competency.competencyextra')->select("profile_id=" . $this->profile)->includes(array('profile_id','competency_id'))->allRows();
    $comp = array();
    foreach ($competences as $competency)
    {
      $comp[] = $competency["competency_id"]['id'];
    }
    return $comp;
  }

  /**
   * We check if the competency is already in the profiles list
   *
   */
  private function checkDatabase($table, $prodid)
  {
    $selectTable = "";
    $selectRow = "";
    switch ($table)
    {
      case "competency":
        $selectRow = "competency_id";
        break;
      case "niveau":
        $selectRow = "niveau_id";
        break;
    }
    if (atkGetNode('competency.competencyprofilecomp')->select($selectRow.'='.$prodid . ' AND profile_id =' . $this->profile)->rowCount())
    {
      return false;
    }
    return true;
  }

  /**
   * We check if the competency is already in the AlwaysShowList
   *
   */
  function checkDatabaseAlways($competency, $profile)
  {
    $db = &atkGetDb();
    //prepare the query for getting the data\
    $query = &$db->createQuery();
    //we add the table competences
    $query->addTable("competency_show_for_profile");
    $query->addCondition("profile_id=" . $profile);
    $query->addCondition('competency_id =' . $competency);
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
   * @param Integer $id id of the niveau in the database
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
   * delete a competency from a person
   *
   */
  function clearData()
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
    $db = &atkGetDb();
    if ($table == "niveau")
    {
      $query = &$db->createQuery();
      $query->addTable("competency_niveau");
      $query->addCondition("level >" . $compNiveau["level"]);
      $query->addCondition("competency_id =" . $compNiveau["id"]);
      $query->addCondition("id NOT IN(SELECT niveau_id FROM competency_profile_comp WHERE competency_id =" . $prodId . " AND profile_id=" . $this->profile . ")");
      $query->addField("id");
      $niveaus = $db->getrows($query->buildselect());
      //we walk trough all the levels higher then the one we want to delete
      foreach ($niveaus as $niv)
      {
        $dQuery = $db->createQuery();
        $dQuery->addTable("competency_profile_comp");
        $dQuery->addCondition("profile_id=" . $this->profile);
        $dQuery->addCondition("niveau_id=" . $niv["id"]);
        $db->query($dQuery->buildDelete());
        //$db->query("Delete FROM competency_profile_comp WHERE profile_id = ".$this->profile." AND niveau_id = ".$niv["id"]);
      }
    }
    $dQuery = $db->createQuery();
    $dQuery->addTable("competency_profile_comp");
    $dQuery->addCondition("profile_id=" . $this->profile);
    $dQuery->addCondition($selectRow . "=" . $prodId);
    $db->query($dQuery->buildDelete());
    // $db->query("Delete FROM competency_profile_comp WHERE profile_id = ".$this->profile." AND ".$selectRow." = ".$prodId);
  }

  /**
   * This function gets all the data to needs and asks the template to give back html
   * After that it outputs the data to the screen
   *
   */
  function update()
  {
    $domain = $this->structurizeData();
    //the array we use to see if  a person already has a competence
    $competences = $this->getActiveCompetences();
    $output = $this->fillProfiles();
    $params["domain"] = $domain;
    $params["competences"] = $competences;
    $ui = new atkUI();
    $content .= $ui->render(moduleDir("competency") . "/templates/profilematch.tpl", $params);
    print $output . $content;
  }

  /**
   * We get all the data out of the database and format them in an array so we can later
   * use this array to create a neat structure of divs to display the competences in
   *
   * @return Array[domain][competence][niveau]=> id/level/name
   */
  function loadCompetencesAction()
  {
    if($this->profile==0)
      return;

    $profileNode = atkGetNode('competency.competency_profile_match');
	$this->allowAdd = false;
	if($profileNode->allowed('add'))
	{
	  $this->allowAdd = true;
    }
    $this->competences = $this->getActiveCompetences();
    $this->alwaysShow = $this->getAlwaysShow();
    $this->extra = $this->getExtra();
    $db = &atkGetDb();
    //prepare the query for getting the data\
    $query = &$db->createQuery();
    //we add the table competences
    $query->addTable("competency_niveau");
    $query->addJoin("competency", "", "competency.id=competency_niveau.competency_id", competency . competencygniveau);
    $query->addJoin("competency_domain", "", "competency_domain.id=competency.domain_id", competency . competencydomain);
    $query->addField("level");
    $query->addField("competency.id as id");
    $query->addField("competency_niveau.id as competencyid");
    $query->addField("competency_niveau.name as competency");
    $query->addField("competency.name as cgroup");
    $query->addField("competency_domain.name as domain");
    $query->addOrderBy("competency_domain.name");
    $query->addOrderBy("cgroup");
    $query->addOrderBy("level");
    $competences = $db->getrows($query->buildSelect());
    $domain = array();
    //we put the data in a handy array we can use to write away the divs
    foreach ($competences as $competence)
    {
      $domain[$competence["domain"]][$competence["cgroup"]]["id"] = $competence["id"];
      $domain[$competence["domain"]][$competence["cgroup"]][$competence["competency"]] = $competence["competencyid"];
    }
    //now we add all competences without a level
    $compQuery = &$db->createQuery();
    $compQuery->addTable("competency");
    $compQuery->addJoin("competency_domain", "", "competency_domain.id=competency.domain_id", competency . competencydomain);
    $compQuery->addField("competency.name as cgroup");
    $compQuery->addField("competency_domain.name as domain");
    $compQuery->addField("competency.id as id");
    $query->addOrderBy("competency_domain.name");
    $compQuery->addOrderBy("cgroup");
    $compQuery->addCondition("competency.id NOT IN (SELECT competency_id FROM competency_niveau)");
    $competences = $db->getrows($compQuery->buildSelect());
    //we add these competences to the same list
    foreach ($competences as $competence)
    {
      $domain[$competence["domain"]][$competence["cgroup"]]["id"] = $competence["id"];
    }
    $this->domain = $domain;
  }

  /**
   * Retuns the competences this person already has
   *
   * @return Array of competences
   */
  function getActiveCompetences()
  {
    $db = &atkGetDb();
    $query = &$db->createQuery();
    //we add the table competences
    $query->addTable("competency_profile_comp");
    $query->addField("niveau_id");
    $query->addField("competency_id");
    $query->addCondition("profile_id=" . $this->profile);
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
    return ($output);
  }

  /**
   * Retuns the competences this person already has
   *
   * @return Array of competences in the profile
   */
  function getCompetencesInProfile()
  {
    $db = &atkGetDb();
    $query = &$db->createQuery();
    //we add the table competences
    $query->addTable("competency_profile_comp");
    $query->addField("competency_id");
    $query->addCondition("profile_id=" . $this->profile);
    $competences = $db->getrows($query->buildSelect());
    $profile = array();
    foreach ($competences as $competency)
    {
      $profile[] = $competency["competency_id"];
    }
    return $profile;
  }

  /**
   * Fill the profiles select
   *
   * @return string HTML code for the profile select
   */
  function fillProfiles()
  {
    $profileNode = atkGetNode('competency.competencyprofile');
    $profiles = $profileNode->select()->orderBy('name');
    $select = atkText("_profile", "competency") . "<Select id=\"profileChoser\" name=\"profiles\" class=\"profileChoser\">";

    if ($this->profile != 0)
    {
      $this->profileId = $this->profile;
    }

    $this->listProfiles = $profiles;
  }

  /* When the mouse is over a competence this function will be called to show extra information about that competence
   *
   */
  function over()
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
      $query->addJoin("competency_domain", "", "competency.domain_id=competency_domain.id", competency . competency);
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
      $query->addJoin("competency", "", "competency_niveau.competency_id=competency.id", competency . competency);
      $query->addJoin("competency_domain", "", "competency.domain_id=competency_domain.id", competency . competency);
      $query->addField("competency_niveau.description as name");
      $query->addField("competency.name as competency");
      $query->addField("competency_domain.name as cDomain");
      $query->addField("level");
      $query->addCondition("competency_niveau.id =" . $niveau);
      $competences = $db->getrows($query->buildSelect());
      $params["competences"] = $competences;
    }
    $params["lng"] = $this->lng;
    $ui = new atkUI();
    $html = $ui->render(moduleDir("competency") . "templates/mouseoverProfile.tpl", $params, "competency");
    print($html);
  }
}
?>
