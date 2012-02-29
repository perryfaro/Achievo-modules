<?php
$setup->executeSQL("
  CREATE TABLE `statusreport` (
    `id` int(11) NOT NULL,
    `project_id` int(11) NOT NULL,
    `report_date` date NOT NULL,
    `author` int(11) NOT NULL,
    `progress` text NOT NULL,
    `satisfaction` text NOT NULL,
    `budget` int(11) NOT NULL,
    `risks` text,
    `countermeasures` text,
    `mt_owner` int(11) default NULL,
    `pm_traffic_light` char(10) NOT NULL,
    `achievo_traffic_light` char(10) NOT NULL,
    `achievo_flag_reason` varchar(255) default NULL,
    PRIMARY KEY  (`id`))");

$setup->addColumn("person","show_all_flagged_projects","INT(1)");

$setup->setVersion(1);