<?php
include_once("achievotools.inc");
class competencypersonview extends atkNode
{
  function competencypersonview()
  {
    $page = &$this->getPage();
    $this->atkNode(competencypersonview,NF_NO_ADD|NF_NO_DELETE|NF_NO_EDIT|NF_NO_EXTENDED_SEARCH);
    $this->add(new atkAttribute("firstname",AF_SEARCHABLE));
    $this->add(new atkAttribute("lastname",AF_SEARCHABLE) );
    $this->add(new atkAttribute("competence",AF_SEARCHABLE));
    $this->add(new atkAttribute("added",AF_SEARCHABLE));
    $this->add(new atkAttribute("competencygroup",AF_SEARCHABLE));
    $this->add(new atkAttribute("competencydomain",AF_SEARCHABLE));
  }

  function action_report(&$handler)
  {
    $sessionManager = &atkGetSessionManager();
    //if we chose how many records we want to see we make view that value else we make it default 10
    $view = ($sessionManager->pagevar("view")?$sessionManager->pagevar("view"):10);
    //if we are on a differnt page then the first one we set offset that value else we set it default 0;
    $offset =  ($sessionManager->pagevar("pos")?$sessionManager->pagevar("pos"):0);
    //we look at what page we are
    $current =$offset/$view;
    $ui = &$this->getUi();
    $page = &$this->getPage();
    $this->addStyle("style.css");

    $user = getUser();
    $userId = $user["id"];
    //we split the orderby string for ascent or descent
    $order = $this->getSort($sessionManager->pageVar("atkorderby"));

    //open database
    $db = atkGetDb();
    //prepare output
    $output = "";
    //prepare the query for getting the data
    $query = $db->createQuery();
    //we add the table competences
    $query->addTable("competences");
    $query->addOrderBy($order);
    $query->addJoin("person","","id=person_id",person.person);
    $query->addJoin("competency","","competency.id=competency_id",competency.competency);
    $query->addJoin("competency_group","","competency_group.id=competency.group_id",competency.competencygroup);
    $query->addJoin("competency_domain","","competency_domain.id=competency_group.domain_id",competency.competencydomain);
    $query->addField("name","","competency");
    $query->addField("name as competencygroup","","competency_group");
    $query->addField("name as domain","","competency_domain");
    $query->addCondition("person.id =".$userId . " OR person.supervisor=".$userId);

    //when we use a search function
    if ($sessionManager->pageVar("atksearch"))
    {
      $search =$this->getSearch($sessionManager->pageVar("atksearch"));
      //for each key value pair we add a condition to the query if it contains a value
      foreach ($search as $name=>$value){
        if ($value)
        {
          $query->addCondition($name . " LIKE '" . $value . "%'");
        }
      }
    }
    //we execute this query before we ad any limits so we can use a count
    $result = $db->getrows($query->buildSelect());

    //after this we ad some more data for the table
    $query->addField("firstname");
    $query->addField("lastname");
    $query->addField("added");
    //if we chose to view all we wont give a limit to the query
    if ($view != -1)
    {
      $query->setlimit($offset,$view);
    }
    //after we add the limit we execute again so we  can use that for output
    $competences= $db->getrows($query->buildSelect());


    //we create the count string (records x-y of z)
    $recordCountString = "<p>Records ". ($offset+1) . "-" . ($offset+count($competences)) . " of " . count($result)."</p>";

    //we create the record so we can put this in the atkRecord list
    $rldata = array();
    foreach ($competences as $comp)
    {
      $record["firstname"]= $comp["firstname"];
      $record["lastname"]= $comp["lastname"];
      $record["added"]= $comp["added"];
      $record["competence"]= $comp["name"];
      $record["competencygroup"]= $comp["competencygroup"];
      $record["competencydomain"]= $comp["domain"];
      $rldata[] = $record;
    }

    $compnode = getNode("competency.competency");
    $rl = atknew("atk.recordlist.atkrecordlist");

    //we create the select so we can chose how many records we want to see on the screen
    //we give a onchange event to this select so when we change the number of views the page will refresh
    $option ="";
    $option.="<form name=\"fview\" action=\"/dispatch.php?\" method=\"get\">".chr(13);
    $option.="<input type=\"hidden\" name=\"pos\">";
    $option.=atkSessionManager::formState().chr(13)."<Select id=\"bla\" onchange=\"document.fview.submit()\" name=\"view\">".chr(13);
    $option.="<option". ($view==5?" selected":"") ." value=\"5\">5</option>".chr(13);
    $option.="<option". ($view==10?" selected":"") ." value=\"10\">10</option>".chr(13);
    $option.="<option". ($view==15?" selected":"") ." value=\"15\">15</option>".chr(13);
    $option.="<option". ($view==20?" selected":"") ." value=\"20\">20</option>".chr(13);
    $option.="<option". ($view==25?" selected":"") ." value=\"25\">25</option>".chr(13);
    $option.="<option". ($view==-1?" selected":"") ." value=\"-1\">Alles</option>".chr(13);
    $option.="</select>".chr(13);
    $option.="</form>".chr(13);

    $pageDirStr = $this->makePageDirector($offset,$current,$view,count($result));

    $contentblocks[] =$ui->renderBox(array("title"=>"Overview","content"=>$recordCountString."<BR>".$pageDirStr."<BR>".$option."<BR>".$rl->render($this, $rldata,array(),"","rlform")."<BR>".$pageDirStr."<BR><BR>".$recordCountString));
    $actionpage = $this->renderActionPage("admin", $contentblocks);
    $page->addContent($actionpage);
  }

  /**
   * Get the direction and field the table has to be sorted on
   *
   * @param String $sortString sort string from the session
   * @return formated string with correct name
   */
  function getSort($sortString)
  {
    $orderBy = explode(" ", $sortString);

    $field = $orderBy[0];
    $direction ="";
    if(count($orderBy) >1)
    {
      $direction = " ".$orderBy[1];
    }

    //what do we need to sort on not everywhere the name used is the same as the name we need to order on.
    switch ($field)
    {
      case "":
        $order = "domain";
        break;
      case "competence":
        $order = "name";
        break;
      case "competencydomain":
        $order = "domain";
        break;
      default:
        $order =$field;
        break;
    }
    return $order . $direction;
  }

  /**
   * Returns the search values
   *
   * @param array of search params $searchString
   * @return formated array with search params
   */
  function getSearch($searchString){
    $returnSearch = Array();
    foreach ($searchString as $name=>$value){
      switch ($name)
      {
        case "":
          $search = "domain";
          break;
        case "competence":
          $search = "competency.name";
          break;
        case "competencygroup":
          $search = "competency_group.name";
          break;
        case "competencydomain":
          $search = "competency_domain.name";
          break;
        default:
          $search =$name;
          break;
      }
      $returnSearch[$search]= $value;
    }
    return $returnSearch;
  }

  function makePageDirector($offset,$current,$size,$total)
  {
    $nr = ceil($total/$size);
    $output = "";
    if ($current != 0)
    {
      $output.="<a href=\"javascript:void(0)\" onclick=\"{fview.pos.value=". ($offset-$size). ";document.fview.submit()}\"><</a>  ";
    }
    for ($i=0;$i< $nr;$i++)
    {
      if ($i==$current)
      {
        $output.= $i . "|";
      }else{
        $output.= "<a href=\"javascript:void(0)\" onclick=\"{fview.pos.value=". ($i*$size). ";document.fview.submit()}\">".$i . "</a>|";
      }
    }

    if ($current < ($nr-1))
    {
      $output.="  <a href=\"javascript:void(0)\" onclick=\"{fview.pos.value=". ($offset+$size). ";document.fview.submit()}\">></a>  ";
    }
    return $output;
  }
}
?>