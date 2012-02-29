<?php

useAttrib('atkCurrencyAttribute');
useAttrib('atkTextAttribute');
useRelation('atkManyToOneRelation');

class carlease_insurance extends atkNode
{
    public function __construct($name=__CLASS__, $flags=0)
    {
        parent::__construct($name, $flags);

        $this->add(new atkAttribute('id', AF_AUTOKEY));
        $this->add(new atkManyToOneRelation('car_id', 'carlease.carlease', AF_HIDE));
        $this->add(new atkManyToOneRelation('organization_id', 'organization.organization', AF_RELATION_AUTOLINK|AF_RELATION_AUTOCOMPLETE));
        $this->add(new atkCurrencyAttribute('ownrisk', 0, 10, '&euro;'));
        $this->add(new atkTextAttribute('description', AF_SEARCHABLE))
             ->m_rows = TEXT_LARGE;

        $this->setTable('carlease_insurance');
        $this->setOrder('organization_id.name');
        $this->setDescriptorTemplate('[organization_id.name]');
    }
}
