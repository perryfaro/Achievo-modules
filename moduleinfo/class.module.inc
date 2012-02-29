<?php

  useattrib("atktextattribute");

  class module extends atkNode
  {
    var $m_invalids = false;

    function module()
    {
      $this->atkNode("module", NF_READONLY);

      $this->add(new atkAttribute("name", AF_PRIMARY));
      $this->add(new atkTextAttribute("description"));
      $this->add(new atkAttribute("version"));
      $this->add(new atkAttribute("patches"));
      $this->add(new atkAttribute("path"));

      $this->m_securityMap["readme"] = "admin";
      $this->m_securityMap["uninstall"] = "admin";
      $this->m_securityMap["install"] = "admin";
      $this->m_securityMap["patch"] = "admin";

      $this->setTable("virtual");
    }

    function countDb($where)
    {
      global $g_modules;
      if ($where!="") return 1;
      $count = 0;
      foreach ($g_modules as $module=>$path)
      {
        // Count only extension modules.
        if (substr($path, 0, 9)!="./modules")
        {
      	  $count++;
        }
      }
      return $count;
    }

    function rowColor($record)
    {
      if (!$this->isValidInstall($record))
      {
        return "#ff0000";
      }
      else if (count($record["patches"]))
      {
        return "#ff9900";
      }
    }

    function patches_display($record)
    {
      $cnt = count($record["patches"]);
      if ($cnt)
      {
        return $cnt.": #".implode(", #", $record["patches"]);
      }
      return "";
    }

    function selectDb($where="")
    {
      global $g_modules;
      $recs = array();
      foreach ($g_modules as $module=>$path)
      {
        // Show only extension modules.
        if (substr($path, 0, 9)!="./modules" && ($where=="" || $where=="virtual.name='".$module."'"))
        {
          $rec = array("name"=>$module, "path"=>$path);
          $mod = &getModule($module);
          if (is_object($mod))
          {
            if (method_exists($mod, "getDescription")) // precaution, older modules may not have this method.
            {
              $rec["description"] = $mod->getDescription();
            }
          }
          $rec["version"] = $this->getVersion($module);
          if ($rec["version"]!=NULL)
          {
            $rec["patches"]= $this->getPatches($module, $rec["version"]);
          }

          $recs[] = $rec;
        }
      }
      return $recs;
    }

    function action_patch()
    {
      $page = &$this->getPage();
      $ui = &$this->getUi();
      $theme = &atkTheme::getInstance();
      $ui = &$this->getUi();     
      $page->register_style($theme->stylePath("style.css"));
      

      $module = $this->m_postvars["module"];

      $recs = $this->selectDb("virtual.name='$module'");
      $moduleinfo = $recs[0];

      $availablepatches = $moduleinfo["patches"];

      $setup = &getNode("setup.setup");

      for($i=0, $_i=count($availablepatches); $i<$_i; $i++)
      {
        $patchfile = moduleDir($module)."install/patch-".$availablepatches[$i].".inc";
        $setup->includeInstallFile($module, $patchfile);
        $setup->setVersion($availablepatches[$i]);
        $result[$module] = "patched";
      }


      $output = $setup->renderResult($result);

      $setup->renderReport($output);

      $tmp_output=$output;

      $tmp_output.='<br><form method="get">'.session_form(SESSION_BACK).
                          '<input type="submit" value="&lt;&lt; '.atkText('back').'"></form>';

      $box = $ui->renderBox(array("title"=>atkText("patchresult", "moduleinfo","module"),
                                            "content"=>$tmp_output));
      $page->addContent($box);
    }

    function isValidInstall($record)
    {
      $install   = moduleDir($record["name"])."install/install.inc";
      if (file_exists($install) && $record["version"]<1)
      {
        // An install file is present, so there should be a version number.
        // If the versionnumber is not ok, the module is not installed
        // correctly.
        return false;
      }
      // If no install file is present, or if there's a valid version number,
      // we must assume the module is installed correctly.
      return true;
    }

    function recordActions($record, &$actions)
    {
      global $g_modules;
      $readmefile = moduleDir($record["name"])."doc/README";
      $uninstall   = moduleDir($record["name"])."install/uninstall.inc";
      if (file_exists($readmefile))
      {
        $actions["readme"] = dispatch_url("moduleinfo.module", "readme", array("module"=>$record["name"]));
      }
      if ($this->isValidInstall($record))
      {
        if (file_exists($uninstall))
        {
          $actions["uninstall"] = dispatch_url("moduleinfo.module", "uninstall", array("atkselector"=>$this->primaryKey($record)));
        }
        if (count($record["patches"]))
        {
          $actions["patch"] = dispatch_url("moduleinfo.module", "patch", array("module"=>$record["name"]));
        }
      }
      else
      {
        $this->m_invalids = true;
        $install   = moduleDir($record["name"])."install/install.inc";
        if (file_exists($install))
        {
          $actions["install"] = dispatch_url("moduleinfo.module", "install", array("module"=>$record["name"]));
        }
      }
    }

    function action_readme()
    {
      
      $page = &$this->getPage();
      $theme = &atkTheme::getInstance();
      $ui = &$this->getUi();     
      $page->register_style($theme->stylePath("style.css"));
      
      $module = $this->m_postvars["module"];
      $readmefile = moduleDir($module)."doc/README";

      $text = implode("",file($readmefile));

      $tmp_output=$this->statusbar();
      $tmp_output.='<div align="left"><pre>'.$text."</pre></div>";

      $box = $ui->renderBox(array("title"=>atkText("readme", "moduleinfo","module"),
                                            "content"=>$tmp_output));

      $page->addContent($box);
    }

    function descriptor_def()
    {
      return "[name]";
    }

    function action_uninstall()
    {
      $recs = $this->selectDb($this->m_postvars["atkselector"]);
      $module = $recs[0]["name"];

      if (!empty($this->m_postvars['confirm']))
      {
        // Confirmation page was displayed and 'yes' was clicked
        $uninstallfile = moduleDir($module)."install/uninstall.inc";

        $setup = &getNode("setup.setup");

        $currentversion = $this->getVersion($module);

        if ($currentversion>=1)
        {
          include_once($uninstallfile);
          $versioninfo = &getNode("setup.versioninfo");
          $versioninfo->deleteDb("module='$module'");
          $this->redirect();
        }

      }
      elseif (empty($this->m_postvars['cancel']))
      {
        // Confirmation page was not yet displayed
        $content = $this->confirmAction("virtual.name='$module'", "uninstall", false);

        $page = &$this->getPage();
        $page->addContent($content);
      }
      else
      {
        // Cancel was clicked.
        $this->redirect();
      }
    }

    function adminFooter()
    {
      $res = "";
      if ($this->m_invalids)
      {
        $res.= '<div align="left">'.atkText("explain_incomplete_installs","moduleinfo","module")."</div><br>";
      }

      $res.='<br>'.stringparse(atkText("checkwebsite", "moduleinfo","module"),
                   array("linkstart"=>'<a href="http://www.achievo.org/download/modules" target="_new">',
                         "linkend"=>"</a>")).'<br>';
      return $res;
    }

    function action_install()
    {
      $module = $this->m_postvars["module"];

      $setup = &getNode("setup.setup");
      $result = array();
      $setup->installModule($module, $result, false);

      $output = $setup->renderResult($result);

      $setup->renderReport($output);

      if (atkLevel()>0)
      {
        $output.= '<br><form method="get">'.session_form(SESSION_BACK).
                          '<input type="submit" value="&lt;&lt; '.atktext('back').'"></form>';
      }

      $ui = &$this->getUI();
      $boxedoutput = $ui->renderBox(array("title"=>atkText("install","moduleinfo", "module"),
                                  "content"=>$output));
      $page = &$this->getPage();
      $theme = &atkTheme::getInstance();
      $page->register_style($theme->stylePath("style.css"));
      $page->addContent($boxedoutput);
    }

    function getVersion($module)
    {
      static $s_versioninfo = array();

      if (count($s_versioninfo)==0)
      {
        $setup = &getNode("setup.setup");
        $s_versioninfo = $setup->getInstalledModules();
      }
      return $s_versioninfo[$module];
    }

    function getPatches($module, $currentversion)
    {
      // module is installed. Check if we need to patch.
      $availablepatches = array();
      if ($handle=@opendir(moduleDir($module)."install"))
      {
        while ($file = readdir($handle))
        {
          if (substr($file,0,5)=="patch")
          {
            $patchno = substr($file, 6, -4); // Patchnumber is between 'patch-' and '.inc'
            if ($patchno>$currentversion)
            {
              $availablepatches[] = $patchno;
            }
          }
        }
        closedir($handle);

        if (count($availablepatches))
        {
          // We need to install patches.
          natcasesort($availablepatches); // We must install patches in the correct order.
          return $availablepatches;
        }
      }
      return array();
    }


  }

?>