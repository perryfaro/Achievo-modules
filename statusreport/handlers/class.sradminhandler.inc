<?php
/**
   * This file is part of the Achievo ATK distribution.
   * Detailed copyright and licensing information can be found
   * in the doc/COPYRIGHT and doc/LICENSE files which should be
   * included in the distribution.
   *
   * @package atk
   * @subpackage handlers
   *
   * @copyright (c)2000-2004 Ivo Jansch
   * @copyright (c)2000-2004 Ibuildings.nl BV
   * @license http://www.achievo.org/atk/licensing ATK Open Source License
   *
   * @version $Revision: 6310 $
   * $Id: class.atkadminhandler.inc 6793 2010-03-18 12:25:11Z boy $
   */

/**
   * Handler for the 'admin' action of a node. It displays a recordlist with
   * existing records, and links to view/edit/delete them (or custom actions
   * if present), and an embedded addform or a link to an addpage (depending
   * on the presence of the NF_ADD_LINK or NF_ADD_DIALOG flag).
   *
   * @author Ivo Jansch <ivo@achievo.org>
   * @package atk
   * @subpackage handlers
   *
   */
atkimport("atk.handlers.atkadminhandler");

class srAdminHandler extends atkAdminHandler
{
  /**
     * Generic method invoker.
     *
     * Handler methods invoked with invoke() instead of directly, have a major
     * advantage: the handler automatically searches for an override in the
     * node. For example, If a handler calls its getSomething() method using
     * the invoke method, the node may implement its own version of
     * getSomething() and that method will then be called instead of the
     * original. The handler is passed by reference to the override function
     * as first parameter, so if necessary, you can call the original method
     * from inside the override.
     *
     * The function accepts a variable number of parameters. Any parameter
     * that you would pass to the method, can be passed to invoke(), and
     * invoke() will pass the parameters on to the method.
     *
     * There is one limitation: you can't pass parameters by reference if
     * you use invoke().
     *
     * <b>Example:</b>
     *
     * <code>
     *   $handler->invoke("editPage", $record, $mode);
     * </code>
     *
     * This will call editPage(&$handler, $record, $mode) on your node class
     * if present, or editPage($record, $mode) in the handler if the node has
     * no override.
     *
     * @param String $methodname The name of the method to call.
     * @return mixed The method returns the return value of the invoked
     *               method.
     */
  function invoke($methodname)
  {
    if($methodname != 'adminFooter')
    {
      return parent::invoke($methodname);
    }

    $arguments = func_get_args(); // Put arguments in a variable (php won't let us pass func_get_args() to other functions directly.
    // the first argument is $methodname, which we already defined by name.
    array_shift($arguments);

    return call_user_func_array(array(&$this, $methodname), $arguments);
  }

  public function adminFooter()
  {
    $flagged = self::getView("flagged");
    $res = parent::invoke('adminFooter');

    if ($flagged=="flagged")
    {
      $res .= atktext("show_all_flagged_project","statusreport")." ".href(dispatch_url("project.project",$this->m_action,array("flagged"=>"nonflagged")),
                                                    atktext('clicktoviewall',"statusreport"))."<br>";
    }
    elseif($flagged=="nonflagged")
    {
      $res .= atktext("show_all_project","statusreport")." ".href(dispatch_url("project.project",$this->m_action,array("flagged"=>"flagged")),
                                                    atktext('clicktoviewflagged',"statusreport"))."<br>";
    }
    return $res;
  }

  /**
     * The action method
     */
  function action_admin()
  {
    $flagged = self::getView("flagged");

    if ($flagged == "flagged")
    {
      $filter = "1=0";
      $ids = statusreportutil::getFlaggedProjectid();

      if(!empty($ids))
      {
        $filter = "project.id IN(".implode(",",$ids).")";
      }
      $this->getNode()->addFilter($filter);
    }
    return parent::action_admin();
  }

  static function getView($varname)
  {
    $sessionManager = &atkGetSessionManager();

    /**
     * "flagged" must be present in stack. If not - we don't come from PIM page,
     * so we haven't show "flagged porject" link.
     */
    $value = $sessionManager->stackVar($varname);

    return $value;
  }

}