<?php

class carlease_currentOwnerAttribute extends atkAttribute
{
    public function __construct($name, $flags=0)
    {
        parent::__construct($name, $flags|AF_NO_SORT|AF_HIDE_ADD|AF_HIDE_EDIT|AF_HIDE_SEARCH);
        $this->setLoadType(POSTLOAD);
        $this->setStorageType(NOSTORE);
    }

    public function load($db, $record)
    {
        $ownerdescriptions = array();
        $ownernode = atkGetNode('carlease.carlease_owner');
        $employeenode = atkGetNode('employee.employee');
        $where = sprintf("carlease_owner.car_id='%d'
                     AND (carlease_owner.startdate IS NULL OR carlease_owner.startdate <= NOW())
                     AND (carlease_owner.enddate   IS NULL OR carlease_owner.enddate   >= NOW())", $record['id']);
        $owners = $ownernode->selectDb($where);
        foreach($owners as $owner) {
            $ownerdescriptions[] = $employeenode->descriptor($owner['employee']);
        }
        return implode(', ', $ownerdescriptions);
    }
}
