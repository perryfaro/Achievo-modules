<?php

/**
 * Competency profile privilege node
 * Represents the manages relation between competency profiles
 */
class competencyprofileprivilege extends atkNode {

    /**
     * Constructor
     * @param string $name Nodetype
     * @param integer $flags Node flags
     */
    public function __construct($name = __CLASS__, $flags = 0) {
        parent::__construct($name, $flags);

        useRelation('atkmanytoonerelation');
        $this->add(new atkManyToOneRelation('profile_id', 'competency.competencyprofile', AF_PRIMARY));
        $this->add(new atkManyToOneRelation('manages_profile_id', 'competency.competencyprofile', AF_PRIMARY));

        $this->setTable('competency_profile_privilege');
    }

}
