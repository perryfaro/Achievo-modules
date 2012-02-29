<?php
atkimport('atk.front.atkfrontcontroller');

class domainholescontroller extends atkFrontController
{

  function showAction()
  {
  }

  /**
   * Shoews the graph for competency holes.
   *
   * @param  int $profileId what profile do we need to show the holes for
   * @param  string $type
   * @param  $domainId what domain do we want to show the graph for
   * @return void
   */
  function competencyHoleAjax($profileId, $type, $domainId)
  {
    $this->showGraph($profileId, $domainId);
  }

  /**
   * Partial that is called when one of the values changes and then rerenders the
   * graph
   * @return void
   */
  function showGraphAction()
  {
    $this->mythemedir = moduleDir('competency') . 'themes/default/';
    $this->showGraph($_GET['profileId'], $_GET['domain']);
  }

  /**
   * function that takes care of collection all the data needed to render the graph
   *
   * @param  int $profileId id of the profile
   * @param  int $domain id of the domain
   * @return bool
   */
  function showGraph($profileId, $domain)
  {
    //when we didn't chose a profile yet we don't need to load anything so we give the message that the user should select a
    //profile to see the graph
    if ($profileId == 0)
    {
      print("Select a profile to see graph");
      return true;
    }
    $graphwidth = atkconfig("graph_width", 550);
    $graphheight = round($graphwidth / 650 * 250);
    $params = array();
    $params["profile"] = $profileId;
    $params["reach"] = 10;
    useattrib("competency.extendeddatagraphattribute");
    $node = getNode("competency.competency");
    //$countRec = new competency();
    $recordset = $node->graph_competencyDomainHoles($params, true);
    $records = count($recordset);
    $recordsLeft = $records % 10;
    $graphs = ceil($records / 10);
    $nr = ceil($records / $graphs);
    //we want a graph for every 10 records
    for ($i = 0; $i < $graphs; $i ++)
    {
      //if we have more then 1 graph its not nessesary we have dummy records when one of the graphs
      //has to little points. So in that case we borrow plots from the previous graph and add them
      //to the later graph so that we have only data that mathers.
      $params["reach"] = $nr;
      $params["offset"] = $i * $nr;
      $graph = &new extendedDataGraphAttribute("Competency_Void", "competencyDomainHoles", $params, "radar");
      $graph->setSource("competency.competency");
      $this->output .= (" <IFRAME width=" . ($graphwidth + 20) . " height=" . ($graphheight + 20) . " scrolling=NO frameborder=0 src=\"" . $graph->getSRc() . "\"></iframe");
    }
  }
}
?>
