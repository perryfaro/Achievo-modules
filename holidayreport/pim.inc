<?php

  function phr_getReviewItems()
  {
    global $g_securityManager;
    $rn = &getNode("holidayreport.holidayrequest_review");

    if ($g_securityManager->allowed("holidayreport.holidayrequest", "review"))
    {
      // is reviewer, show all items to review.
      atkdebug("reviewer");
      $rn->addFilter("holidayreport_request.status='request'");
    }
    else
    {
      // only show items we are reviewer of.
      atkdebug("no reviewer");
      $user = &getUser();
      $rn->addFilter("holidayreport_request.status='request' AND holidayreport_request.review='".$user["id"]."'");
    }
    $recs = $rn->selectDb();

    if (count($recs))
    {
      $actions = $rn->defaultActions("admin");
      unset($actions["delete"]);
      unset($actions["view"]);
      $actions["deny"] = dispatch_url("holidayreport.holidayrequest_review", "deny", array("id"=>"[id]"));
      $actions["grant"] = dispatch_url("holidayreport.holidayrequest_review", "grant", array("id"=>"[id]"));

      $rl = &atknew("atk.recordlist.atkrecordlist");

      return $rl->render($rn, $recs, $actions, 0);
    }
    return "";
  }

  function phr_myRequests()
  {
    $rn = &getNode("holidayreport.holidayrequest");

    // only show requests that are not yet handled, or requests that
    // are handled but are only 2 weeks old. (so to not clutter the
    // display.
    $stamp = date("Y-m-d", mktime(12,0,0,date("m"), date("d")-14, date("Y")));
    $user = &getUser();
    $rn->addFilter("holidayreport_request.userid='".$user["id"]."' AND (holidayreport_request.status='request' OR entrydate>'$stamp')");

    $recs = $rn->selectDb();

    if (count($recs))
    {
      $rl = &atknew("atk.recordlist.atkrecordlist");
      return $rl->render($rn, $recs, array());
    }
    return "";
  }

?>