<?php

$setup->executeSQL("CREATE TABLE `sflink_syncstatus` (
    `type` char(7) NOT NULL,
    `syncdatetime` datetime,
    PRIMARY KEY  (`type`))");

$setup->executeSQL("INSERT INTO sflink_syncstatus (type) VALUES ('account')");
$setup->executeSQL("INSERT INTO sflink_syncstatus (type) VALUES ('contact')");

$setup->setVersion(2);
