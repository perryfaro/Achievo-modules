<?php

/**
 * Competency ACL implementation
 *
 * This is not yet used throughout the entire competency module, but it's a
 * start at least.
 */
class competencyAcl {

    /**
     * User currently logged in
     * @var array User record
     */
    protected $user;

    /**
     * Constructor
     */
    public function __construct() {
        $this->user = atkGetUser();
    }

    /**
     * Returns the profile id for the current user
     * @return integer Profile id
     */
    public function getProfile() {
        $db = atkGetDb();

        $query = $db->createQuery();
        $query->addTable("competency_profile_person");
        $query->addField("profile_id");
        $query->addCondition("person_id='" . $db->escapeSQL($this->user['id']) . "'");

        return $db->getValue($query->buildSelect(), 0);
    }

    /**
     * Returns selectable employees
     * @param array $fields Fields to include in the records
     * @return array Selectable employees (indexed array of records)
     */
    public function getSelectableEmployees(array $fields) {
        $db = atkGetDb();

        $query = $db->createQuery();
        $query->addTable('person');
        $query->addJoin('competency_profile_person', '', "person.id=competency_profile_person.person_id", true);
        $query->addCondition("role='employee'");
        $query->addCondition("status='active'");
        $query->addOrderBy('firstname');
        foreach($fields as $field) {
            $query->addField($field);
        }

        $competencyNode = atkGetNode('competency.competencymatch');
        if (!$competencyNode->allowed('show_all_users')) {
            $profileQuery = $db->createQuery();
            $profileQuery->addTable('competency_profile_privilege');
            $profileQuery->addCondition("profile_id='" . $db->escapeSQL($this->getProfile()) . "'");
            $profileQuery->addField('manages_profile_id');
            $conditions = array();
            $conditions[] = "id='" . $db->escapeSQL($this->user['id']) . "'";
            $conditions[] = "competency_profile_person.profile_id IN (" . $profileQuery->buildSelect() . ")";
            if ($competencyNode->allowed('show_own_team')) {
                $conditions[] = "supervisor='" . $db->escapeSQL($this->user['id']) . "'";
            }
            $query->addCondition('(' . implode(' OR ', $conditions) . ')');
        }

        return $db->getRows($query->buildSelect());
    }

    /**
     * Check if we're allowed to add or remove competencies for a user
     * @param integer $userId
     * @return boolean True if allowed
     */
    public function allowCompetencyChange($userId) {
        if ($this->user['id'] == $userId) {
            return false;
        }

        $competencyNode = atkGetNode('competency.competencymatch');
        if ($competencyNode->allowed('add')) {
            return true;
        } else {
            $db = atkGetDb();
            $query = $db->createQuery();
            $query->addTable('competency_profile_person', 'manager');
            $query->addJoin('competency_profile_privilege', 'privilege', 'manager.profile_id=privilege.profile_id', true);
            $query->addJoin('competency_profile_person', 'employee', 'privilege.manages_profile_id=employee.profile_id', true);
            $query->addCondition("manager.person_id='" . $db->escapeSQL($this->user['id']) . "'");
            $query->addCondition("employee.person_id='" . $db->escapeSQL($userId) . "'");
            $query->addField('COUNT(1)', 'allowed');
            $allowed = $db->getValue($query->buildSelect());
            return ($allowed == 1);
        }
    }

    /**
     * Check if we're allowed to set the profile for a user
     * @param integer $userId
     * @return boolean True if allowed
     */
    public function allowSetProfile($userId) {
        if ($this->user['id'] == $userId) {
            return false;
        }

        $competencyNode = atkGetNode('competency.competencymatch');
        if ($competencyNode->allowed('add')) {
            if ($competencyNode->allowed('show_all_users')) {
                return true;
            } elseif ($competencyNode->allowed('show_own_team')) {
                $db = atkGetDb();
                $query = $db->createQuery();
                $query->addTable('person');
                $query->addCondition("id='" . $db->escapeSQL($userId) . "'");
                $query->addCondition("supervisor='" . $db->escapeSQL($this->user['id']) . "'");
                $query->addCondition("role='employee'");
                $query->addCondition("status='active'");
                $query->addField('COUNT(1)', 'allowed');
                $allowed = $db->getValue($query->buildSelect());
                return ($allowed == 1);
            }
        }

        return false;
    }

}
