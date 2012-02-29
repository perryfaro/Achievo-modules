<?php
useattrib("atktextattribute");
useattrib("atknumberattribute");
useattrib("atkdateattribute");
useattrib("competency.levelattribute");

class competencyniveau extends atkNode
{
  function recordActions($rec, &$actions, &$mraactions)
  {

    if(atkGetNode('competency.competences')->select(atkGetNode('competency.competences')->getTable().'.niveau_id ='.$rec["id"])->rowCount() > 0)
    {
      unset($actions["delete"]);
    }
  }

  function preDelete($record)
  {
    atkGetNode('competency.competencyprofilecomp')->deleteDb( atkGetNode('competency.competencyprofilecomp')->getTable().'.niveau_id=' . $record['id']);
    atkGetNode('competency.competenceplanning')->deleteDb(atkGetNode('competency.competenceplanning')->getTable().'.niveau_id='. $record['id']);
  }

  function competencyniveau()
  {
    $sessionmanager = &atkGetSessionManager();

    if ($sessionmanager)
    {
      $comp = explode("=",$sessionmanager->pageVar("atkfilter"));
    }

    $this->atkNode("competencyniveau");
    $this->add(new atkAttribute("id", AF_PRIMARY|
    AF_HIDE|
    AF_AUTO_INCREMENT));
    $this->add(new atkAttribute("name",AF_OBLIGATORY));
    $this->add(new atkTextAttribute("description"));
    $id =isset($comp[1])?$comp[1]:0;

    $level = $this->add(new levelAttribute("level",$id,AF_LIST_NO_NULL_ITEM|AF_OBLIGATORY));
    //   $level->setLevels(1);

    if($sessionmanager && $sessionmanager->pageVar("atkaction")=="edit" && $sessionmanager->pageVar("atknodetype") == "competency.competencyniveau")
    {
      $lvl = explode("=",$sessionmanager->pageVar("atkselector"));
      $test = isset($lvl[1])?$lvl[1]:0;
      $level->changeLevelByLvlId($test);
    }
    $group = $this->add(new atkManyToOneRelation("competency_id","competency.competency",AF_HIDE));
    $group->addListColumn('domain_id');

    $this->setTable("competency_niveau");
    $this->setOrder("level");
    $group->addDependee("level");
  }


  function descriptor_def()
  {
    return "[name]";
  }

  /**
   * Deletes records from competences where the competency that is deleted is used
   * otherwise it will give complications in displaying who has what competences
   * And lowers levels if there are levels above the item that is deleted
   * @param Array $record
   * @return boolean
   */
  function postDelete($record)
  {
    $id = $record["id"];
    $level = $record["level"];
    $competencyid = $record["competency_id"]["id"];

    $db = atkGetDb();
    $db->query("Delete FROM competency_competences WHERE competency_id = ".$id);

    $query = $db->createQuery();
    $query->addTable("competency_niveau");
    $query->addField("level");
    $query->addField("id");
    $query->addCondition("level>".$level);
    $query->addCondition("competency_id=".$competencyid);
    $results= $db->getrows($query->buildSelect());
    foreach($results as $result)
    {
      $db->query("UPDATE competency_niveau SET level = ".($result["level"]-1) ." WHERE(level = ".$result["level"]." AND competency_id=".$competencyid . " AND id =". $result["id"] ." )");
    }

    //if we delete a level we also need to delete the entrie sin profiles and persons
    $query = $db->createQuery();
    $query->addTable("competency_competences");
    $query->addCondition("niveau_id=".$record["id"]);
    $db->query($query->buildDelete());
    $query = $db->createQuery();
    $query->addTable("competency_profile_comp");
    $query->addCondition("niveau_id=".$record["id"]);
    $db->query($query->buildDelete());

    return true;
  }

  /**
   * before we add anything we will look if we have added this infront of an other level
   *if thats the case we make all leves that come behind this one 1 level higher
   *
   * @param array $record
   * @return boolean
   */
  function preAdd($record)
  {
    $id = $record["id"];
    $level = $record["level"];
    $competencyid = $record["competency_id"]["id"];

    $db = atkGetDb();
    $query = $db->createQuery();
    $query->addTable("competency_niveau");
    $query->addField("level");
    $query->addField("id");
    $query->addCondition("level>=".$level);
    $query->addCondition("competency_id=".$competencyid);
    $results= $db->getrows($query->buildSelect());
    foreach($results as $result)
    {
      $db->query("UPDATE competency_niveau SET level = ".($result["level"]+1) ." WHERE(level = ".$result["level"]." AND competency_id=".$competencyid  . " AND id =". $result["id"] ." )");
    }
    return true;
  }

  /**
   * When we add a new competence infront of other there is the chance we have to update profiles and persons
   * since when they have the old level one they should also have the new one
   *
   * @param array $record
   */
  function postAdd($record)
  {
    //now we have to check all already added competences to see if this add doesn't has any effect on them for example when we add a new number one while the pervious nr one has
    //been acquired already we want to set this new level one as acquired as well because we don't want people to have a lvl 2 without lvl 1
    $db = atkGetDb();
    $query = $db->createQuery();
    $query->addTable("competency_competences");
    $query->addJoin("competency_niveau", "", "competency_niveau.id = competency_competences.niveau_id",true);
    $query->addField("niveau_id");
    $query->addField("competency_competences.competency_id");
    $query->addField("added");
    $query->addField("person_id");
    $lvl = $record["level"]+1;
    $query->addCondition("level=".$lvl);
    $query->addCondition("competency_competences.competency_id=".$record["competency_id"]["id"]);


    $nivs = $db->getrows($query->buildSelect());


    //now for all records we we find we set the new level as acquired as well
    foreach($nivs as $niv)
    {
      $insertQ = $db->createQuery();
      $insertQ->addTable("competency_competences");
      $insertQ->addField("competency_id",$record["competency_id"]["id"]);
      $insertQ->addField("niveau_id",$record["id"]);
      $insertQ->addField("person_id",$niv["person_id"]);
      $insertQ->addField("added",$niv["added"]);
      $db->query($insertQ->buildInsert());
    }

    //we do the same for profile

    $query = $db->createQuery();
    $query->addTable("competency_profile_comp");
    $query->addJoin("competency_niveau", "", "competency_niveau.id = competency_profile_comp.niveau_id",true);
    $query->addField("niveau_id");
    $query->addField("competency_profile_comp.competency_id");
    $query->addField("profile_id");
    $lvl = $record["level"]+1;
    $query->addCondition("level=".$lvl);
    $query->addCondition("competency_profile_comp.competency_id=".$record["competency_id"]["id"]);


    $nivs = $db->getrows($query->buildSelect());


    //now for all records we we find we set the new level as acquired as well
    foreach($nivs as $niv)
    {
      $insertQ = $db->createQuery();
      $insertQ->addTable("competency_profile_comp");
      $insertQ->addField("competency_id",$record["competency_id"]["id"]);
      $insertQ->addField("niveau_id",$record["id"]);
      $insertQ->addField("profile_id",$niv["profile_id"]);
      $db->query($insertQ->buildInsert());
    }
  }

  function preUpdate($record)
  {

    $id = $record["id"];
    $level = $record["level"];
    $competencyid = $record["competency_id"]["id"];


    $db = atkGetDb();
    $query = $db->createQuery();
    $query->addTable("competency_niveau");
    $query->addField("level");
    $query->addCondition("id=".$record["id"]);
    $results= $db->getrows($query->buildSelect());


    //now we select the levels we want to edit
    $db = atkGetDb();
    $query = $db->createQuery();
    $query->addTable("competency_niveau");
    $query->addField("level");
    $query->addField("id");
    $query->addCondition("competency_id=".$competencyid);

    if ($record["level"] > $results[0]["level"])
    {
      $query->addCondition($query->betweenCondition("level",$results[0]["level"]+1,($record["level"]),false));
      $levels= $db->getrows($query->buildSelect());
      foreach($levels as $level)
      {
        $db->query("UPDATE competency_niveau SET level = ".($level["level"]-1) ." WHERE(level = ".$level["level"]." AND competency_id=".$competencyid  . " AND id =". $level["id"] ." )");
      }
    }
    else
    {
      $query->addCondition($query->betweenCondition("level",$record["level"],($results[0]["level"]-1)));
      $levels= $db->getrows($query->buildSelect());
      foreach($levels as $level)
      {
        $db->query("UPDATE competency_niveau SET level = ".($level["level"]+1) ." WHERE(level = ".$level["level"]." AND competency_id=".$competencyid  . " AND id =". $level["id"] ." )");
      }
    }
  }
}
?>