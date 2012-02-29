<?php
  /**
   * coveragereport  node class file
   *
   * @package achievo_modules
   * @subpackage billing
   *
   * @author ivo <ivo@ibuildings.nl>
   *
   * @copyright (c) 2005 Ibuildings.nl BV
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision: 2758 $
   * $Id: class.coveragereport.inc 2758 2006-05-22 00:18:30Z guido $
   */

  /**
   * @internal includes
   */
  useattrib("atkattribute");
  useattrib("atkdateattribute");

  atkimport("module.utils.dateutil");

  include_once("achievotools.inc");

  /**
   * The coveragereport node class
   *
   * @author ivo <ivo@ibuildings.nl>
   * @package achievo
   * @subpackage reports
   */
  class coveragereport extends atkNode
  {
    /**
     * coveragereport constructor
     */
    function coveragereport()
    {
      $this->atkNode("coveragereport");
      $this->setSecurityAlias("billing.report");
      $this->addSecurityMap("plot", "coveragereport");
    }

    function getFilterRecord()
    {
      static $s_record = NULL;
      global $g_sessionManager;

      if ($s_record==NULL)
      {
        $s_record = array();
        //$s_record["coordinator"] = $g_sessionManager->pageVar("coordinator");
        $s_record["from"] = $g_sessionManager->pageVar("from");
        $s_record["to"] = $g_sessionManager->pageVar("to");
        $s_record["includeholiday"] = $g_sessionManager->pageVar("includeholiday");

        // convert attribute html values to internal values
        $attrs = &$this->getFilterAttrs();
        foreach (array_keys($attrs) as $attribname)
        {
          $p_attrib = &$attrs[$attribname];
          $s_record[$attribname] = &$p_attrib->fetchValue($s_record);
        }


        $go = $g_sessionManager->pageVar("go");
        if ($go!=1)
        {
          // initial loading. We'll put the from/to fields to reasonable defaults.

          $s_record["from"] = dateUtil::str2arr(dateUtil::startOfWeek(date("Ymd")));

          // next month
          $next_month = mktime(12,0,0,date("m")+1, date("d"), date("Y"));
          $s_record["to"] = dateUtil::str2arr(dateUtil::endOfWeek(date("Ymd", $next_month)));
        }
      }
      return $s_record;
    }

    function &getFilterAttrs()
    {
      useattrib("employee.myemployeesattribute");

      //$attrs["coordinator"] = &new myEmployeesAttribute("coordinator", "project.project.any_project");

      useattrib("atklistattribute");
      $attrs["includeholiday"] = &new atkListAttribute("includeholiday", array(atktext("exclude", "billing"), atktext("include", "billing")), array("exclude", "include"), AF_OBLIGATORY|AF_LIST_NO_NULL_ITEM|AF_NO_TRANSLATION);

      $attrs["from"] = &new atkDateAttribute("from");
      $attrs["to"] = &new atkDateAttribute("to");

      return $attrs;
    }

    function getFilterBar()
    {
      $record = $this->getFilterRecord();

      $attrs = &$this->getFilterAttrs();

      $output = '<form action="dispatch.php" method="get">';
      $output.= session_form();
      $output.= '<input type="hidden" name="go" value="1">'; //trigger to prevent loading the first time

      //if ($this->hasAnyProjectPrivilege())
      //{
        //$output.= atktext("coordinator").": ".$attrs["coordinator"]->edit($record)." ";
      //}

      $output.= atktext("includeholidays", "billing").": ".$attrs["includeholiday"]->edit($record)." ";

      $output.= "<br/>";

      $output.= atktext("show", "billing")." ".$attrs["from"]->edit($record)." ";
      $output.= atktext("to")." ".$attrs["to"]->edit($record);

      $output.= ' <input type="submit" value="'.atktext("refresh").'">';
      $output.= '</form>';

      return $output;
    }

    function hasAnyProjectPrivilege()
    {
      global $g_securityManager;
      return $g_securityManager->allowed("project.project", "any_project");
    }

    function action_coveragereport()
    {
      $filter = $this->getFilterRecord();

      $content = $this->getFilterBar();

      if ((array_key_exists("go", $this->m_postvars)) && ($this->m_postvars["go"]==1))
      {
        $coordinator = ""; // no filtering for now!
        //if (!$this->hasAnyProjectPrivilege())
        //{
//          $user = getUser();
          //$coordinator = $user["id"];
        //}
        //else
        //{
//          $coordinator = $filter["coordinator"]["id"];
        //}
        $imgurl = session_url(dispatch_url("billing.coveragereport", "plot")."&coordinator=".$coordinator.
                  "&from=".dateUtil::arr2str($filter["from"])."&to=".dateUtil::arr2str($filter["to"]).
                  "&includeholiday=".$filter["includeholiday"], SESSION_NESTED);

        $content.= '<br/><br/><img src="'.$imgurl.'" border="0"/><br/><br/>';
      }

      $ui = &$this->getUi();
      $box = $ui->renderBox(array("title"=>atktext("coveragereport", "billing"), "content"=>$content));

      $this->addStyle("style.css");
      $page = &$this->getPage();
      $page->addContent($box);
    }

    function action_plot()
    {
      // Plot the coverage report
      include_once(moduleDir("billing")."coveragereport.inc");

      // Flush any buffered content and prevent resending headers by atkoutput
      flush();
      die();
    }
  }
?>
