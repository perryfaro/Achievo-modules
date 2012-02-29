<?php
atkimport('atk.front.atkfrontcontroller');
class companyController extends atkFrontController
{
  /**
   * 
   * @return void
   */
  function updateSelectAction()
  {
    if ($_GET["type"] == "competency")
    {
      if (isset($_GET["domain"]))
      {
        $this->fillCompetences($_GET["domain"]);
      }
    }
    else
    {
      if ($_GET["competency"] != "undefined")
      {
        $this->fillNiveauByCompetency($_GET["competency"], $_GET["domain"]);
      }
      else if (isset($_GET["domain"]))
      {
        $this->fillNiveauByDomain($_GET["domain"]);
      }
    }
  }

  function showAction()
  {
    $this->mythemedir = moduleDir('competency') . 'themes/default/';
  }

  /**
   * Returns a JSON string to fill the competency select
   *
   * @param unknown_type $domain
   */
  function fillCompetences($domain)
  {
    $db = &atkGetDb();
    //prepare output
    $output = "";
    //prepare the query for getting the data
    $query = &$db->createQuery();
    //we add the table competences
    $query->addTable("competency");
    $query->addField("id");
    $query->addField("name");
    if ($domain > 0)
    {
      $query->addCondition("domain_id =" . $domain);
    }
    $competences = $db->getrows($query->buildSelect());
    atkimport('atk.utils.atkjson');
    $json = atkJSON::encode($competences);
    ob_end_clean();
    print($json);
    die();
  }

  /**
   * This returns a JSON string to fill the niveau select by giving the domain
   *
   * @param int $domain id of the domain
   */
  function fillNiveauByDomain($domain)
  {
    $db = &atkGetDb();
    //prepare output
    $output = "";
    //prepare the query for getting the data
    $query = &$db->createQuery();
    //we add the table competences
    $query->addTable("competency_niveau");
    $query->addField("id");
    $query->addField("name");
    if ($domain > 0)
    {
      $query->addCondition("competency_id IN (SELECT id FROM competency where domain_id =" . $domain . ")");
    }
    $competences = $db->getrows($query->buildSelect());
    atkimport('atk.utils.atkjson');
    $json = atkJSON::encode($competences);
    ob_end_clean();
    print($json);
    die();
  }

  /**
   * Returns a JSON string for filling a select
   *
   * @param int $competency id of the competency
   * @param int $domain id of the domain
   */
  function fillNiveauByCompetency($competency, $domain)
  {
    $db = &atkGetDb();
    //prepare output
    $output = "";
    //prepare the query for getting the data
    $query = &$db->createQuery();
    //we add the table competences
    $query->addTable("competency_niveau");
    $query->addField("id");
    $query->addField("name");
    if ($competency > "0" && $competency > "")
    {
      if (is_numeric($competency))
      {
        $query->addCondition("competency_id IN (SELECT id FROM competency where id = " . $competency . ")");
      }
      else
      {
        $query->addCondition("competency_id IN (SELECT id FROM competency WHERE name LIKE '" . $competency . "')");
      }
    }
    if ($domain > 0 && $domain > "")
    {
      $query->addCondition("competency_id IN (SELECT id FROM competency where domain_id =" . $domain . ")");
    }
    $competences = $db->getrows($query->buildSelect());
    atkimport('atk.utils.atkjson');
    $json = atkJSON::encode($competences);
    ob_end_clean();
    print($json);
    die();
  }
}
?>