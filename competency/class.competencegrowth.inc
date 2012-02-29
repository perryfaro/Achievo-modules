<?php
require_once(atkconfig("atkroot")."atk/utils/adodb-time.inc.php");
class competencegrowth extends atkNode
{

  var $m_viewstart = "";
  var $m_viewend = "";
  var $m_resolution = "";
  var $m_user= "";
  var $m_username= "";
  var $m_person = "";
  var $m_legenda ="";

  function competencegrowth()
  {

    $this->atkNode("competencegrowth");

    $sessionManager = &atkGetSessionManager();
    $this->m_viewstart = $sessionManager->stackVar("viewstart");

    if ($this->m_viewstart=="")
    {
      $this->m_viewstart = date("Y-m-d",adodb_mktime(0,0,0,date("m")-1,date("d"),date("Y")));
    }
    else if (is_array($this->m_viewstart))
    {
      $this->m_viewstart = sprintf("%04d-%02d-%02d", $this->m_viewstart["year"],
      $this->m_viewstart["month"],
      $this->m_viewstart["day"]);
    }

    $this->m_viewend = $sessionManager->stackVar("viewend");
    if ($this->m_viewend=="")
    {
      $this->m_viewend = date("Y-m-d");
    }
    else if (is_array($this->m_viewend))
    {
      $this->m_viewend = sprintf("%04d-%02d-%02d", $this->m_viewend["year"],
      $this->m_viewend["month"],
      $this->m_viewend["day"]);
    }

    $this->m_person = $sessionManager->stackVar("person");
    if ($this->m_person=="")
    {
      $this->m_person = 0;
    }

    $this->m_zoom = $sessionManager->stackVar("zoom");

    $this->m_projectid = $sessionManager->stackVar("projectid");

    $this->m_resolution = $sessionManager->stackVar("resolution");
    if ($this->m_resolution=="") $this->m_resolution = "auto";

    if (isset($_GET["showLegenda"]))
    {
      $this->m_legenda = $_GET["showLegenda"];
    }else{
      $this->m_legenda = 0;
    }
  }

  function action_stats(&$handler)
  {
    $ui = &$this->getUi();
    $page = &$this->getPage();
    $page->register_script(moduleDir("competency")."javascript/competencygrowth.js");

    $this->addStyle("style.css");
    $page->register_scriptcode("setObservers();");
    $page->register_style(moduleDir("competency")."/styles/legenda.css");
    //we say we want to use the datagrapattribute
    useattrib("competency.extendedDataGraphAttribute");
    $startdate = $this->m_viewstart;
    $enddate = $this->m_viewend;

    if($this->m_legenda !=0)
    {
      $page->register_loadscript("toggleLegenda()");
    }
    $page->register_loadscript("initLegenda()");
    //we need top give some parameters if we only want a pie chart we can set $params = "1"
    //if we want to generate any of the other charts we have to give a start and end date so this
    //wil only work for showing values over time
    $params["resolution"] = $this->m_resolution;
    $params["user"] = $this->m_user;
    $params = array();
    $params["startdate"] = $this->m_viewstart;
    $params["enddate"]=  $this->m_viewend;

    $params["person"] = $this->m_person;

    //we want to see graphs for every domain in the profile of the person we chose

    $db = &atkGetDb();
    //prepare output
    $output = "";
    //prepare the query for getting the data
    $query = &$db->createQuery();
    //we add the table competences
    $query->addTable("competency");
    $query->addJoin("competency_domain","","competency_domain.id = competency.domain_id");
    $query->addCondition("competency.id IN (SELECT competency_id FROM competency_competences where person_id=".$params["person"] .")");
    $query->addField("distinct(competency.domain_id) as domain");
    $query->addField("competency_domain.name as dName");
    $domains= $db->getrows($query->buildSelect());

    foreach($domains as $domain)
    {
      $params["domain"]= $domain["domain"];

      $dname = str_replace(" ","_",$domain["dName"]);

      $graph = &new extendedDataGraphAttribute("Teamview", "growth", $params, "combBar");
      $graph->setSource("competency.competency");

      $graphString.= chr(13)."<div class=\"domain\">".chr(13);
      $graphString.=chr(13)."<A onclick=\"expand('bin".  $dname ."','img_". $dname."')\"><img id='img_".$dname."' align'=\"left\" src=\"./atk/images/minus.gif\" onclick=\"expand('bin".  $dname ."','img_". $dname."')\"> ".$domain["dName"]."</a>";
      $graphString.="<br><div id=bin". $dname.">".$graph->showGraph()."</div>";
      $graphString.="</div>";
    }
    $boxes = array($ui->renderBox(array("title"=>atkText("select","competency")." ".atkText("growth","competency")." ". $this->m_username,"content"=>"<BR>".$this->navigator()."<br>".$this->getLegenda())),
    $ui->renderBox(array("title"=>atkText("overview","competency")." ".atkText("growth","competency")." ". $this->m_username,"content"=>"<BR>".$graphString."<br>")),
    );

    $actionpage = $this->renderActionPage("stats", $boxes);
    $page->addContent($actionpage);
  }

  /**
   * 	Gets and returns the Data for the teamview table
   *
   * @param integer $id id of the user to get data from
   * @return $output as code to be renderd on the screen
   */
  function renderAsTable($id)
  {
    //open database
    $db = &atkGetDb();
    //prepare output
    $output = "";
    //prepare the query for getting the data
    $query = &$db->createQuery();
    //we add the table competences
    $query->addTable("competences");
    $query->addJoin("person","","id=person_id",person.person);
    $query->addField("firstname");
    $query->addField("lastname");
    $query->addField("added");
    $query->addField("count(added) as totaal");
    $query->addCondition('status="active"');
    $query->addCondition("role=\"employee\"");

    if ($id)
    {
      $query->addCondition("id=".$id);
    }

    $query->addGroupBy("added");

    //we exececute the query
    $competences= $db->getrows($query->buildSelect());

    //first entry in the array you will give for teh table will be the header
    //using atktext we can later change the titles with a language file
    //TODO (check if this is true)
    $data[] = array("#",atkText("firstname","competency"),atkText("lastname","competency"),atkText("added","competency"),atkText("competences","competency"),atkText("","project"));
    //now we add for every record a row to the array
    for ($i=0, $_i=count($competences); $i<$_i; $i++)
    {
      $row = array();
      $row[]=($i+1);
      $row[]= $competences[$i]["firstname"];
      $row[]= $competences[$i]["lastname"];
      $row[]= $competences[$i]["added"];
      $row[] = $competences[$i]["totaal"];
      $detailparams[atksearch] = array("firstname"=>$competences[$i]["firstname"],"lastname"=>$competences[$i]["lastname"],"added"=>$competences[$i]["added"]);

      $row[] = atkHref(dispatch_url("competency.competencypersonview", "report", $detailparams), "<img border='0' src='./atk/themes/steelblue/icons/recordlist/view.gif'>", SESSION_NESTED);
      //$row[] = "<img src='./images/history.png' height-16 width=16 alt=1>";
      $data[] = $row;
    }
    // we say we want to use the atktable renderen
    $tbl = &atknew("atk.utils.atktablerenderer");
    //we ant the second colm to be aligned to the right
    $tbl->setColAlignment(2,TBL_RIGHT);
    //we say we want to render the first row as header and want to alternate the color between rows by using TBL_DATA
    $output.=$tbl->render($data, TBL_DATA, "recordlist");
    return $output;
  }

  function navigator()
  {
    $user = getUser();

    useattrib("atkdateattribute");
    $startdate = $this->m_viewstart;
    $enddate = $this->m_viewend;
    $list = $this->m_person;
    $dummyrec = array("viewstart"=>array("year"=>substr($startdate,0,4),
    "month"=>substr($startdate,5,2),
    "day"=>substr($startdate,8,2)),
    "viewend"=>array("year"=>substr($enddate,0,4),
    "month"=>substr($enddate,5,2),
    "day"=>substr($enddate,8,2)));

    $startdateatt = new atkDateAttribute("viewstart","","", 0, date("Ymd"));
    $enddateatt = new atkDateAttribute("viewend","","", 0, date("Ymd"));

    $db = &atkGetDb();
    //prepare the query for getting the data\
    $query = &$db->createQuery();
    //we add the table competences
    $query->addTable("person");
    $query->addField("lastname");
    $query->addField("firstname");
    $query->addField("id");
    $query->addCondition('status="active"');
    $query->addCondition("role=\"employee\"");

    //if the person doesnt have show all then filter the data
    if ($this->userHasViewAll() == false && $this->user["name"] != "administrator")
    {
      $viewSelf = $this->userHasViewSelf();
      $viewTeam = $this->userHasViewTeam();

      if ($viewSelf && $viewTeam)
      {
        $query->addCondition("(supervisor=".$user[id]. " OR id=".$user[id].")");
      }else if ($viewTeam){
        $query->addCondition("(supervisor=".$user[id].")");
      }else if ($viewSelf){
        $query->addCondition("(id=".$user[id].")");
      }else{
        //we need this last one otherwise we automaticly see all what isnt the tyhing we want here
        $query->addCondition("(id=0)");
      }
    }
    $persons = $db->getrows($query->buildSelect());

    $valuesArray = array();
    $optionsArray = array();
    foreach ($persons as $person)
    {
      $valuesArray[]=$person["id"];
      $optionsArray[]=$person["firstname"] . " " . $person["lastname"];
    }

    $listAtt = new atkListAttribute("person",$optionsArray,$valuesArray);

    $listAtt->setInitialValue($list);
    $showLegendaAtrr = new atkBoolAttribute("showLegenda");
    $output = '<form action="'.getDispatchFile().'" method="get">';
    $output.= session_form();
    $output.= '<input type="hidden" name="atknodetype" value="'.$this->atkNodeType().'">';
    $output.= '<input type="hidden" name="atkaction" value="'.$this->m_action.'">';

    //now we create the table for a better layout
    $output.= "<table>";
    $output.="<tr>";
    $output .= "<td>".atkText("start date", "competency") . ':</td>' ;
    $output.= "<td>" . $startdateatt->edit($dummyrec) . '</td>' ;
    $output.="</tr><tr>";
    $output.= "<td>" .atkText("end date", "competency") .':</td>';
    $output.= "<td>" . $enddateatt->edit($dummyrec). "</td>";
    $output.="</tr><tr>";
    $output .= "<td>" . atkText("person", "competency") .":</td>";
    $output.= "<td>" . $listAtt->edit($list) . "</td>";
    $output.="</tr><tr>";
    $output.= "<td>" . atkText("show_legenda", "competency") .":</td>";
    $output.= "<td>" . $showLegendaAtrr->edit(array("showLegenda"=>$this->m_legenda)) . "</td>";
    $output.="</tr>";
    $output.="</table>";
    $output .= ' &nbsp;<br><input type="submit" value="' . atkText("refresh", "competency") . '">';
    $output .= '</form>';

    return $output;
  }

  /**
   * Checks if the current user has teh right to view all users
   *
   * @return Boolean
   */
  function userHasViewAll()
  {
    $user = getUser();
    $db = &atkGetDb();
    //prepare the query for getting the data\
    $query = &$db->createQuery();
    $query->addTable("person");//id
    $query->addJoin("employeerole","","employee_id=id",person);
    $query->addJoin("accessright","","employeerole.role_id=accessright.role_id",employeerole);
    $query->addField("lastname","","person");
    $query->addField("id","","person");
    $query->addCondition("accessright.action ='show_all_users'");
    $query->addCondition("accessright.node ='competency.competencegrowth'");

    if ($user[id])
    {
      $query->addCondition("person.id =".$user[id]);
    }

    $rights = $db->getrows($query->buildSelect());

    if (count($rights) >0)
    {
      return true;
    }
    return false;
  }

  function userHasViewSelf()
  {
    $user = getUser();
    $db = &atkGetDb();
    //prepare the query for getting the data\
    $query = &$db->createQuery();
    $query->addTable("person");//id
    $query->addJoin("employeerole","","employee_id=id",person);
    $query->addJoin("accessright","","employeerole.role_id=accessright.role_id",employeerole);
    $query->addField("lastname","","person");
    $query->addField("id","","person");
    $query->addCondition("accessright.action ='show_self'");
    $query->addCondition("accessright.node ='competency.competencegrowth'");

    if ($user[id])
    {
      $query->addCondition("person.id =".$user[id]);
    }

    $rights = $db->getrows($query->buildSelect());

    if (count($rights) >0)
    {
      return true;
    }
    return false;
  }

  /**
   * Makes the string for the legenda
   *
   * @return string HTML string to show the legenda
   */
  function getLegenda()
  {
    $output="<div class=legenda id=legenda>";
    $output.="Legenda";
    $output.="<div id=green class=green>". atktext("more_then_in_profile","competency")."</div>";
    $output.="<div id=blue class=blue>".atktext("currentely_acquired","competency")."</div>";
    $output.="<div id=red class=red>".atktext("in_profile_but_not acquired","competency")."</div>";
    $output.="</div>";
    //we wnat the div to be hidden as soon as it is loaded
    $output.="<script type=\"text/javascript\">toggleLegenda()</script>";
    return $output;
  }

  function userHasViewTeam()
  {
    $user = getUser();
    $db = &atkGetDb();
    //prepare the query for getting the data\
    $query = &$db->createQuery();
    $query->addTable("person");//id
    $query->addJoin("employeerole","","employee_id=id",person);
    $query->addJoin("accessright","","employeerole.role_id=accessright.role_id",employeerole);
    $query->addField("lastname","","person");
    $query->addField("id","","person");
    $query->addCondition("accessright.action ='show_team'");
    $query->addCondition("accessright.node ='competency.competencegrowth'");

    if ($user[id])
    {
      $query->addCondition("person.id =".$user[id]);
    }

    $rights = $db->getrows($query->buildSelect());

    if (count($rights) >0)
    {
      return true;
    }
    return false;
  }
}
?>
