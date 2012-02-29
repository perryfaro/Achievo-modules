<?php

  useattrib("atknumberattribute");
  userelation("atkmanytoonerelation");
  useattrib("atklistattribute");

  class discount extends atkNode
  {
    function discount()
    {
      global $g_sessionManager;

      $this->atkNode("discount");

      $this->add(new atkAttribute("id", AF_AUTOKEY));
      $this->add(new atkListAttribute("type", array("relative", "absolute"), array("r", "a"), AF_OBLIGATORY));
      $this->add(new atkNumberAttribute("amount", AF_OBLIGATORY, 10, 2));
      $this->add(new atkNumberAttribute("bill_line_id", AF_HIDE|AF_OBLIGATORY));

      $tmp = new atkManyToOneRelation("apply_on","billing.bill_line",AF_HIDE_LIST|AF_OBLIGATORY);

      $bill_id = $g_sessionManager->pageVar("bill_id");
      if ($bill_id!="")
      {
        $tmp->m_destinationFilter = "calcoption<>'discount' AND bill_id=".$bill_id;
      }

      $this->add($tmp);

      $this->setSecurityAlias("billing.bill");

      $this->setTable("billing_discount");
    }

    function postAdd($record, $mode="add")
    {
      // Get the short description and fixed amount
      $billlinenode = &atkGetNode("billing.bill_line");
      $discountnode = &atkGetNode("billing.discount");
      //$discounts = $discountnode->selectDb(sprintf("billing_discount.bill_line_id='%s'", $record["id"]));
      $shortdescription = '';
      $discount = $record;
      $originalamounts = $billlinenode->selectDb(sprintf("billing_bill_line.id='%s'", $discount["apply_on"]["id"]));
      $originalamount = $originalamounts[0]["fixed_amount"];
      if ($discount["type"]=="r")
      {
        $shortdescription .= abs($discount["amount"]) . "% over " . $originalamount;
        $fixed_amount = round($originalamount * (0 - $discount["amount"]) / 100, 2);
      }
      if ($discount["type"]=="a")
      {
        $shortdescription .= "EUR " . abs($discount["amount"]) . " van " . $originalamount;
        $fixed_amount = $originalamount - $discount["amount"];
      }

      // Apply the new fixed amount and shortdescription to the owner bill line
      $billlinenode = &atkGetNode("billing.bill_line");
      $billlines = $billlinenode->selectDb(sprintf("billing_bill_line.id='%s'", $discount["bill_line_id"]));
      $billline = $billlines[0];

      $billline["shortdescription"] = empty($billline["shortdescription"]) ? $shortdescription : ($billline["shortdescription"] . " $shortdescription");
      $billline["fixed_amount"] = $fixed_amount;

      $billlinenode->updateDb($billline);


      return true;
    }

  }

?>