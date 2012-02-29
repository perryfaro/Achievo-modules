<?php

class competency_profile_match extends atkNode
{

  var $profile = "0";

  var $user;

  function competency_profile_match()
  {
    $sessionManager = &atkGetSessionManager();
    $this->atkNode("competency_profile_match");
    $this->user = getUser();
    $this->profile = $sessionManager->pageVar("profile");
  }

  function action_stats(&$handler)
  {
    //We prepare the content
    $content = "";
    //We add the loading image
    $page = $this->getPage();
    $ui = $this->getUi();
    if (isset($this->user['id']))
    {
      $output = $ui->render(moduleDir("competency") . "templates/profile.tpl", $padrams, "competency");
      if (! isset($_GET['uri']))
      {
        $this->addStyle("style.css");
        $page->register_style(moduleDir("competency") . "styles/web.css");
        $page->register_style(moduleDir("competency") . "styles/proto.menu.0.6.css");
        $page->register_style(atkconfig("atkroot") . "atk/themes/default/styles/atkdateattribute.css");
        $boxes = array(
          $ui->renderBox(array(
            "title" => atkText("overview", "competency") . " " . atkText("competences", "competency") . " " . $this->m_username , 
            "content" => $output)));
        $actionpage = $this->renderActionPage("stats", $boxes);
        $page->addContent($actionpage);
      }
      else
      {
        $page->addContent($output);
      }
    }
    else
    {
      $page->addContent(atktext('this_page_cannot_be_viewed_as_administrator', 'competency'));
    }
  }
}
?>