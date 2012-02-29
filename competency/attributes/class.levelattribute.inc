<?php

useattrib('atklistattribute');
/**
 * An attribute that makes sure you don't have double levels
 * When you add a level that already exists it will make the rest 1 bigger
 * when you delete one it will close the gap between the levels
 *
 */
class levelAttribute extends atkListAttribute
{
  function levelAttribute($name,$id,$flags=0)
  {
    $values = $this->initLevel($id);
    $this->atkListAttribute($name, $values, $values, $flags);
  }

  /**
   * initializes the level attribute
   *
   * @param int $id id of the competency you want to select levels for
   * @return array with values
   */
  function initLevel($id)
  {
    $db = atkGetDb();
    // we check if the database exists because otherwise we can get trouble with the installer script
    if($db->tableExists("competency_niveau"))
    {
      $query = $db->createQuery();
      $query->addTable("competency_niveau");
      $query->addField("competency_id");
      $query->addField("max(level) as max");
      $query->addGroupBy("level");
      $query->addCondition("competency_id=".$id);
      $result = $db->getrows($query->buildSelect());

      if ($id > 0 || $id > "")
      {
        if($result[0]["max"] != null){
          $plus = $result[0]["max"];
        }else{
          $plus=1;
        }
      }
      else
      {
        $plus=99;
      }
      for($i =0;$i<count($result)+$plus;$i++)
      {
        $values[] = $i+1;
      }
    }else
    {
      $values[] = 1;
    }
    return $values;
  }

  function setLevels($levels)
  {
    for($i=1;i<=$levels;$i++)
    {
      $values[]=$i;
    }
    $this->setOptions($values,$values);
  }

  /**
   * Selects the level you want
   *
   * @param int $id the id of the level you want to select
   * @return array with values
   */
  function selectLevel($id)
  {
    $db = atkGetDb();
    if($db->tableExists("competency_niveau"))
    {
      $query = $db->createQuery();
      $query->addTable("competency_niveau");
      $query->addField("competency_id");
      $query->addField("max(level) as max");
      $query->addGroupBy("level");
      $query->addCondition("competency_id in (select competency_id from competency_niveau where id = ".$id . ")");
      $result = $db->getrows($query->buildSelect());
      $values = array();
      // $values[] = 1;
      for($i =0;$i<count($result);$i++)
      {
        $values[] = $i+1;
      }
    }else
    {
      $values[] = 1;
    }
    return $values;
  }

  /**
   * Changes the level that you want to change
   *
   * @param int $id id of the level you want to change
   */
  function changeLevelByLvlId($id)
  {
    $tempId = $id;
    //  $selected = $this->getSelected();
    if ($id!=""){
      $values = $this->selectLevel($tempId);
      $this->setOptions($values,$values);
    }
  }
}
?>