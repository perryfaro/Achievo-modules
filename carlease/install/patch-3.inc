<?php

$setup->installNode('carlease.carlease_insurance');
$cars = atkGetDb()->getRows('SELECT id,insurancecompany,insuranceownrisk FROM carlease WHERE insurancecompany IS NOT NULL OR insuranceownrisk IS NOT NULL');
$insurancenode = atkGetNode('carlease.carlease_insurance');
foreach($cars as $car)
{
    $insurance = array(
        'car_id'           => array('id' => $car['id']),
        'organization_id'  => array('id' => $car['insurancecompany']),
        'ownrisk'          => $car['insuranceownrisk'],
    );
    $insurancenode->addDb($insurance);
}
$setup->dropColumn('carlease.carlease', 'insurancecompany');
$setup->dropColumn('carlease.carlease', 'insuranceownrisk');
$setup->setVersion(2);
