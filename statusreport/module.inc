<?php
  $modifiers[] = "employee.userprefs";
  $modifiers[] = "project.project";

  class mod_statusreport extends achievoModule
  {
    function getNodes()
    {
      registerNode("statusreport.statusreport", array("admin", "add","pm","mt"));
    }

    function getDependencies($mode)
    {
      return array("project");
    }


    function getPimItems()
    {
      return array("projectmanagerview"=>"getProjectManagerView",
                   "managementview"=>"getManagementView");
    }

    function getProjectManagerView()
    {
      include_once(moduleDir("statusreport")."pim.inc");
      return pimProjectManagerView();
    }

    function getManagementView()
    {
      include_once(moduleDir("statusreport")."pim.inc");
      return pimManagementView();
    }

    function userprefs_modifier(&$node)
    {
      $sr = &getNode("statusreport.statusreport");
      if($sr->allowed('mt'))
      {
        useattrib("atkBoolAttribute");
        $node->add(new atkBoolAttribute("show_all_flagged_projects"),'pim');
      }
    }

    /**
     * Map non-exist access right pimitem-pim to some exist access right
     *
     * @return array
     */
    function getPimRights()
    {
      return array("projectmanagerview"=>array('statusreport.statusreport','pm'),
      "managementview"=>array('statusreport.statusreport','mt'));
    }

    /**
     * Callback function for row coloring - see project_modifier below
     *
     * @param array $rec data row
     * @return string css class name
     */
    function rowColor($rec)
    {
      $sr = &getNode("statusreport.statusreport");
      list($rec) = $sr->selectDb("statusreport.project_id='".$rec['id']."'","statusreport.id DESC",1,"",array("pm_traffic_light","achievo_traffic_light"));
      $res = $sr->m_traffic_color[$sr->m_traffic_light[$rec['pm_traffic_light']][$rec['achievo_traffic_light']]];
      return $res;
    }

    /**
     * Project node modifier
     *
     * @param atkNode $node
     */
    function project_modifier(&$node)
    {
      /**
       * If you use PHP 5.3, you can use the below mentioned code
       */
      /*$callback = function($rec)
      {
        $sr = &getNode("statusreport.statusreport");
        list($rec) = $sr->selectDb("statusreport.project_id='".$rec['id']."'","statusreport.id DESC",1,"",array("pm_traffic_light","achievo_traffic_light"));
        $res = $sr->m_traffic_color[$sr->m_traffic_light[$rec['pm_traffic_light']][$rec['achievo_traffic_light']]];
        return $res;
      };

      $node->setRowClassCallback($callback);
      */

      $sr = &getNode("statusreport.statusreport");
      if($sr->allowed("pm") || $sr->allowed("mt"))
      {
        $node->setRowClassCallback(array($this,'rowColor'));

        $node->add(new atkOneToManyRelation("statusreport","statusreport.statusreport","project_id",AF_HIDE_LIST|AF_HIDE_ADD),"status_report");

        $node->addAllowedAction("status_update");
        $node->addAllowedAction("planning");
        $node->addAllowedAction("view_last_statusreport");

        atkDataGrid::registerModifier('project.project', array('mod_statusreport', 'addStyle'));
        atkDataGrid::registerModifier('statusreport.statusreport', array('mod_statusreport', 'addStyle'));

        $handler = atknew('module.statusreport.handlers.sradminhandler');
        atkRegisterNodeHandler($node->m_type, "admin", $handler);
      }
    }

    public static function addStyle()
    {
      $page = &atkinstance("atk.ui.atkpage");
      $theme = &atkinstance("atk.ui.atktheme");

      $page->register_style($theme->stylePath("recordlist.css"));
      $page->register_style($theme->stylePath("statusreport.css","statusreport"));
    }

  }