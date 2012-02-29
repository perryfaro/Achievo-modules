<?php
useattrib("atkdummyattribute");
userelation("atkmanytoonerelation");

class expense_lock extends atkNode
{
  function expense_lock()
  {
    $this->atkNode("expense_lock",NF_NO_EDIT|NF_NO_EXTENDED_SEARCH|NF_NO_VIEW|NF_MRA);
    $this->add(new atkAttribute("id", AF_AUTOKEY));
    $this->add(new atkDummyAttribute("weekexplain",atktext("hourlock_week_explanation"),AF_HIDE_LIST));
    $this->add(new atkAttribute("week",AF_SEARCHABLE|AF_OBLIGATORY, 6));
    $this->add(new atkDummyAttribute("useridexplain",atktext("hourlock_userid_explanation"),AF_HIDE_LIST));
    $this->add(new atkManyToOneRelation("userid","employee.employee", AF_SEARCHABLE));
    $this->setOrder("week DESC");
    $this->setTable("billing_expense_lock");
  }

  function descriptor_def()
  {
    return "[week]";
  }

  function postAdd($record)
  {
    atkdebug("postAdd");
    // After adding a lock for all users, we delete individual locks for that same week.
    if ($this->m_attribList["userid"]->isEmpty($record))
    {
      $db = &$this->getDb();
      $query = "DELETE FROM billing_expense_lock WHERE week='".$record['week']."' AND userid IS NOT NULL AND id<>".$record["id"];
      $db->query($query);
    }
  }

  function week_validate(&$record, $mode)
  {
    $value = $record["week"];
    $week = substr($value,4,2);
    $year = substr($value,0,4);
    if (strlen($value)!=6 || !is_numeric($value) || $week<0 || $week>53 || $year<1970)
    {
      triggerError($record, "week", "error_invalidweek");
    }
  }

  function userid_display($rec, $mode)
  {
    if ($this->m_attribList["userid"]->isEmpty($rec))
    {
      return "All users";
    }
    else
    {
      return $this->m_attribList["userid"]->display($rec, $mode);
    }
  }

}

?>
