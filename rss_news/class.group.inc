<?php

class group extends atkNode 
{
  
  function group()
  {
    $this->atkNode("group",NF_MRA|NF_ADD_LINK);
    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("name",AF_OBLIGATORY));
    
    $this->setSecurityAlias("rss_news.url");
    $this->setOrder("name");
    $this->setTable("rss_news_group");
  }
  
  function descriptor_def()
  {
    return "[name]";
  }
}



?>