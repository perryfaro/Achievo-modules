<?PHP

userelation("atkManyToOneRelation");
class url extends atkNode 
{
  
  function url()
  {
    $this->atkNode("url",NF_MRA|NF_ADD_LINK);
    $this->add(new atkAttribute("id",AF_AUTOKEY));
    $this->add(new atkAttribute("url",AF_OBLIGATORY|AF_HIDE_LIST));
    $this->add(new atkAttribute("name",AF_OBLIGATORY));
    $this->add(new atkManyToOneRelation("group_id","rss_news.group",AF_OBLIGATORY));
    $this->setOrder("name");
    $this->setTable("rss_news_url");
  }
  
  function descriptor_def()
  {
    return "[group_id.name] / [name]";
  }
}

?>