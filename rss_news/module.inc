<?php
  $modifiers[] = "employee.userprefs";

  class mod_rss_news extends achievoModule 
  {
    function getMenuItems()
    {
      menuitem("rss_news_group", dispatch_url("rss_news.group", "admin"), "setup");
      menuitem("rss_news_url", dispatch_url("rss_news.url", "admin"), "setup");
    }
    
    function getNodes()
    {
      registerNode("rss_news.personal", array("admin","add","delete","edit"));
      registerNode("rss_news.url", array("admin","add","delete","edit"));
    }
    
    function getDescription()
    {
      return "RSS News Module.";
    }
    
    function getPimItems()
    {
      include_once(moduleDir("rss_news")."pim.inc");
      $ret = pimRSSNews();
      if($ret=="") 
      { 
        return "";
      }
      else 
      {
        return array("rss_news"=>$ret);
      }
    }  
    
    function userprefs_modifier(&$node)
    {
      userelation("atkOneToManyRelation");
      
      $node->add(new atkOneToManyRelation("rss_news","rss_news.personal","user_id"),"rss_news"); 
    }
  }
?>