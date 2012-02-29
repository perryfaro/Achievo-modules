<?php

class competencymatch extends atkNode
{

  var $user = "";

  var $allowed = "nothing";

  var $m_userhasviewall = "";

  function competencymatch()
  {
    $sessionManager = &atkGetSessionManager();
    $session = atkSessionManager::getSession();
    $this->atkNode("competencymatch");
    $this->user = getUser();
    //we check if this user is a supervisor
    $output = "";
    $db = atkGetDb();
    //prepare the query for getting the data\
    $query = $db->createQuery();
    //we add the table competences
    if (isset($this->user['id']))
    {
      $query->addTable("person");
      $query->addField("id");
      $query->addCondition("supervisor = " . $this->user["id"]);
      $persons = $db->getrows($query->buildSelect());
      if (count($persons) > 0)
      {
        $this->allowed = "team";
      }
      else
      {
        $this->allowed = "user";
      }
    }
  }

  function action_stats(&$handler)
  {
    $page = $this->getPage();
    $ui = $this->getUi();
    if (isset($this->user['id']))
    {
      $output = $ui->render(moduleDir("competency") . "templates/match.tpl", $padrams, "competency");
      if (! isset($_GET['uri']))
      {
        atkimport('atk.ui.atkdialog');
        $addDialog = new atkDialog($this->atkNodeType(), 'schedule_add', 'dialog');
        $addCall = $addDialog->getCall(true, false, false, true);
        $saveUrl = partial_url($this->atkNodeType(), 'schedule_save', 'save');

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

        $this->getPage()->register_loadscript("
          var addCall = {$addCall};
          var saveUrl = '{$saveUrl}';

          updateTodo();
          setScheduleURLs(addCall, saveUrl);
        ");
      }
      else
      {
        echo $output;
        die();
      }
    }
    else
    {
      $page->addContent(atktext('this_page_can_not_be_viewed_as_administrator', 'competency'));
    }
  }

  public function allowed($action, $record="")
  {
    $acl = atkNew('module.competency.utils.competencyacl');
    switch ($action)
    {
      case 'schedule_add': return $acl->allowCompetencyChange($_POST['user']);
      case 'schedule_save': return $acl->allowCompetencyChange($_POST['person']);
      default: return parent::allowed($action, $record);
    }
  }

  /**
   * Outputs the configuration dialog for a new schedule instance.
   */
  public function action_schedule_add()
  {
    $saveLabel = $this->text('save');      
    
    $page = $this->getPage();
    $result = '
      <form id="scheduleForm" onsubmit="(function() { saveScheduleDialog(); }).defer(); return false;">
        <input type="hidden" name="schedule_competence_id" value="'.$_POST['competence_id'].'" />
        <input type="hidden" name="schedule_person_id" value="'.$_POST['user'].'" />
        <div class="configContent">
          <table cellspacing="0" cellpadding="0" border="0">
            <tbody>
              <tr class="section_default" id="ar_planDate">
                <td valign="top" class="fieldlabel"><b>Datum</b>:  </td>
                <td valign="top" class="field" id="competency_competenceplanning_planDate"><input type=text id="schedule_plandate" name="plandate" /><input type="reset" onclick="return openCalendar(event);" value="..." class="atkDateAttribute button atkbutton"></td>
              </tr>
              <tr class="section_default" id="ar_remark">
                <td valign="top" class="fieldlabel"><b>Opmerking</b>:  </td>
                <td valign="top" class="field" id="competency_competenceplanning_remark"><textarea class="atkTextAttribute" cols="60" rows="8" wrap="soft" name="remark" id="schedule_remark"></textarea></td>
              </tr>
            </tbody>
          </table><br />
          <div class="configButtons">
            <input type="submit" class="atkdefaultbutton" value="'.atk_htmlentities($saveLabel).'">&nbsp;&nbsp;   
            <input type="button" onclick="ATK.Dialog.getCurrent().close();" value="'.atk_htmlentities($this->text('cancel')).'">
          </div>
        </div>
      </form>
    ';

    $page->register_scriptcode("ATK.Dialog.getCurrent().resize(700, 230);");
    $page->addContent($result);
  }

  /**
   * Save a new planning.
   */
  public function action_schedule_save()
  {
    $db = atkGetDb();

    $levelInfo = explode('!', $_POST['competence']);
    $levelId = $levelInfo[1];

    if (!empty($levelId))
    {
      $competenceId = $db->getValue("SELECT competency_id FROM competency_niveau WHERE id = ".$levelId);

      $db->query("INSERT INTO competency_planning (competency_id, niveau_id, person_id, planDate, remark)
                  VALUES ({$competenceId}, {$levelId}, {$_POST['person']}, '".$db->escapeSQL($_POST['date'])."', '".$db->escapeSQL($_POST['remark'])."')");
    }
  }
}
?>
