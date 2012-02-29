<?php

  atkimport ("module.holidayreport.holidayrequest");

 /**
  * The node class for holiday_request.
  */
 class holidayrequest_review extends holidayrequest
 {
   function holidayrequest_review()
   {
     $this->holidayrequest("holidayrequest_review", NF_NO_ADD);
     $this->setSecurityAlias("holidayreport.holidayrequest");
     $this->addSecurityMap("admin", "review");
     $this->addSecurityMap("grant", "review");
     $this->addSecurityMap("deny", "review");

     $attr = &$this->getAttribute("remark");
     $attr->addFlag(AF_READONLY);

     $attr = &$this->getAttribute("userid");
     $attr->removeFlag(AF_HIDE);

     $this->add(new atkManyToOneRelation("review", "employee.employee"));
     $this->add(new atkTextAttribute("advise"));

   }

   function allowed($action, $record="")
   {
     if (($action=="deny"||$action=="grant"))
     {
       $user = getUser();

       if ($record=="" && $_REQUEST["id"]!="") // trying to perform the action (in which case the framework doesn't pass us the action.
       {
         $db = &$this->getDb();
         $recs = $db->getrows("SELECT review FROM holidayreport_request WHERE id=".$db->escapeSQL($_REQUEST["id"]));
         if (count($recs) && $recs[0]["review"]==$user["id"]) return true;
       }
       else
       {
         if ($record["review"]["id"] == $user["id"]) return true;
       }
     }
     return parent::allowed($action, $record);

   }

   function action_grant()
   {
     $this->updateStatus($_REQUEST["id"], "granted");
     $this->redirect();
   }

   function action_deny()
   {
     $this->updateStatus($_REQUEST["id"], "denied");
     $this->redirect();
   }

   function updateStatus($id, $status)
   {
     $db = &$this->getDb();
     return $db->query("UPDATE holidayreport_request SET status='$status' WHERE id='$id'");
   }

   function recordActions($record, &$actions)
   {
     unset($actions["delete"]);
     if ($record["status"]=="request") //||$record["status"]=="denied")
     {
       $actions["grant"] = dispatch_url("holidayreport.holidayrequest_review", "grant", array("id"=>"[id]"));
     }
     if ($record["status"]=="request") //||$record["status"]=="granted")
     {
       $actions["deny"] = dispatch_url("holidayreport.holidayrequest_review", "deny", array("id"=>"[id]"));
     }
   }

 }

?>
