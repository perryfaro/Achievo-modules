<?php
useattrib("atkDummyAttribute");
useattrib("atkNumberAttribute");
useattrib("atkBoolattribute");
userelation("atkManyToOneRelation");

class personal extends atkNode 
{

  function personal()
  {
    $this->atkNode("personal",NF_MRA|NF_ADD_LINK);
    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("url"));
    $this->add(new atkAttribute("name"));
    $this->add(new atkManyToOneRelation("url_id","rss_news.url",AF_FORCE_LOAD));
    $this->add(new atkAttribute("user_id",AF_HIDE));
    $this->add(new atkDummyAttribute("comment2",text("help_refresh_time"),AF_HIDE_LIST));
    $this->add(new atkNumberAttribute("refresh_time",AF_HIDE_LIST));
    $this->add(new atkBoolAttribute("show_feed_title",AF_HIDE_LIST));
    $this->add(new atkBoolAttribute("show_feed_image",AF_HIDE_LIST));
    $this->add(new atkBoolAttribute("show_descriptions",AF_HIDE_LIST));
    $this->add(new atkBoolAttribute("show_input",AF_HIDE_LIST));
    $this->add(new atkDummyAttribute("comment1",text("help_max_items"),AF_HIDE_LIST));
    $this->add(new atkNumberAttribute("max_items"));
    $this->add(new atkDummyAttribute("comment3",text("help_limit"),AF_HIDE_LIST));
    $this->add(new atkNumberAttribute("limit_item_title",AF_HIDE_LIST));
    $this->add(new atkNumberAttribute("limit_description",AF_HIDE_LIST));
    $this->add(new atkBoolAttribute("decode_utf8",AF_HIDE_LIST));    
    
    $user = getUser();
    $this->addFilter("user_id=".$user["id"]);
    $this->setTable("rss_news_personal");
  }
  
  function initial_values()
  {
    $user = getUser();
    return array("refresh_time"=>14400,
                 "max_items"=>0,
                 "limit_item_title"=>0,
                 "item_title_longer_then"=>0,
                 "limit_description"=>0,
                 "description_longer_then"=>0,
                 "show_feed_title"=>true,
                 "user_id"=>$user["id"]); 
  }
  
  function number_of_items_display(&$rec)
  {
    if($rec["number_of_items"]==0) return text("show_all"); 
    return $rec["number_of_items"];
  }
  
  function validate(&$record,$mode)
  {
    if ($record["url"]=="" && $record["name"]=="" && $record["url_id"]["id"]=="") 
    {
       $attr = &$this->m_attribList["url_id"];
       $attr->m_flags|=AF_OBLIGATORY;
    }
    return parent::validate($record, $mode);
  }

  function preAdd(&$rec)
  {
    if($rec["name"]!="" && $rec["url"]!="") $rec["url_id"]["id"]="";
  }
  
  function preUpdate(&$rec)
  {
    if($rec["name"]!="" && $rec["url"]!="") $rec["url_id"]["id"]="";
  }
}
?>
