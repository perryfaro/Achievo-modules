<?php
useattrib("atktextattribute");
useattrib("atknumberattribute");
useattrib("atkdateattribute");
class competency extends atkNode
{

  function competency ()
  {
    $this->atkNode("competency",NF_EDITAFTERADD);
    $this->add(new atkAttribute("id", AF_PRIMARY | AF_HIDE | AF_AUTO_INCREMENT | AF_FORCE_LOAD));
    $this->add(new atkAttribute("name", AF_OBLIGATORY | AF_SEARCHABLE));
    $this->add(new atkTextAttribute("description"));
    $this->add(new atkManyToOneRelation("domain_id", "competency.competencydomain", AF_OBLIGATORY | AF_SEARCHABLE));
    $this->add(new atkOneToManyRelation("competencyniveau", "competency.competencyniveau", "competency_id",AF_CASCADE_DELETE));
    $this->setTable("competency");
  }

  function descriptor_def ()
  {
    return "[name]";
  }

  function recordActions($rec, &$actions, &$mraactions)
  {

    if(atkGetNode('competency.competences')->select(atkGetNode('competency.competences')->getTable().'.competency_id='.$rec["id"])->rowCount() > 0)
    {
      unset($actions["delete"]);
    }
  }

  /**
   * Makes sure that when our competency is deleted it will remove all connections in the other tables as well
   *
   * @param array $record the record to remove from the database
   */
  function preDelete($record)
  {
    atkGetNode('competency.competencyextra')->deleteDb(atkGetNode('competency.competencyextra')->getTable().'.competency_id=' . $record['id']);
    atkGetNode('competency.competencycomments')->deleteDb(atkGetNode('competency.competencycomments')->getTable().'.competency_id=' . $record['id']);
    atkGetNode('competency.competencyprofilecomp')->deleteDb( atkGetNode('competency.competencyprofilecomp')->getTable().'.competency_id=' . $record['id']);
    atkGetNode('competency.competenceplanning')->deleteDb(atkGetNode('competency.competenceplanning')->getTable().'.competency_id='. $record['id']);
    atkGetNode('competency.alwaysshow')->deleteDb(atkGetNode('competency.alwaysshow')->getTable().'.competency_id='. $record['id']);
  }

  /**
   * Generates the data we need to  make a radar graph of competencty holes
   * This method generates the data on domain niveau
   *
   * @param array params This array needs to contain a value for ["profile"]
   * @param boolean $raw
   * @return array with data for the graph
   */
  function graph_competencyDomainHoles ($params, $raw = false)
  {
    $db = &atkGetDb();
    // first we select all persons that have this profile
    //prepare the query for getting the data\
    $query = &$db->createQuery();
    //we add the table competences
    $query->addTable("person");
    $query->addField("id");
    $query->addCondition("id IN (SELECT person_id FROM competency_profile_person where profile_id =" . $params["profile"] . ")");
    $persons = $db->getrows($query->buildSelect());
    $personsCount = count($persons);
    $levelQuery = &$db->createQuery();

    $levelQuery->addTable("competency_profile_person");
    $levelQuery->addJoin("person", "", "person.id = competency_profile_person.person_id", true);
    $levelQuery->addJoin("competency_competences", "", "person.id = competency_competences.person_id", true);
    $levelQuery->addJoin("competency_niveau", "", "competency_niveau.id = competency_competences.niveau_id", true);
    $levelQuery->addJoin("competency", "", "competency_competences.competency_id = competency.id", true);
    $levelQuery->addJoin("competency_domain", "", "competency_domain.id = competency.domain_id", true);
    $levelQuery->addField("competency_competences.competency_id as competency");
    $levelQuery->addField("max(competency_niveau.level) as level");
    $levelQuery->addField("competency_domain.name as dName");
    $levelQuery->addField("competency_domain.id as dId");
    $levelQuery->addCondition("competency_profile_person.profile_id =" . $params["profile"]);
    $levelQuery->addGroupBy("person.id");
    $levelQuery->addGroupBy("competency_competences.competency_id");

    $levels = $db->getrows($levelQuery->buildSelect());

    $counts = array();
    $csims = array();
    foreach ($levels as $comp)
    {
      //data for the urls
      if (isset($counts[$comp["dName"]]))
      {
        $counts[$comp["dName"]] += isset($comp["level"]) ? $comp["level"] : 1;
      } else
      {
        $counts[$comp["dName"]] = isset($comp["level"]) ? $comp["level"] : 1;
      }
      $csims[$comp["dName"]] = "javascript:top.frames['main'].changeUrl(" . $comp["dId"] . "," . $params["profile"] . ")";
    }

    $csimArray = array();
    foreach ($csims as $sim)
    {
      $csimArray[] = $sim;
    }

    $lQuery = &$db->createQuery();
    $lQuery->addTable("competency_profile_comp");
    $lQuery->addJoin("competency_niveau", "", "competency_niveau.id = competency_profile_comp.niveau_id", true);
    $lQuery->addJoin("competency", "", "competency_profile_comp.competency_id = competency.id", true);
    $lQuery->addJoin("competency_domain", "", "competency_domain.id = competency.domain_id", true);
    $lQuery->addField("max(level) as level");
    $lQuery->addField("competency_profile_comp.competency_id as competency");
    $lQuery->addField("competency_domain.name as dName");
    $lQuery->addField("competency_domain.id as dId");
    $lQuery->addCondition("competency_profile_comp.profile_id =" . $params["profile"]);
    $lQuery->addGroupBy("competency_profile_comp.competency_id");

    $leve = $db->getrows($lQuery->buildSelect());
    $coun = array();
    foreach ($leve as $com)
    {
      if (isset($coun[$com["dName"]]))
      {
        $coun[$com["dName"]] += isset($com["level"]) ? $com["level"] : 1;
      } else
      {
        $coun[$com["dName"]] = isset($com["level"]) ? $com["level"] : 1;
      }
    }

    $dataset = array();
    foreach ($coun as $itemKey => $item)
    {
    $average = (int)$item * $personsCount;
    if($average > 0)
    {
        $percentage = (100 / $average)  * $counts[$itemKey];
      }
    else
      {
      $percentage = 0;
      }
    $dataset[$itemKey] = $percentage;
    }

    if ($raw)
    {
      return($dataset);
    }
    //we check how many graphs will be generated and how much data is in the last of  them. if we have less then 3 points we will
    //take points from the second last and add them to the last so its till a good graph

    return array("name" => "competency voids" , "data" => array_slice($dataset,$params["offset"],$params["reach"],true) , "CSIM" => $csimArray );
  }

  /**
   * creates the graph for competency holes
   *
   * @param array $params this array needs to contain values for ["profile"] and ["domain"]
   * @param boolean $raw
   * @return array
   */
  function graph_competencyHoles ($params, $raw = false)
  {
    $domain = $params["domain"];
    $db = &atkGetDb();

    // first we select all persons that have this profile
    //prepare the query for getting the data\
    $query = &$db->createQuery();
    //we add the table competences
    $query->addTable("person");
    $query->addField("id");
    $query->addCondition("id IN (SELECT person_id FROM competency_profile_person where profile_id =" . $params["profile"] . ")");
    $persons = $db->getrows($query->buildSelect());

    $personsCount = count($persons);
    $levelQuery = &$db->createQuery();

    $levelQuery->addTable("competency_profile_person");
    $levelQuery->addJoin("person", "", "person.id = competency_profile_person.person_id", true);
    $levelQuery->addJoin("competency_competences", "", "person.id = competency_competences.person_id", true);
    $levelQuery->addJoin("competency_niveau", "", "competency_niveau.id = competency_competences.niveau_id", true);
    $levelQuery->addJoin("competency", "", "competency_competences.competency_id = competency.id", true);
    $levelQuery->addJoin("competency_domain", "", "competency_domain.id = competency.domain_id", true);
    $levelQuery->addField("competency_competences.competency_id as competency");
    $levelQuery->addField("max(competency_niveau.level) as level");
    $levelQuery->addField("competency.name as cName");
    $levelQuery->addCondition("competency_profile_person.profile_id =" . $params["profile"]);
    $levelQuery->addCondition("competency_domain.id = " . $domain);
    $levelQuery->addGroupBy("person.id");
    $levelQuery->addGroupBy("competency_competences.competency_id");
    $levels = $db->getrows($levelQuery->buildSelect());

    $counts = array();
    foreach ($levels as $comp)
    {
      if (isset($counts[$comp["cName"]]))
      {
        $counts[$comp["cName"]] += isset($comp["level"]) ? $comp["level"] : 1;
      } else
      {
        $counts[$comp["cName"]] = isset($comp["level"]) ? $comp["level"] : 1;
      }
    }

    // then we select the competences we need
    $lQuery = &$db->createQuery();
    $lQuery->addTable("competency_profile_comp");
    $lQuery->addJoin("competency_niveau", "", "competency_niveau.id = competency_profile_comp.niveau_id", true);
    $lQuery->addJoin("competency", "", "competency_profile_comp.competency_id = competency.id", true);
    $lQuery->addJoin("competency_domain", "", "competency_domain.id = competency.domain_id", true);
    $lQuery->addField("max(level) as level");
    $lQuery->addField("competency_profile_comp.competency_id as competency");
    $lQuery->addField("competency.name as cName");
    $lQuery->addCondition("competency_profile_comp.profile_id =" . $params["profile"]);
    $lQuery->addCondition("competency_domain.id = " . $domain);
    $lQuery->addGroupBy("competency_profile_comp.competency_id");

    $leve = $db->getrows($lQuery->buildSelect());

    $coun = array();
    foreach ($leve as $com)
    {
      if (isset($coun[$com["cName"]]))
      {
        $coun[$com["cName"]] += isset($com["level"]) ? $com["level"] : 1;
      } else
      {
        $coun[$com["cName"]] = isset($com["level"]) ? $com["level"] : 1;
      }
    }

    $dataset = array();

    foreach ($coun as $itemKey => $item)
    {
      $percentage = (100 / ($item * $personsCount)) * $counts[$itemKey];
      $dataset[$itemKey] = $percentage;
    }
    if ($raw)
    {
      return array("profile" => $levels , "competences" => $leve);
    }
    return array("name" => "competency voids" , "data" => $dataset);
  }

  /**
   * Generates the data to creat the graph for competency growth
   * This function generate steh data for 1 person
   *
   * @param array $params need sto contain values for ["startdate"],["enddate"],["domain"] and ["person"]
   * @param boolean $raw
   * @return array
   */
  function graph_growth ($params, $raw = false)
  {
    $startdate = $params["startdate"];
    $enddate = $params["enddate"];
    $domain = $params["domain"];
    $person = $params["person"];

    $profileData = $this->_getProfile($person, $domain);
    $startLevels = $this->_getStartLevelsPerson($person, $domain, $startdate);
    $endLevels = $this->_getEndLevelsPerson($person, $domain, $enddate);

    $dataset = array();
    foreach ($profileData as $dataKey => $data)
    {
      foreach ($data as $itemKey => $item)
      {
        //first we calculate the should have then we look if its - number if this is the case we know that the
        //person had more competences then he needed so we make it positive and put it in has more then we make shoudlhave 0
        //and we lessen normal with tha shouldhave
        $normal = $startLevels[$dataKey][$itemKey];
        $shouldhave = $item - $startLevels[$dataKey][$itemKey];
        if ($shouldhave < 0)
        {
          $hasMore = abs($shouldhave);
          $normal += $shouldhave;
          $shouldhave = 0;
        } else
        {
          $hasMoreE = 0;
        }
        //first we calculate the should have then we look if its - number if this is the case we know that the
        //person had more competences then he needed so we make it positive and put it in has more then we make shoudlhave 0
        $normalE = $endLevels[$dataKey][$itemKey];
        $shouldhaveE = ($item - $endLevels[$dataKey][$itemKey]);
        if ($shouldhaveE < 0)
        {
          $hasMoreE = abs($shouldhaveE);
          $normalE += $shouldhaveE;
          $shouldhaveE = 0;
        } else
        {
          $hasMoreE = 0;
        }

        //we take the end date because this one will neve have less then begindate

        unset($startLevels[$dataKey][$itemKey]);
        unset($endLevels[$dataKey][$itemKey]);

        //before we send teh date we will format the dates so they are in dtuch format
        $tempStart = strtotime($startdate);
        $startdate = date("d-m-Y",$tempStart);
        $tempEnd = strtotime($enddate);
        $enddate = date("d-m-Y",$tempEnd);

        $dataset[$itemKey] = array($startdate => array("normal" => $normal , "shouldhave" => $shouldhave , "hasmore" => $hasMore) , $enddate => array("normal" => $normalE , "shouldhave" => $shouldhaveE , "hasmore" => $hasMoreE));
      }
    }
    // we add the competences that arent in a profile but the person acquired them anyway
    foreach($endLevels as $edDkey=>$edD)
    {
      foreach($edD as $edCkey=>$edC)
      {
        $hasMoreE = ($edC);
        $dataset[$edCkey] = array($startdate => array("normal" => 0 , "shouldhave" => 0 , "hasmore" => isset($startLevels[$edDkey][$edCkey])!=0?($startLevels[$edDkey][$edCkey]):0) , $enddate => array("normal" => 0 , "shouldhave" => 0 , "hasmore" => $hasMoreE));
      }
    }
    return $dataset;
  }

  /**
   * This function generates the data we need for the creation of the growth graph for domain and/or team
   *
   * @param array $params needs to contain values for ["startdate"],["enddate"],["domain"] and ["team"] if ["team"] is 0 the data wil be generated for teh whoel company else for teh given team
   * @param boolean $raw
   * @return array
   */
  function graph_growthTeam ($params, $raw = false)
  {
    $domain = $params["domain"];
    $team = isset($params["team"]) ? $params["team"] : 0;
    $startdate = $params["startdate"];
    $enddate = $params["enddate"];
    $profile = $params["profile"];

    $profiles = $this->_getProfiles($domain, $team,$profile);
    $startLevels = $this->_getLevelsByDate($domain, $startdate, $team,$profile);
    $endLevels = $this->_getLevelsByDate($domain, $enddate, $team,$profile);
    $dataset = array();

    foreach ($profiles as $dataKey => $data)
    {
      foreach ($data as $itemKey => $item)
      {
        //first we calculate the should have then we look if its - number if this is the case we know that the
        //person had more competences then he needed so we make it positive and put it in has more then we make shoudlhave 0
        //and we lessen normal with tha shouldhave

        //because we can have diffent peopel in as well the profile as the competences we merge the arrays with people in them
        //because we use p + users nr as key we eliminate all double people so we can have a distinct count of how many have the competency
        // plus how many should have to (even if they dont have it yet)
        $join = array_merge($startLevels[$dataKey][$itemKey]["persons"],$item["persons"]);
        $nrOfPersons = count($join);
        $normal = $startLevels[$dataKey][$itemKey]["mx"] /  $nrOfPersons;
        $shouldhave = ($item["mx"] / $item["pCount"]) - ($startLevels[$dataKey][$itemKey]["mx"] /  $nrOfPersons);
        if ($shouldhave < 0)
        {
          $hasMore = abs($shouldhave);
          $normal += $shouldhave;
          $shouldhave = 0;
        } else
        {
          $hasMore = 0;
        }

        //first we calculate the should have then we look if its - number if this is the case we know that the
        //person had more competences then he needed so we make it positive and put it in has more then we make shoudlhave 0
        $joinE = array_merge($endLevels[$dataKey][$itemKey]["persons"], $item["persons"]);
        $nrOfPersonsE = count($joinE);


        $normalE = $endLevels[$dataKey][$itemKey]["mx"] / $nrOfPersonsE;
        $shouldhaveE = ($item["mx"] / $item["pCount"]) - ($endLevels[$dataKey][$itemKey]["mx"] / $nrOfPersonsE);
        if ($shouldhaveE < 0)
        {
          $hasMoreE = abs($shouldhaveE);
          $normalE += $shouldhaveE;
          $shouldhaveE = 0;
        } else
        {
          $hasMoreE = 0;
        }
        unset($startLevels[$dataKey][$itemKey]);
        unset($endLevels[$dataKey][$itemKey]);

        //before we send the date we will format the dates so they are in dutch format
        $tempStart = strtotime($startdate);
        $startdate = date("d-m-Y",$tempStart);
        $tempEnd = strtotime($enddate);
        $enddate = date("d-m-Y",$tempEnd);
        $dataset[$itemKey] = array($startdate => array("normal" => $normal , "shouldhave" => $shouldhave , "hasmore" => $hasMore) , $enddate => array("normal" => $normalE , "shouldhave" => $shouldhaveE , "hasmore" => $hasMoreE));
      }
    }
    //we can have competences that are not in a profile we will add then now.
    //because they are not in a profile we know that its not required so the required level is 0

    //we take the end date because this one will never have less then begindate
    foreach($endLevels as $edDkey=>$edD)
    {
      foreach($edD as $edCkey=>$edC)
      {
        $hasmore = (isset($startLevels[$edDkey][$edCkey]["mx"])!=0?($startLevels[$edDkey][$edCkey]["mx"]):0)/count($startLevels[$edDkey][$edCkey]["persons"]);
        $hasMoreE = ($edC["mx"]/count($edC["persons"]));
        $dataset[$edCkey] = array($startdate => array("normal" => 0 , "shouldhave" => 0 , "hasmore" => $hasmore ), $enddate => array("normal" => 0 , "shouldhave" => 0 , "hasmore" => $hasMoreE));
      }
    }
    return $dataset;
  }

  /**
   * gets the profile of the person specified for the specified domain
   *
   * @param int $person id of the person we want the profile for
   * @param int $domain if of the domain we want the profile for
   * @return array
   */
  private function _getProfile ($person, $domain)
  {
    $db = &atkGetDb();
    $query = &$db->createQuery();
    $query->addTable("competency_profile_person");
    $query->addJoin("competency_profile_comp", "", "competency_profile_comp.profile_id = competency_profile_person.profile_id", true);
    $query->addJoin("competency", "", "competency.id = competency_profile_comp.competency_id", true);
    $query->addJoin("competency_niveau", "", "competency_profile_comp.niveau_id = competency_niveau.id", true);
    $query->addJoin("competency_domain", "", " competency.domain_id = competency_domain.id");
    $query->addField("max(level) as mx");
    $query->addField("competency_profile_comp.competency_id");
    $query->addField("competency_domain.name as dName");
    $query->addField("competency.name as cName");

    $query->addGroupBy("competency_profile_comp.competency_id");

    $query->addCondition("person_id=" . $person);
    $query->addCondition("competency_domain.id =" . $domain);
    $counts = $db->getrows($query->buildSelect());

    $dataset = array();
    foreach ($counts as $count)
    {
      $dataset[$count["dName"]][$count["cName"]] = $count["mx"] > 0 ? $count["mx"] : 1;
    }
    return $dataset;
  }

  /**
   * This function gets all the profiles of the people in a team (or a whole company)
   * or gets the data for the profile specified
   *
   * @param int $team when 0 it wont look for a team but for the whole company
   * @param int $domain id of the domain we want to search on
   */
  private function _getProfiles ($domain, $team = 0,$profile = 0)
  {
    $db = &atkGetDb();

    $query = &$db->createQuery();
    $query->addTable("competency_profile_person");
    $query->addJoin("competency_profile_comp", "", "competency_profile_comp.profile_id = competency_profile_person.profile_id", true);
    $query->addJoin("competency", "", "competency.id = competency_profile_comp.competency_id", true);
    $query->addJoin("competency_niveau", "", "competency_profile_comp.niveau_id = competency_niveau.id", true);
    $query->addJoin("competency_domain", "", " competency.domain_id = competency_domain.id");
    $query->addField("max(level) as mx");
    $query->addField("count(distinct(competency_profile_person.person_id)) as pCount");
    $query->addField("competency_profile_comp.competency_id");
    $query->addField("competency_domain.name as dName");
    $query->addField("competency.name as cName");
    $query->addField("competency_profile_person.person_id as person");
    $query->addGroupBy("competency_profile_comp.profile_id");
    $query->addGroupBy("competency_profile_comp.competency_id");
    $query->addGroupBy("person_id");

    if ($team != 0)
    {
      $query->addCondition("person_id=" . $team . " OR person_id IN (SELECT id from person WHERE supervisor =" . $team . ")");
    }

    if ($profile !=0)
    {
      $query->addCondition("competency_profile_comp.profile_id ="." $profile");
    }

    $query->addCondition("competency_domain.id =" . $domain);


    $counts = $db->getrows($query->buildSelect());

    $dataset = array();
    foreach ($counts as $count)
    {
      $dataset[$count["dName"]][$count["cName"]]["mx"] += $count["mx"] > 0 ? ($count["mx"] * $count["pCount"]) : $count["pCount"];
      $dataset[$count["dName"]][$count["cName"]]["pCount"] += $count["pCount"] > 0 ? $count["pCount"] : 1;
      $dataset[$count["dName"]][$count["cName"]]["persons"]["p" . $count["person"]] = $count["person"];
    }
    return $dataset;

  }

  private function _getStartLevelsPerson ($person, $domain, $startdate)
  {
    $db = &atkGetDb();
    $query = &$db->createQuery();
    $query->addTable("competency_competences");
    $query->addJoin("competency", "", "competency.id = competency_competences.competency_id", true);
    $query->addJoin("competency_niveau", "", "competency_competences.niveau_id = competency_niveau.id", true);
    $query->addJoin("competency_domain", "", " competency.domain_id = competency_domain.id", true);
    $query->addCondition("person_id=" . $person);
    $query->addField("max(level) as mx");
    $query->addField("competency_domain.name as dName");
    $query->addField("competency.name as cName");
    $query->addCondition("competency_domain.id =" . $domain);
    $query->addCondition("competency_competences.added <= '" . $startdate . "'");
    $query->addGroupBy("competency_competences.competency_id");

    $counts = $db->getrows($query->buildSelect());

    foreach ($counts as $count)
    {
      $dataset[$count["dName"]][$count["cName"]] = $count["mx"] > 0 ? $count["mx"] : 1;
    }
    return $dataset;
  }

  private function _getEndLevelsPerson ($person, $domain, $enddate)
  {
    $db = &atkGetDb();
    $query = &$db->createQuery();
    $query->addTable("competency_competences");
    $query->addJoin("competency", "", "competency.id = competency_competences.competency_id", true);
    $query->addJoin("competency_niveau", "", "competency_competences.niveau_id = competency_niveau.id", true);
    $query->addJoin("competency_domain", "", " competency.domain_id = competency_domain.id", true);
    $query->addCondition("person_id=" . $person);
    $query->addField("max(level) as mx");
    $query->addField("competency_domain.name as dName");
    $query->addField("competency.name as cName");
    $query->addCondition("competency_domain.id =" . $domain);
    $query->addCondition("competency_competences.added <= '" . $enddate . "'");
    $query->addGroupBy("competency_competences.competency_id");

    $counts = $db->getrows($query->buildSelect());

    foreach ($counts as $count)
    {
      $dataset[$count["dName"]][$count["cName"]] = $count["mx"] > 0 ? $count["mx"] : 1;
    }
    return $dataset;
  }

  private function _getLevelsByDate ($domain, $date, $team = 0,$profile =0)
  {
    $db = &atkGetDb();
    $query = &$db->createQuery();
    $query->addTable("competency_competences");
    $query->addJoin("competency", "", "competency.id = competency_competences.competency_id", true);
    $query->addJoin("competency_niveau", "", "competency_competences.niveau_id = competency_niveau.id", true);
    $query->addJoin("competency_domain", "", " competency.domain_id = competency_domain.id");
    $query->addField("max(level) as mx");
    $query->addField("count(distinct(competency_competences.person_id)) as pCount");
    $query->addField("competency_domain.name as dName");
    $query->addField("competency.name as cName");
    $query->addField("competency_competences.person_id as person");
    $query->addGroupBy("competency_competences.competency_id");
    $query->addGroupBY("competency_competences.person_id");
    $query->addCondition("added <= '" . $date . "'");

    if ($team != 0)
    {
      $query->addCondition("person_id=" . $team . " OR person_id IN (SELECT id from person WHERE supervisor =" . $team . ")");
    }

    if ($profile != 0)
    {
      $query->addCondition("person_id IN (SELECT person_id FROM competency_profile_person WHERE profile_id =".$profile . ")");
    }
    $query->addCondition("competency_domain.id =" . $domain);
    $counts = $db->getrows($query->buildSelect());

    $dataset = array();
    foreach ($counts as $count)
    {
      $dataset[$count["dName"]][$count["cName"]]["mx"] += $count["mx"] > 0 ? ($count["mx"] * $count["pCount"]) : $count["pCount"];
      $dataset[$count["dName"]][$count["cName"]]["pCount"] += $count["pCount"] > 0 ? $count["pCount"] : 1;
      $dataset[$count["dName"]][$count["cName"]]["persons"]["p" . $count["person"]] = $count["person"];
    }
    return $dataset;
  }

}
?>
