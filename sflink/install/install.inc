<?php

$setup->addColumn("organization","salesforce_id","VARCHAR(18)");
$setup->addColumn("person","salesforce_id","VARCHAR(18)");
$setup->executeSQL("CREATE TABLE `sf_dupe` (
    `id` int(10) NOT NULL auto_increment,
    `type` varchar(50),
    `type_id` int(10),
    `name` varchar(100),
    `name1` varchar(100),
    `created_at` datetime,
    `message` varchar(200),
    `fields` varchar(200),
    `statuscode` varchar(200),
    `direction` varchar(50),
    PRIMARY KEY  (`id`))");

$setup->executeSQL("CREATE TABLE `sflink_syncstatus` (
    `type` char(7) NOT NULL,
    `syncdatetime` char(24),
    PRIMARY KEY  (`type`))");

$setup->executeSQL("INSERT INTO sflink_syncstatus (type) VALUES ('account')");
$setup->executeSQL("INSERT INTO sflink_syncstatus (type) VALUES ('contact')");

$setup->setVersion(3);
