<?php
/*
 TODO:

 - Blocks on, could be an dummy that get all depends on bug_id's where depend_on=current_bug
 
*/

useattrib("atkTextAttribute");
useattrib("atkListAttribute");
useattrib("atkDateTimeAttribute");
useattrib("atkUpdateStampAttribute");
useRelation("atkManyToOneRelation");
useRelation("atkOneToManyRelation");
useattrib("achilles.lastModifiedByAttribute");
useattrib("atkFuzzySearchAttribute");
useattrib("atkdummyattribute");

class bug extends atkNode 
{
  // Todo: Relations
  var $m_history_fields = array("title","keywords","url","priority");

  var $prioName = array("P1","P2","P3","P4","P5");
  var $prioId   = array(1,2,3,4,5);


  function bug()
  {
    global $ATK_VARS,$g_sessionManager;
	
    $this->atkNode("bug",NF_ADD_LINK|NF_TRACK_CHANGES|NF_NO_DELETE|NF_EXPORT);
    
    $this->add(new atkAttribute("bug_id",AF_PRIMARY|AF_READONLY|AF_AUTO_INCREMENT|AF_SEARCHABLE|AF_FORCE_LOAD));
    $this->add(new atkAttribute("title",AF_OBLIGATORY|AF_SEARCHABLE));
    $this->add(new atkTextAttribute("description",AF_HIDE_LIST|AF_OBLIGATORY));
    $this->add(new atkManyToOneRelation("severity_id","achilles.severity",AF_OBLIGATORY|AF_SEARCHABLE|AF_MANYTOONE_AUTOLINK));
    $this->add(new atkListAttribute("priority" , $this->prioName, $this->prioId,AF_OBLIGATORY|AF_SEARCHABLE));
    $this->add(new atkManyToOneRelation("project_id","project.project",AF_HIDE_LIST|AF_OBLIGATORY|AF_READONLY));
    $this->add(new atkManyToOneRelation("milestone_id","achilles.milestone",AF_SEARCHABLE|AF_MANYTOONE_AUTOLINK));

    $this->add(new atkManyToOneRelation("status_id","achilles.status",AF_SEARCHABLE|AF_MANYTOONE_AUTOLINK));

	
    $this->add(new atkListAttribute("completed",array("0%","10%","20%","30%","40%","50%","60%","70%","80%","90%","100%"),array(0,10,20,30,40,50,60,70,80,90,100), AF_NO_TRANSLATION|AF_LIST_NO_NULL_ITEM));

    $this->add(new atkManyToOneRelation("resolution_id","achilles.resolution",AF_SEARCHABLE|AF_MANYTOONE_AUTOLINK));

    $this->add(new atkManyToOneRelation("assigned_to","person.person"));
    $this->add(new atkManyToOneRelation("qa_contact","person.person",AF_HIDE_LIST));
    
    // TODO: component_id depends on project_id !
 
    $this->add(new atkManyToOneRelation("component_id","achilles.component",AF_HIDE_LIST|AF_MANYTOONE_AUTOLINK));

	 $project = &$this->getAttribute("project_id"); 


    // TODO: version_id depends on project_id !
    $this->add(new atkManyToOneRelation("version_id","achilles.version",AF_HIDE_LIST|AF_MANYTOONE_AUTOLINK));

/*
	 $project->addDependee("version_id"); //Subsets depend on the type of module
	 $achilles_version = &$this->getAttribute("version_id"); 
	 $achilles_version ->setDestinationFilter("achilles_version.project_id ='[project_id.id]'");
*/

    // TODO:: milestone_id depends on project_id !
/*
    $project->addDependee("milestone_id"); //Subsets depend on the type of module
    $achilles_milestone = &$this->getAttribute("milestone_id"); 
    $achilles_milestone ->setDestinationFilter("achilles_milestone.project_id ='[project_id.id]'");
*/

    $this->add(new atkManyToOneRelation("os_id","achilles.os",AF_HIDE_LIST|AF_MANYTOONE_AUTOLINK));
    $this->add(new atkAttribute("keywords",AF_HIDE_LIST));
    $this->add(new atkAttribute("url",AF_HIDE_LIST));
     
    // TODO: It seems the fuzzy attribut does work with numbers ?
    //$this->add(new atkFuzzySearchAttribute("dependency_search", "achilles.bug", "storeDependency", FSA_MULTISELECT));
    
    //12/09/2007  Dysfunctional depending, id Incorrect
	 //$this->add(new atkOneToManyRelation("dependency","achilles.bug_dependency","bug_id",AF_HIDE_LIST));
 	 $this->add(new atkOneToManyRelation("dependency","achilles.bug_dependency","depends_on",AF_HIDE_LIST));

    //$this->add(new atkFuzzySearchAttribute("blocks_search", "achilles.bug", "storeBlock", FSA_MULTISELECT));
    //$this->add(new atkOneToManyRelation("block","achilles.bug_dependency","depends_on",AF_HIDE_LIST));
    
	 //$this->add(new atkFuzzySearchAttribute("cc_search", "employee.employee", "storeCC", FSA_MULTISELECT));
    $this->add(new atkOneToManyRelation("cc","achilles.bug_cc","bug_id",AF_HIDE_LIST));
    
    //$this->add(new atkDateTimeAttribute("close_date","","",AF_HIDE_LIST|AF_HIDE_ADD|AF_READONLY));
    $comments = $this->add(new atkOneToManyRelation("comment","achilles.comment","bug_id",AF_HIDE_LIST|AF_HIDE_ADD));
	

    $this->add(new atkOneToManyRelation("attachement","achilles.attachment","bug_id",AF_HIDE_LIST|AF_HIDE_ADD));
    
    
    $this->add(new atkManyToOneRelation("created_by","employee.employee",AF_HIDE_LIST|AF_READONLY),"history");
    $this->add(new atkDateTimeAttribute("created_on","","",AF_HIDE_LIST|AF_READONLY),"history");
    $this->add(new lastModifiedByAttribute("last_modified_by","employee.employee"),"history");
    $this->add(new atkUpdateStampAttribute("last_modified_on",AF_HIDE_LIST|AF_READONLY),"history");
    
    $this->add(new atkOneToManyRelation("history","achilles.bug_history","bug_id",AF_HIDE_LIST|AF_HIDE_ADD|AF_READONLY),"history");
	//Message "story is not editable", despite the edit button
	$this->add(new atkDummyAttribute("no_edit", "<b>".atktext("no_edit")."</b>",AF_HIDE_LIST),"history");
    

    $prjid = $g_sessionManager->pageVar("selectedprojectid");
    $s_record["projectid"] = $g_sessionManager->pageVar("selectedproject");

    //$prjid Is always empty
    if($prjid!="")
    {
      $this->addFilter("achilles_bug.project_id=$prjid"); 
    }


   // 17/09/2007 It adds to sort by last amended

     $this->setOrder("achilles_bug.last_modified_on DESC");
     $this->setTable("achilles_bug");
     $this->addSecurityMap("viewprintable", "view"); 
  }
  
  function descriptor_def()
  {
    return "[bug_id] [title]";
  }
  
  function initial_values()
  {
    global $g_sessionManager;
    $user = getUser();
    $now = array("year"=>date("Y"),
                 "month"=>date("m"),
                 "day"=>date("d"),
                 "hours"=>date("H"),
                 "minutes"=>date("i"),
                 "seconds"=>date("s"));

   return array("project_id"=>array("id"=>$g_sessionManager->pageVar("selectedprojectid")),
                "created_on"=>$now,
                "created_by"=>array("id"=>$user["id"]),
                "last_modified_by"=>array("id"=>$user["id"]));
  }
  
  function preAdd(&$rec)
  {
    //TODO: if assigned_to or qa_contact is empty, use defaults from component 
  }
  
  function postUpdate($rec)
  {
    // TODO: if the record is changed, it should send a email
    foreach($this->m_history_fields as $field)
    {
      if($rec[$field]!=$rec["atkorgrec"][$field])
        $this->updateHistory($rec["bug_id"],$field,$rec["atkorgrec"][$field],$rec[$field]);
    }   

  // 18/09/07 It added foreign key fields for history
  $m_history_fields_fk_name = array("severity_id","status_id","resolution_id","project_id","component_id","version_id","milestone_id","os_id");

    foreach($m_history_fields_fk_name as $field)
    {
      if($rec[$field]["name"]!=$rec["atkorgrec"][$field]["name"])
        $this->updateHistory($rec["bug_id"],$field,$rec["atkorgrec"][$field]["name"],$rec[$field]["name"]);
    }   
	// assignedto and qa_contact have another person field
	$m_history_fields_fk_person = array("assigned_to", "qa_contact");
    foreach($m_history_fields_fk_person as $field)
    {
      if($rec[$field]["firstname"]!=$rec["atkorgrec"][$field]["firstname"])
        $this->updateHistory($rec["bug_id"],$field,$rec["atkorgrec"][$field]["lastname"].", ".$rec["atkorgrec"][$field]["firstname"],$rec[$field]["lastname"].", ".$rec[$field]["firstname"]);
    }   
  
     // 12/09/2007 It adds to see if it changed the person assigned to send your mail
     if($rec["assigned_to"]!=$rec["atkorgrec"]["assigned_to"])
        return $this->sendEmail($rec);
  }

  function postAdd($rec)
  {
     // 12/09/2007 It adds to see if it changed the person assigned to send your mail
    if($rec["assigned_to"]!="")
      return $this->sendEmail($rec);
  }



function phpEmail($emailFrom,$emailTo,$subject,$message, $emailCC="", $emailBCC="",$nameFrom="", $nameTo="", $file=""){

     $mail_host = "localhost";

     if (!empty($mail_host)) {
          $mail = &atknew("atk.utils.atkmailer");
          $mail->isHTML(true);
          $mail->From     = $emailFrom;
          $mail->FromName = $nameFrom;
          $mail->Host     = $mail_host;
         
          if (!empty($emailCC)) {
               $mail->AddCC($emailCC);   
          }

          if (!empty($emailBCC)) {
               $mail->AddBCC($emailBCC);   
          }

          // Now you only need to add the necessary stuff
          //$client_message = getClientMessage("feedback_thankyou");
          $mail->AddAddress($emailTo, $nameTo);
          $mail->Subject = $subject;//$subject;
          $mail->Body    = $message;//$message;
         
          $good_file = false;
          // attach file, if provided
      if (!empty($file)) {
           if(file_exists($file)){// only attach file if exists
              if(is_file($file)){   
                         $mail->AddAttachment($file);  // optional name
                         $good_file = true;
              }else{
                         //print "<h3><font color=red>ERROR: Attachment no a file</font></h3>For some reason the provided file, $file, is not a file";
                         return false;
                    }
           }else{
                    //print "<h3><font color=red>ERROR: Attachment does not exist</font></h3>For some reason the provided file, $file, does not exist";
                    return false;               
               }          
          }           
      
          if(!$mail->Send()){
             //print "<font color=red>There was an error sending the message</font>";
             return false;
          }

          // remove all previous emails
          $mail->ClearAllRecipients();
          $mail->ClearAttachments();         
         
          //print "<font color=green>Email sent successfully to $emailTo through mail host $mail_host<br> $save_msg </font>";
          return true;             
     }else{
          //print "<h3><font color=red>ERROR: No mail host has been defined</font></h3>Please set this at the general settings section or the company record section";
          return false;
     }
     

}// end function 

	// 12/09/2007 
    function sendEmail($rec)
    {

     //$owner = $rec["owner"]["id"];
     $newassign = $rec["assigned_to"]["id"];
     $oldassign = $rec["atkorgrec"]["assigned_to"]["id"];

     // read all relevant emails.
     $query = "SELECT DISTINCT lastname, firstname, email, userid, id
               FROM person
               WHERE id IN('".$g_user["id"]."','$newassign','$oldassign')";
		//        WHERE id IN('$owner','".$g_user["id"]."','$newassign','$oldassign')";
     $db = &atkGetDb();
     $emails    = $db->getrows($query);
     $pid  = $rec["project_id"]["id"];
     // get project name
     $project="";

     if($pid!="")
     {
       $query = "SELECT name FROM project WHERE id='$pid'";
       $res   = $db->getrows($query);
       $project = $res[0]["name"];
     }
     $body = "<B>".$this->text("bug_Body")." </B>".$rec["created_by"]["lastname"]." ".$this->text("bug_Body1").".<BR>";
	 $body.= "<B>#".$rec["bug_id"].".</B><BR>";
     $body.= "<B>".$this->text("title").": </B>".$rec["title"].".<BR>";
     $body.= "<B>".$this->text("description").":  </B> <BR>".$rec["description"].".<BR>";
	 $body.= "<B>".$this->text("bug_severity_id").": </B>".$rec["severity_id"]["name"].".<BR>";
     $body.= "<B>".$this->text("priority").": </B>".$this->prioName[$rec["priority"]-1].".<BR>";

	 if ($project!="") 
			$body.= "<B>".$this->text("bug_project_id").": </B>".$project."<BR>";

     $body.= "<BR>";
     for ($i=0;$i<count($emails);$i++)
     {
       if ($emails[$i]["id"]==$oldassign)
         $body.= "<B>".$this->text("bug_previouslyassignedto").":  </B>".$emails[$i]["lastname"].", ".$emails[$i]["firstname"]."<BR>";
       if ($emails[$i]["id"]==$newassign)
         $body.= "<B>".$this->text("bug_assignedto").":  </B>".$emails[$i]["lastname"].", ".$emails[$i]["firstname"]."<BR>";
       if ($emails[$i]["id"]==$g_user["id"])
         $body.= "<B>".$this->text("bug_changedby").":  </B>".$emails[$i]["lastname"].", ".$emails[$i]["firstname"]."<BR>";
       $to[] = $emails[$i]["email"];
     }
	 $location = "$ip";
	   
     $location.= session_url(dispatch_url($this->atknodetype(),"view",array("atkselector"=>$this->primaryKey($rec["bug_id"]))),SESSION_NESTED);

 	 $loc = "$ip";
	 $loc.= session_url(dispatch_url($this->atknodetype(), "view",array("atkselector"=>"achilles_bug.bug_id='".$rec["bug_id"]."'")),SESSION_DEFAULT);

     $body.= "<A href='".$loc."'>".$this->text("bug_editurl")."</A><BR>";

     // send mail
     $header = $this->prioHeader($rec["priority"]);
	 $subject = $this->text("bug_changed")." (#".$rec["bug_id"] .") ".$this->text("bug_project_id")." ".$project."";

//To send mail with HTML format
$this->phpEmail("sandy@achievo.org",implode(",",$to),$subject,$body, "", "","Achievo Bug", $emails[0]["lastname"].", ".$emails[0]["firstname"],"");

      return true;
    }

  
  function rowColor($record)
  {
	if($record["severity_id"]["row_color"]!="")
	  {
		// 20-02-08  If this fixed or closed takes away the color
		if ($record["status_id"]["name"]=="fixed" || $record["status_id"]["name"]=="Closed"){
		
		return   "#FFFFFF";
       }
		return   $record["severity_id"]["row_color"];
	  }
	  else return "";
  }
  
  function storeCC($rec, $cc)
  {
    global $g_db;
    for ($i=0, $_i=count($cc); $i<$_i; $i++)
    {
      $g_db->query("INSERT INTO achilles_bug_cc (bug_id, user_id) VALUES ('".$rec["bug_id"]."', '".$cc[$i]["id"]."')");
    }
  }

  function storeDependency($rec, $dependency)
  {
    global $g_db;
    for ($i=0, $_i=count($dependency); $i<$_i; $i++)
    {
      $g_db->query("INSERT INTO achilles_bug_dependency (bug_id, depends_on) VALUES ('".$rec["bug_id"]."', '".$dependency[$i]["bug_id"]."')");
    }
  }

  function storeBlock($rec, $block)
  {
    global $g_db;
    for ($i=0, $_i=count($block); $i<$_i; $i++)
    {
      $g_db->query("INSERT INTO achilles_bug_dependency (bug_id, depends_on) VALUES ('".$block[$i]["bug_id"]."', '".$rec["bug_id"]."')");
    }
  }
    
  function updateHistory($bug_id,$changed_field,$old_value,$new_value)
  {
    $hs_node = getNode("achilles.bug_history");
    $user = getUser();
    $now = array("year"=>date("Y"),
                 "month"=>date("m"),
                 "day"=>date("d"),
                 "hours"=>date("H"),
                 "minutes"=>date("i"),
                 "seconds"=>date("s"));
                 
    $rec = array("bug_id"=>$bug_id,"changed_field"=>$changed_field,
                 "old_value"=>$old_value,"new_value"=>$new_value,
                 "created_on"=>$now,"created_by"=>array("id"=>$user["id"]));
    $hs_node->addDb($rec);
    
  }

	// 12/09/07 Degree of progress
  function completed_display($rec, $mode) 
  {
    $percentage = (isset($rec["completed"]) && is_numeric($rec["completed"])?$rec["completed"]:0);
	if ($mode=="csv")
     {
        return $percentage;
     } 
    if($percentage>=75)
    {
      $color="green";
    }
    elseif($percentage>25 && $percentage<75)
    {
      $color="yellow";
    }
    else $color="red";

	$completed = '<div title="'.$percentage.'%"  style="height: 8px; width: 50px; border: 1px solid black; padding: 1px; text-align: left;">
              <div style="height: 8px; width: '.$percentage.'%; background: '.$color.';"></div>'.$percentage.' %
            </div>';
	

    return $completed;
  }
  // support function to add the priority header to a mail message
  function prioHeader($level)
  {
   $cr = chr(13).chr(10);

   switch ($level)
   {
    case 1:
     $mslevel = "High";
     break;

    case 2:
     $mslevel = "High";
     break;

    case 3:
     $mslevel = "Normal";
     break;

    case 4:
     $mslevel = "Low";
     break;

    case 5:
     $mslevel = "Low";
     break;

    default:
     $level   = 3;
     $mslevel = "Normal";
   }

   $header  = "X-Priority: ".$level.$cr;
   $header .= "X-MSMail-Priority: ".$mslevel.$cr;
   return $header;
  }



	function action_viewprintable(&$handler)
	{
	   $page = &$this->getPage();
	   $db = &$this->getDb();
     $rows = $this->selectDb();
	   $recordlist = &atknew("atk.recordlist.atkprintablerecordlist");
     $content = $recordlist->render($this, $rows, array(""), NULL);
     $page->addContent($content);
     $page->addContent('<div style="text-align:center;">Impreso ' . date("F j, Y - g:i a") . '</div>');
	   $page->register_loadscript("window.print();");
     return;
   }


   function adminFooter()
   { 
      $page = &$this->getPage();
        $page->register_script(atkconfig("atkroot")."atk/javascript/newwindow.js");
      
      //----- get the current param list, change action to viewprintable
       $sParams = str_replace("atkaction=admin", "atkaction=viewprintable", $_SERVER['QUERY_STRING']);
      
      $temp = atkPopup('dispatch.php',$sParams,'printWindow',600,1000);
      return '<input type="button" value="'.atkText("print_view").'" onClick="' . $temp . '" />';
      
   } 
}

?>