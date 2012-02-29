<?php

  useattrib("atkdateattribute");
  useattrib("atknumberattribute");
  useattrib("atklistattribute");
  useattrib("atkfileattribute");
  userelation("atkmanytoonerelation");
  atkimport("module.billing.attributes.reminder_docmanager_linkattribute");

  class bill_reminder_number extends atkNode
  {
    function bill_reminder_number()
    {
      $this->atkNode("bill_reminder_number",NF_NO_EDIT);
      $this->setSecurityAlias("billing.bill");
      $this->add(new atkAttribute("id",AF_AUTOKEY));
      $this->add(new atkManyToOneRelation("bill_id", "billing.bill", AF_READONLY|AF_RELATION_AUTOLINK));
      $this->add(new atkNumberAttribute("reminder_number",AF_READONLY));
      $this->add(new atkListAttribute("template",$this->getReminderTemplates(),"",AF_OBLIGATORY));
      $this->add(new atkDateAttribute("date_created","","",0,0,AF_HIDE_ADD|AF_READONLY_EDIT));
      $this->add(new reminder_docmanager_linkattribute("link_to_docmanager",AF_HIDE_ADD));
      $this->setTable("bill_reminder_number");
      $this->setDescriptorTemplate("[bill_id.bill_nr] (reminder [reminder_number])");
    }

    function initial_values()
    {
      $return = array();

      // get next reminder number
      $db = &atkGetDb();
      $result = array_pop($db->getRows("SELECT MAX(reminder_number) AS mx FROM " . $this->m_table));
      $highest = $result['mx'];
      if (!$highest || !is_numeric($highest) || $highest < 10000)
        $highest = 10000;

      $return['reminder_number'] = ($highest + 1);

      return $return;
    }

    function getReminderTemplates()
    {
      return atkConfig::get('billing','reminderdoc_templates',array("reminder"));
    }

    function preAdd(&$record)
    {
      $record['date_created'] = array(
        "year" => date("Y"),
        "month" => date("m"),
        "day" => date("d")
      );

      return true;
    }

    function postAdd(&$record)
    {
      $this->generateReminderDoc($record['bill_id']['id'],$record['reminder_number'],$record['template']);
      return true;
    }

    function generateReminderDoc($bill_id,$reminder_number,$doctemplate)
    {
      $billNode = &getNode("billing.bill");
      
      // get the project id we are adding the reminder to!
      $row = array_pop($billNode->selectDb($billNode->m_table.'.id = "'.$bill_id.'"'));
      $project_id = $row['project_id']['projectid']['id'];
      
      // generate bill and tell the method to add the bill to the project.project docmanager
      $billNode->generateBill($bill_id,true,$reminder_number,"reminder_" . $reminder_number . ".sxw",$doctemplate,'project.project',$project_id);
    }
  }

?>
