<?php
useattrib("atktextattribute");
useattrib("atknumberattribute");
useattrib("atkdateattribute");
userelation("atkmanyboolrelation");
/**
 * Here we create a profile for competences
 *
 */
class competencyprofile extends atkNode
{

  function competencyprofile()
  {
    $this->atkNode($name);
    $this->add(new atkAttribute("id", AF_PRIMARY|
    AF_HIDE|
    AF_AUTO_INCREMENT));
    $this->add(new atkAttribute("name",AF_OBLIGATORY|AF_UNIQUE));
    $this->add(new atkTextAttribute("description"));
    $managesRelation = $this->add(new atkManyBoolRelation('manages', 'competency.competencyprofileprivilege', 'competency.competencyprofile'));
    $managesRelation->setLocalKey('profile_id');
    $managesRelation->setRemoteKey('manages_profile_id');

    $this->setTable("competency_profile");
  }

  /**
   * We add an extra action that opens the screen that adds competences to the profile
   *
   * @param array $rec the record that we add an action to
   * @param array $actions
   * @param array $mraactions
   */
  function recordActions($rec, &$actions, &$mraactions)
  {
    $actions["addcompetences"] = session_url(dispatch_url("competency.competency_profile_match","addCompetences",array("atkaction"=>"stats","profile"=>$rec["id"])));
  }

  function descriptor_def()
  {
    return "[name]";
  }
}
?>