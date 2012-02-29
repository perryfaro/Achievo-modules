<?php

atkimport('module.statusreport.utils.statusreportutil');

function pimProjectManagerView()
{
  $node = &atkGetNode("statusreport.statusreport");
  if($node->allowed("pm"))
  {
    $res = showProjectManagerPage();
  }
  else
  {
    $res = atktext("dont_have_report_right","statusreport");
  }
  return $res;
}

function pimManagementView()
{
  $node = &atkGetNode("statusreport.statusreport");
  if($node->allowed("mt"))
  {
    $res = showManagementPage();
  }
  else
  {
    $res = atktext("dont_have_report_right","statusreport");
  }
  return $res;
}

function showManagementPage()
{
  $user = atkGetUser();
  $res = atktext("pim_page_empty")."<br>";
  /**
   * Check, if management want to always see "all flagged projects" by default;
   */
  if($user["show_all_flagged_projects"])
  {
    $ids = statusreportutil::getFlaggedProjectid();
  }
  else
  {
    $ids = statusreportutil::getProjectIdByMTOwner($user['id']);
  }

  if(!empty($ids))
  {
    $selector = "project.id IN(".implode(",",$ids).")";

    $node = &atkGetNode("project.project");
    $node->m_action = 'admin';
    atkimport('atk.datagrid.atkdatagrid');
    $grid = atkDataGrid::create($node, 'mtview');

    $grid->setMode('admin');
    $grid->removeFlag(atkDataGrid::EXTENDED_SEARCH|atkDataGrid::SEARCH|atkDataGrid::SORT);

    $actions = array();
    $actions["edit"] = dispatch_url("project.project","edit",array("atkselector"=>"[pk]"));
    $actions["planning"] = dispatch_url("project.project","edit",array("atkselector"=>"[pk]","atktab"=>"planning"));
    $actions["status_update"] = dispatch_url("statusreport.statusreport","add",array("atkfilter"=>"statusreport.project_id='[id]'"));
    $actions["view_last_statusreport"] = dispatch_url("statusreport.statusreport","view",array("atkfilter"=>"statusreport.project_id='[id]'"));

    $grid->setDefaultActions($actions);
    $grid->addFilter($selector);

    $res = $grid->render();

  }
  $res .="<div>".href(dispatch_url("reports.hoursurvey", "report"),atktext("menu_hours_hoursurvey"),SESSION_NESTED)."&nbsp;|&nbsp;".
      href(dispatch_url("project.project", "admin",array("flagged"=>"flagged")),atktext("show_all_flagged_projects","statusreport"),SESSION_NESTED)."</div>";

  return $res;
}

/**
 * When a project manager logs into Achievo, his PIM welcome page should
 * always contain a list of his active projects (this means: projects
 * where status is "active" and hours have been booked in the last 14 days,
 * and coordinator is set to the currently logged in user.
 *
 * @return string
 */
function showProjectManagerPage()
{
  $res = atktext("pim_page_empty")."<br>";
  $user = atkGetUserId();
  $ids = statusreportutil::getBookedProjectId(14,$user);
  if(!empty($ids))
  {
    $node = &atkGetNode("project.project");
    $node->m_action = 'admin';
    $selector = "project.coordinator='$user' AND project.id IN(".implode(",",$ids).")";

    atkimport('atk.datagrid.atkdatagrid');
    $grid = atkDataGrid::create($node, 'pmview');

    $grid->setMode('admin');
    $grid->removeFlag(atkDataGrid::EXTENDED_SEARCH|atkDataGrid::SEARCH|atkDataGrid::SORT);

    $actions = array();
    $actions["edit"] = dispatch_url("project.project","edit",array("atkselector"=>"[pk]"));
    $actions["planning"] = dispatch_url("project.project","edit",array("atkselector"=>"[pk]","atktab"=>"planning"));
    $actions["status_update"] = dispatch_url("statusreport.statusreport","add",array("atkfilter"=>"statusreport.project_id=[id]"));

    $grid->setDefaultActions($actions);
    $grid->addFilter($selector);

    $res = $grid->render();
  }
  $res .="<div>".href(dispatch_url("reports.hoursurvey", "report"),atktext("menu_hours_hoursurvey"),SESSION_NESTED)."</div>";
  return $res;
}

