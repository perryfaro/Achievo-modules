<?php

atkimport("atk.atkmetanode");
atkimport('module.statusreport.utils.statusreportutil');

useattrib("statusreport.trafficlightattribute");

class statusreport extends atkMetaNode
{
  public $order = "id DESC";
  public $flags = array(NF_NO_DELETE, NF_NO_EDIT);
  public $m_traffic_light;
  public $m_traffic_color;

  public function __construct($options=array())
  {
    parent::__construct($options);
    $this->m_traffic_light = atkConfig::get("statusreport","traffic_light");
    $this->m_traffic_color = atkConfig::get("statusreport","traffic_color");
  }

  public function meta(atkMetaPolicy $policy)
  {
    $policy->hasOne("project.project",array("source"=>"project_id"));
    $policy->hasOne("employee.employee",array("source"=>"author"));
    $policy->hasOne("employee.employee",array("source"=>"mt_owner"));

    $policy->setTypeAndParams(array("pm_traffic_light","achievo_traffic_light"),"module.statusreport.attributes.trafficlightattribute");
    $policy->setTypeAndParams(array("progress","satisfaction","risks","countermeasures"),"atk.attributes.atktextattribute",array(array("rows"=>3)));

    $policy->addFlags("budget","report_date","achievo_traffic_light","achievo_flag_reason", AF_READONLY);
  }

  public function postMeta()
  {
    $this->getAttribute("achievo_flag_reason")->setForceInsert(true);
    $this->getAttribute("budget")->setForceInsert(true);
  }

  public function rowClass($rec, $nr)
  {
    return parent::rowClass($rec, $nr).' '.$this->m_traffic_color[$this->m_traffic_light[$rec['pm_traffic_light']][$rec['achievo_traffic_light']]];
  }

  public function preAddToEditArray(&$record, $mode="add")
  {
    //We have to see project on add page, so we remove AF_HIDE_ADD flag,
    //that has been add by editArray() method, because we use a filter with project_id
    if($mode=="add")
    {
      $this->getattribute("project_id")->removeFlag(AF_HIDE_ADD);
    }

    if(empty($record["pm_traffic_light"]))
    {
      list($last) = $this->selectDb("","id DESC",array(1),"",array("countermeasures","risks","project_id","pm_traffic_light","mt_owner"));
      if(atk_value_in_array($last))
      {
        $record["pm_traffic_light"] = $last["pm_traffic_light"];
        $record["risks"] = $last["risks"];
        $record["countermeasures"] = $last["countermeasures"];
        if(atk_value_in_array($last['mt_owner'])) $record['mt_owner'] = $last['mt_owner'];
      }

      $row = statusreportutil::getProjectData($record['project_id']['id']);
      $record['budget'] = empty($row['plan']) ? 0 : round($row['fact']/$row['plan']*100);
      $this->setAchievoMetric($row,$record["achievo_traffic_light"],$record["achievo_flag_reason"]);
    }
  }

  private function setAchievoMetric($row, &$light, &$reason)
  {
    if(empty($row['initial']))
    {
      $light = "green";
      $reason = $this->text("project_budget_dont_set");
    }
    /**
     * It should be red if the amount of hours is more than 5% over the budget.
     */
    elseif($row['fact'] > $row['initial']*1.05)
    {
      $light = "red";
      $reason = $this->text("more_5_persent");
    }
    /**
     * It should be amber if the amount of hours spend on the project is 0-5% higher
     * than the sum of the initial estimates of all the phases in the project
     */
    elseif($row['fact'] >= $row['initial'])
    {
      $light = "yellow";
      $reason = $this->text("less_5_persent");
    }
    /**
     * It should also be amber if
     * total spent > ((today-startday)/(endday-startday)) * sum(initial planning).
     */
    elseif(!$this->checkVelocity($row))
    {
      $light = "yellow";
      $reason = $this->text("bad_velocity");
    }
    else
    {
      $light = "green";
    }
  }

  private function checkVelocity($row)
  {
    atkimport("module.utils.dateutil");

    //if some data are incorrect - we can obtain devide by zero error
    $var = dateutil::date_diff(statusreportutil::date2date($row['enddate']),statusreportutil::date2date($row['startdate']))*$row['initial'];
    if($var == 0) return true;

    $plan = dateutil::date_diff(date("Ymd"),statusreportutil::date2date($row['startdate']))/$var;

    if($row['fact'] > $plan) return false;
    else return true;
  }

  public function budget_display($record, $mode="")
  {
    return  $this->getAttribute("budget")->display($record,$mode)."%";
  }

  public function author_selection($rec, $mode)
  {
    $pm[] = atkGetUserId();
    $pm[] = $this->getCoordinator($rec['project_id']['id']);

    $pm = atk_array_merge($pm, statusreportutil::getEmployeeIdWithRight("statusreport","statusreport","pm"));

    $selector = "1=0";
    if(!empty($pm))
    {
      $selector = "id IN(".implode(",",$pm).")";
    }

    $node = &atkGetNode("employee.employee");
    $result = $node->selectDb($selector,"","","",atk_array_merge($node->descriptorFields(),$node->m_primaryKey),$mode);

    return $result;
  }

  /**
   * Only people with "mt" privilege should be visible.
   *
   * @param array $rec db record
   * @param string $mode current mode (edit)
   * @return array recordset with mt employee
   */
  public function mt_owner_selection($rec, $mode)
  {
    atkimport('module.statusreport.utils.statusreportutil');
    $ps = statusreportutil::getEmployeeIdWithRight("statusreport","statusreport","mt");

    $selector = "1=0";
    if(!empty($ps))
    {
      $selector = "id IN(".implode(",",array_unique($ps)).")";
    }
    $node = &atkGetNode("employee.employee");
    $result = $node->selectDb($selector,"","","",atk_array_merge($node->descriptorFields(),$node->m_primaryKey),$mode);

    return $result;
  }

  private function getCoordinator($project_id)
  {
    $node = &atkGetNode("project.project");
    $node->addFlag(NF_NO_FILTER);
    list($row) = $node->selectDb("project.id='".$project_id."'","","","",array("coordinator"));
    return $row['coordinator']['id'];
  }

  public function initial_values()
  {
    return array("author"=>array("id"=>atkGetUserId()),
      "report_date"=>array("year"=>date("Y"),"month"=>date("m"),"day"=>date("d")));
  }
}