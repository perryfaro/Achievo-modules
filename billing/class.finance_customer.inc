<?php

  userelation("atkmanytoonerelation");
  userelation("atkonetomanyrelation");
  useattrib("atkdummyattribute");

  include_once(moduleDir("organization")."class.organization.inc");

  class finance_customer extends organization
  {
    function finance_customer()
    {
      $this->organization();
      $this->m_type = "finance_customer";
      $this->m_flags |= NF_NO_ADD|NF_NO_DELETE;

      // Put all original attribs to readonly
      foreach (array_keys($this->m_attribList) as $key)
      {
        $p_attrib = &$this->m_attribList[$key];
        $p_attrib->addFlag(AF_READONLY);
      }

      $this->add(new atkDummyAttribute("divider", "<hr>", AF_HIDE_LIST|AF_NO_LABEL));
      $this->add(new atkManyToOneRelation("billing_currency","billing.currency",AF_SEARCHABLE));
      $this->add(new atkOneToManyRelation("rates","billing.billing_rate","customer_id",AF_HIDE_LIST));

      $this->m_securityAlias = "billing.finance_customer";
    }

  }
?>