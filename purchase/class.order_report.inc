<?php

  /**
   * order_report node class file
   *
   * Full description of the file
   *
   * @package achievo
   * @subpackage modules
   *
   * @author dennis <dennis@ibuildings.nl>
   *
   * @copyright (c) 2006 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing/ ATK open source license
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision: 3406 $
   * $Id: class.order_report.inc 3406 2006-09-14 09:29:45Z dennis $
   */
 
  /**
   * @internal Load used classes and files
   */
  atkimport("modules.reports.basereport");
  userelation("atkManyToOneRelation");
  useattrib("atkDateAttribute");
  useattrib("atklistattribute");

  /**
   * The node class for order_report.
   *
   * @author dennis <dennis@ibuildings.nl>
   * @package achievo
   * @subpackage modules
   */
  class order_report extends basereport
  {
    /**
     * monthlytimereg constructor
     */
    function order_report()
    {
      $this->basereport("order_report", NF_NO_SECURITY);      
      $this->add(new atkDateAttribute("startdate","","",0,0,AF_OBLIGATORY));
      $this->add(new atkDateAttribute("enddate","","",0,0,AF_OBLIGATORY));
      $this->add(new atkListAttribute("status", array("order", "received", "booked"), ""));
      $this->add(new atkManyToOneRelation("projectid","project.project"));      
    }

    /**
     * Initial values
     *
     * @return array Key=>value pairs representing the initial record to be used as filter
     */
    function initial_values()
    {
      return array(
        "startdate" => array("day"=>date("d"),"month"=>date("m")-1,"year"=>date("Y")),
        "enddate" => array("day"=>date("d"),"month"=>date("m"),"year"=>date("Y")),        
      );
    }
  
    /**
     * Constructs an array of content elements
     *
     * @param array $filterrecord Associative array containing the filter record
     * @return array Associative array containg $title=>$content pairs for each content element. Please note the title will still be translated
     */
    function getContentElements($filterrecord)
    {
      /* @var $order order */
      $order = &atkGetNode("purchase.order");
      $order->m_filters = array();      

      $db = &atkGetDb();
      /* @var $query atkQuery */
      $query = $db->createQuery();
      $query->addField("status","","purchase_order");
      $query->addField("name AS projectname", "","project");
      $query->addField("bill_no","","purchase_order");
      $query->addField("name AS suppliername","","organization");
      $query->addField("entrydate","","purchase_order");
      $query->addField("firstname","","person");
      $query->addField("lastname","","person");
      $query->addField("status","","person");
      $query->addJoin("project","","project.id=purchase_order.project_id",true);
      $query->addJoin("person","","person.id=purchase_order.user_id",true);
      $query->addJoin("organization","","organization.id=purchase_order.supplier_id",true);
      $query->addField("project_id","","purchase_order");
      $query->addTable("purchase_order");

      $startdateSQL = $this->getDateField($filterrecord["startdate"],"ymd");
      $enddateSQL = $this->getDateField($filterrecord["enddate"],"ymd");    
      
      $query->addCondition("purchase_order.entrydate>='$startdateSQL 00:00:00'");
      $query->addCondition("purchase_order.entrydate<='$enddateSQL 23:59:59'");
            
      if(isset($filterrecord["status"]) && $filterrecord["status"]!="")
        $query->addCondition("purchase_order.status='{$filterrecord["status"]}'");
                
      if(isset($filterrecord["projectid"]["id"]) && $filterrecord["projectid"]["id"]!="" && $filterrecord["projectid"]["id"]!=0)
        $query->addCondition("purchase_order.project_id='{$filterrecord["projectid"]["id"]}'");
      
      $records = $query->executeSelect();
      
      //if we have any results, we print them.
      if(count($records))
      {
        $data = array();
        
        // Compose the header and add it to the table
        $header = array();
        $header[] = atktext("project_id","purchase");
        $header[] = atktext("supplier_id","purchase");
        $header[] = atktext("bill_no", "purchase");
        $header[] = atktext("status", "purchase");
        $header[] = atktext("user_id","purchase");
        $header[] = atktext("entrydate", "purchase");
        $data[] = $header;
        
        foreach($records as $rec)
        {
          $row = array();
          $row[] = $rec["projectname"];
          $row[] = $rec["suppliername"];
          $row[] = $rec["bill_no"];
          $row[] = atktext($rec["status"],"purchase");
          $row[] = $rec["firstname"]." ".$rec["lastname"];
          $row[] = $rec["entrydate"];        
          $data[] = $row;
        }  
        $tbl = &atknew("atk.utils.atktablerenderer");
        $output = $tbl->render($data, TBL_DATA, "recordlist");
      }
      else 
        $output = atktext("no_results","purchase");
      
      $title = atktext("order_report","purchase").' - '
              .atktext("from","purchase").": "
              .$this->getDateField($filterrecord["startdate"],"dmy").' '
              .atktext("to","purchase")
              .": ".$this->getDateField($filterrecord["enddate"],"dmy");
      
      // Return in the title=>output array format
      $result = array($title=>$output);
      return $result;
    }
    
    /**
     * Function converts a date array to the y-m-d or the d-m-y format.     
     * 
     * @param array $arr The date array
     * @return String A formatted date string.
     */
    function getDateField($arr,$format="ymd")
    {
      switch($format)
      {
        case "ymd":
          return $arr["year"]."-".$arr["month"]."-".$arr["day"];          
        case "dmy":
          return $arr["day"]."-".$arr["month"]."-".$arr["year"];
        default:
          return $arr["day"]."-".$arr["month"]."-".$arr["year"];          
      }
      
    }
    

  }
?>