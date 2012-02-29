<?php

class account_stats extends atkNode
{

	function account_stats()
	{
		$this->atkNode("account_stats", NF_READONLY);
		$this->setSecurityAlias("crm.account");
	}

	function action_stats()
	{
		atkimport("atk.layout");
		$layout = &layout::getInstance();

		$layout->initGui();
		$layout->ui_top(text("stats"));

		$params["resolution"] = "auto";

		useattrib("graph.dataGraphAttribute");
		$cmpnode = &getNode("crm.account");

		$cmpindustry = &new dataGraphAttribute("account_industry", "cmpindustry", $params, "auto");
		$cmpindustry->setSource("crm.account");

		$output.= "<br><br>".$cmpindustry->showGraph();

		$cmpsource = &new dataGraphAttribute("account_source", "cmpsource", $params, "auto");
		$cmpsource->setSource("crm.account");

		$output.= "<br><br>".$cmpsource->showGraph();

		$cmpstatus = &new dataGraphAttribute("account_crmstatus", "cmpcrmstatus", $params, "auto");
		$cmpstatus->setSource("crm.account");

		$output.= "<br><br>".$cmpstatus->showGraph();

		$layout->output($output);

		$layout->ui_bottom();
	}
}

?>