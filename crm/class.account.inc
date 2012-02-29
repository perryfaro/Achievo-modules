<?php

/**
 * Module CRM
 * Class account
 *
 * This class makes it possible to add, edit and delete accounts.
 *
 * $Id: class.account.inc 2951 2005-11-24 21:59:36Z sandy $
 * @author Sandy Pleyte  <sandy@ibuildings.nl>
 * @version $Revision: 2951 $
 *
 */
  

useattrib("atktextattribute");
useattrib("atkdummyattribute");
useattrib("atkparserattribute");
useattrib("atknumberattribute");
useattrib("atkemailattribute");
userelation("atkonetomanyrelation");
useattrib("atkUpdateStampAttribute");
userelation("atkmanytoonerelation");
useattrib("crm.lastModifiedByAttribute");


class account extends atkNode
{
  function account()
  {
    $this->atkNode("account", NF_ADD_LINK|NF_MRA);

    $this->add(new atkAttribute("id" ,AF_AUTOKEY));
    $this->add(new atkAttribute("name",AF_SEARCHABLE|AF_OBLIGATORY, 100));    
    $this->add(new atkAttribute("code",AF_SEARCHABLE, 100));   
    $this->add(new atkManyToOneRelation("member_of","crm.account",AF_HIDE_LIST|AF_LARGE));
    $this->add(new atkManyToOneRelation("crm_status","crm.account_crm_status",AF_HIDE_LIST));
    $this->add(new atkManyToOneRelation("account_source","crm.account_source",AF_HIDE_LIST));
    $this->add(new atkManyToOneRelation("industry","crm.account_industry",AF_HIDE_LIST));

     
    $this->add(new atkDummyAttribute("visitheader", "<B>".text("visitaddress", "organization")."<B>", AF_HIDE_LIST|AF_NO_LABEL));
    
   
    $this->add(new atkAttribute("visit_address", AF_SEARCHABLE|AF_HIDE_LIST, 100));
    $this->add(new atkAttribute("visit_address2", AF_HIDE_LIST, 100));
    $this->add(new atkAttribute("visit_zipcode", AF_HIDE_LIST, 20));
    $this->add(new atkAttribute("visit_city", AF_HIDE_LIST, 100));
    $this->add(new atkAttribute("visit_state", AF_HIDE_LIST, 100));
    $this->add(new atkAttribute("visit_country", AF_HIDE_LIST, 100));
    
    $this->add(new atkDummyAttribute("mailheader", "<B>".text("mailaddress", "organization")."<B>", AF_HIDE_LIST|AF_NO_LABEL));
    $this->add(new atkAttribute("mail_address", AF_SEARCHABLE|AF_HIDE_LIST, 100));
    $this->add(new atkAttribute("mail_address2", AF_HIDE_LIST, 100));
    $this->add(new atkAttribute("mail_zipcode", AF_HIDE_LIST, 20));
    $this->add(new atkAttribute("mail_city", AF_HIDE_LIST, 100));
    $this->add(new atkAttribute("mail_state", AF_HIDE_LIST, 100));
    $this->add(new atkAttribute("mail_country", AF_HIDE_LIST, 100));
    
    $this->add(new atkDummyAttribute("divider", "<Br>", AF_HIDE_LIST|AF_NO_LABEL));
    $this->add(new atkAttribute("phone",AF_SEARCHABLE|AF_FORCE_LOAD, 20));
    $this->add(new atkAttribute("other_phone",AF_SEARCHABLE|AF_HIDE_LIST, 20));
    $this->add(new atkAttribute("fax",AF_SEARCHABLE, 20));
    
    $this->add(new atkEmailAttribute("email",false,AF_SEARCHABLE|AF_HIDE_LIST, 50));
    $this->add(new atkEmailAttribute("other_email",false,AF_SEARCHABLE|AF_HIDE_LIST, 50));
    
    $this->add(new atkAttribute("website",0, 100));
    $this->add(new atkAttribute("bankaccount", AF_HIDE_LIST, 30));
    $this->add(new atkAttribute("bankaccount2", AF_HIDE_LIST, 30));
    $this->add(new atkAttribute("bankaccount3", AF_HIDE_LIST, 30));
    $this->add(new atkAttribute("bankaccount4", AF_HIDE_LIST, 30));
    $this->add(new atkAttribute("vatnumber", AF_HIDE_LIST, 25));
    $this->add(new atkNumberAttribute("employees",AF_HIDE_LIST,5));
    $this->add(new atkAttribute("revenue",AF_HIDE_LIST,5));
    
    $this->add(new atkManyToOneRelation("rate","crm.rate",AF_HIDE_LIST|AF_READONLY));
    $this->add(new atkManyToOneRelation("assigned_to","employee.employee",AF_SEARCHABLE));
    $this->add(new atkTextAttribute("remark", TEXT_LARGE, AF_HIDE_LIST));
    $this->add(new atkOneToManyRelation("contactperson","crm.contact","account",AF_HIDE_LIST),"contact");
    $this->add(new atkOneToManyRelation("former_names","crm.former_name","account",AF_HIDE_LIST),"former_names");
    $this->add(new atkOneToManyRelation("relation","crm.account_relation","account",AF_HIDE_LIST),"relation");

    $this->add(new atkManyToOneRelation("created_by","employee.employee",AF_HIDE_LIST|AF_READONLY),"history");
    $this->add(new atkDateTimeAttribute("created_on","","",AF_HIDE_LIST|AF_READONLY),"history");
    $this->add(new lastModifiedByAttribute("last_modified_by","employee.employee"),"history");
    $this->add(new atkUpdateStampAttribute("last_modified_on",AF_HIDE_LIST|AF_READONLY),"history");
    //$this->add(new notesManager(),"notes");
    //$this->add(new atkOneToManyRelation("contracts","organization.contracts","customer",AF_HIDE_LIST));
    //$this->addAttribute(new atkOneToManyRelation("projects","project.project","customer",AF_HIDE_LIST));
    //$this->add(new atkParserAttribute("projects", "see_below", AF_HIDE_LIST|AF_HIDE_ADD));
    
    $this->setTable("crm_account", "crm_account");
    $this->setOrder("crm_account.name");
    $this->setIndex("name");
  }
  
  
  function initial_values()
  {
    $user = getUser();
    return array("created_by"=>$user["id"],
                 "last_modified_by"=>$user["id"],
                 "created_on"=>array("year"=>date("Y"),
                                     "month"=>date("m"),
                                     "day"=>date("d"),
                                     "hours"=>date("H"),
                                     "minutes"=>date("i"),
                                     "seconds"=>date("s")));
    
    
  }
  
  function website_display($record)
  {
    $website = $record["website"];
    if (strlen($website)>0) return '<a href="http://'.$website.'" target="_blank">'.$website.'</a>';
    return "";
  }
  
  function website_edit($record, $mode)
  {
     $attr = &$this->getAttribute("website");
     $org = $attr->edit($record, $mode);
     return "http://".$org;
  } 
  
  function name_display($record)
  {
    global $ATK_VARS;
    $action = $ATK_VARS["atkaction"];
    $name = $record["name"];
    $viewUrl = session_url($_SERVER["PHP_SELF"].'?atknodetype='.$this->atkNodeType().
                              '&atkaction=view&atkselector='.rawurlencode($this->primaryKey($record)),
                               SESSION_NESTED);
    if (strlen($name)>0 && ($action!="view" && $action!="select")) return '<a href="'.$viewUrl.'">'.$name.'</a>';
    return $name;
  }
  


  function descriptor_def()
  {
    return "[name], [visit_city]";
  }
  
  function projects_edit($record)
  {
    return $this->showProjects($record, "relation");
  }
  
  function projects_display($record)
  {
    return $this->showProjects($record, "view");
  }
  
  function showProjects($record, $mode)
  {
    $projectnode = getNode("project.project");
    useattrib("project.filterprojectbyorganizationattrib");
    $projectnode->add(new filterProjectByOrganizationAttrib($record["id"]));
    $recs = $projectnode->selectDb("", "", "", $projectnode->m_listExcludes);
    if (count($recs)>0)
    {
      return $projectnode->normalRecordList($recs, $projectnode->defaultActions($mode), RL_NO_SEARCH|RL_NO_SORT);
    }
    return text("none");
  }
  
  
  function graph_cmpindustry($params, $raw=false)
  {      
    $db = $this->getDb();
    
    $data = $db->getrows("SELECT count( * ) AS total, cci.industry_name
                            FROM crm_account cc, crm_account_industry cci
                            WHERE cc.industry = cci.id
                            GROUP BY cc.industry"); 
    if ($raw)
    {
      return $data;
    }
                         
     // convert records to graph-compatible array.
    $dataset = array();                         
    for ($i=0, $_i=count($data); $i<$_i; $i++)
    {                    
      $dataset[$data[$i]["industry_name"]] = $data[$i]["total"];              
    }
      
    return array("account_industry"=>$dataset);
  }
  
  function graph_cmpsource($params, $raw=false)
  {      
    $db = $this->getDb();
    
    $data = $db->getrows("SELECT count( * ) AS total, ccs.source_name
                            FROM crm_account cc, crm_account_source ccs
                            WHERE cc.account_source = ccs.id
                            GROUP BY cc.account_source"); 
    if ($raw)
    {
      return $data;
    }
                         
     // convert records to graph-compatible array.
    $dataset = array();                         
    for ($i=0, $_i=count($data); $i<$_i; $i++)
    {                    
      $dataset[$data[$i]["source_name"]] = $data[$i]["total"];              
    }
      
    return array("account_source"=>$dataset);
  }
  
  function graph_cmpcrmstatus($params, $raw=false)
  {      
    $db = $this->getDb();
    
    $data = $db->getrows("SELECT count( * ) AS total, ccs.status_name
                            FROM crm_account cc, crm_account_status ccs
                            WHERE cc.crm_status = ccs.id
                            GROUP BY cc.crm_status"); 
    if ($raw)
    {
      return $data;
    }
                         
     // convert records to graph-compatible array.
    $dataset = array();                         
    for ($i=0, $_i=count($data); $i<$_i; $i++)
    {                    
      $dataset[$data[$i]["status_name"]] = $data[$i]["total"];              
    }
      
    return array("account_crmstatus"=>$dataset);
  }
  
  
}
?>
