<?php
  /**
   * legalprojectdoctype attribute class file
   *
   * @package achievo_modules
   * @subpackage legal
   *
   * @author guido <guido@ibuildings.nl>
   *
   * @copyright (c) 2006 Ibuildings.nl BV
   * @license http://www.gnu.org/copyleft/gpl.html GNU General Public License 2
   *
   * @version $Revision: 3233 $
   * $Id: class.legalprojectdoctype.inc 3233 2006-07-07 18:33:06Z guido $
   */

  // Load used classes
  useattrib("atklistattribute");

  /**
   * The attribute class for legalprojectdoctype.
   *
   * @author guido <guido@ibuildings.nl>
   * @package achievo_modules
   * @subpackage legal
   */
  class legalprojectdoctype extends atkListAttribute
  {
    /**
     * legalprojectdoctype constructor
     */
    function legalprojectdoctype($name, $flags=0, $size=0)
    {
      $legalmodule = &getModule("legal");
      $typevalues = $legalmodule->getLegalDocTypes();
//      $typeoptions = array();
//      foreach ($typevalues as $id => $typevalue)
//        $typeoptions[] = atktext($typevalue, "legal");

      $this->atkListAttribute($name, array_values($typevalues), array_keys($typevalues), $flags, $size);
    }

    function edit($record="", $fieldprefix="")
    {
      $page = &$this->m_ownerInstance->getPage();
      $legalmodule = &getModule("legal");
      $masterattr = &$this->m_ownerInstance->getAttribute("master");

      $page->register_loadscript($legalmodule->getprojectdoctypevisibilityscript());
      $page->register_script(atkconfig("atkroot")."atk/javascript/dhtml_formtools.js");
      return parent::edit($record, $fieldprefix);
    }

    function value2db($rec)
    {
      if (is_array($rec) && (atkArrayNvl($rec, "master")=="project.project_personcontact"))
      {
        return parent::value2db($rec);
      }
      return NULL;
    }
  }
?>