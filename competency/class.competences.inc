<?Php
useattrib("atktextattribute");
useattrib("atknumberattribute");
useattrib("atkdateattribute");
useattrib("atklistattribute");
useattrib('competency.levelattribute');
/**
 * Competences that employees have
 *
 */
class competences extends atkNode
{

  function competences()
  {

    $this->atkNode("competences");
    $attr = new atkManyToOneRelation("competency_id", "competency.competency", AF_OBLIGATORY | AF_SEARCHABLE);
    $attNiv = $this->add(new atkManyToOneRelation("niveau_id", "competency.competencyniveau", AF_OBLIGATORY | AF_SEARCHABLE));
    $this->add($attr,null,2);
    $this->add($attNiv,null,3);
    $this->add(new atkDateAttribute("added"));
    $attrPers = $this->add(new atkManyToOneRelation("person_id", "employee.employee", AF_OBLIGATORY | AF_SEARCHABLE));
    $this->add($attrPers);
    $this->setTable("competency_competences");
  }
  /**
   * Gets and returns the data to generate a graph to show how many (%)competences people in a team have
   *
   * @param array() $params the parameters to be passed to this data set (for example start and end date)
   * @param boolean $raw must the function return raw data (true) or formated date (Standard-false)
   * @return dataset with data to be used in the graph
   */
  function graph_totalcomp($params, $raw=false)
  {
    $user = getUser();
    $db = &atkGetDb();
    $query="SELECT p.lastname as label,p.firstname , count(c.person_id)as total FROM person p,competences c WHERE p.id = c.person_id AND (p.supervisor =".$user["id"]." OR id=".$user["id"].")GROUP BY c.person_id";

    $data = $db->getrows($query);

    if ($raw)return $data;

    $dataset = array();
    foreach ($data as $item)
    {
      $dataset[$item["firstname"]." " .$item["label"]] = $item["total"];
    }

    return array("totalcomp"=>$dataset);
  }

  /**
   * Gets and returns the data to generate a graph to show the growth of a person
   * in his/her competences
   *
   * @param array() $params the parameters to be passed to this data set (for example start and end date)
   * @param boolean $raw must the function return raw data (true) or formated date (Standard-false)
   * @return dataset with data to be used in the graph
   */
  function graph_growth($params, $raw=false)
  {
    $user =$params["user"];
    $db = &atkGetDb();
    $range = $db->getrows("SELECT
                                 min(added) as minimum,
                                 max(added) as maximum
                               FROM
                                 competences");
    $maxdate = array();
    $mindate = array();
    list ($maxyear, $maxmonth, $maxday) = explode("-", $range[0]["maximum"]);
    list ($minyear, $minmonth, $minday) = explode("-", $range[0]["minimum"]);

    $secondsinaday = 24*60*60;

    $resolution = $params["resolution"];

    if ($resolution=="auto"||$resolution=="")
    {
      $daysbetween = (adodb_mktime(12,0,0,$maxmonth,$maxday,$maxyear)-adodb_mktime(12,0,0,$minmonth,$minday,$minyear))/$secondsinaday;
      if ($daysbetween <= 31)
      {
        // one month range. Daybased stat makes the most sense.
        $resolution="day";
      }
      else if ($daysbetween <= 5*31)
      {
        // five month range. Week based stat makes the most sense.
        $resolution="week";
      }
      else
      {
        // > 5 month range. Month based stat makes most sense.
        $resolution="month";
      }
    }

    switch ($resolution)
    {
      case "month": $groupbystmt = "DATE_FORMAT(added,'%Y%m')"; break;
      case "week": $groupbystmt = "DATE_FORMAT(added,'%Y%V')"; break;
      default: $groupbybystmt = "";
    }



    $query="SELECT ".($groupbystmt==""?"added":$groupbystmt). ",added,firstname, lastname, count(added) as totaal FROM `competences` LEFT JOIN person ON id=person_id WHERE id=".$user." GROUP BY ".($groupbystmt==""?"added":$groupbystmt);
    $data = $db->getrows($query);

    if ($raw)return $data;

    $dataset = array();
    //we have a collection var to add all values together
    $totaladded;

    foreach ($data as $item)
    {
      $totaladded+=$item["totaal"];
      $dataset[$item["added"]] = $totaladded;
    }

    return array("totalcomp"=>$dataset);
    //}
  }

  /**
   * Get data for the company graphs
   *
   * @param Array $params parameters to be passed to this function/ Asked for ["team"]=> teamnumber
   * @param array $raw the unformatted array out of the database
   * @return Array formated array out of the database
   */
  function graph_company($params, $raw=false)
  {
    $db = &atkGetDb();
    $query="SELECT p.lastname as label,p.firstname , count(c.person_id)as total FROM person p,competences c WHERE p.id = c.person_id AND (p.supervisor =".$params["team"]." OR id=".$params["team"].")GROUP BY c.person_id";
    $data = $db->getrows($query);

    //little query to get the name of the supervisor
    $qSupervisor ="SELECT firstname,lastname from person where id =".$params["team"];
    $name = $db->getrows($qSupervisor);

    if ($raw)return $data;

    $dataset = array();
    foreach ($data as $item)
    {
      $dataset[$item["firstname"]." " .$item["label"]] = $item["total"];
    }

    return array("Team ". $name[0]["firstname"]." " . $name[0]["lastname"]=>$dataset);

  }

  function graph_companyTotal($params, $raw=false)
  {
    $user = getUser();
    $db = &atkGetDb();
    $query="SELECT distinct(person.supervisor)as team,p.lastname,p.firstname from person ,person p WHERE p.id = person.supervisor";

    $teams = $db->getrows($query);

    $dataset = array();

    foreach ($teams as $team)
    {
      if ($team["team"]){
        $qTeam = "SELECT p.id , p.firstname , p.lastname FROM person p,competences c WHERE (p.id =".$team["team"]." OR p.supervisor =".$team["team"].") AND c.person_id = p.id";
        $qTeams = $db->getrows($qTeam);
        $dataset["Team " . $team["firstname"] . " " . $team["lastname"]]  = count($qTeams);
      }
    }

    return array("Total Teams"=>$dataset);
  }
}
?>