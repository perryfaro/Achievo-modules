<?php
  useattrib("atkdummyattribute");

  class wikiLinkAttribute extends atkDummyAttribute
  {
    var $m_prefix = "";

    function wikiLinkAttribute($prefix="", $postfix="", $flags=0)
    {
      $this->atkDummyAttribute("wiki", "", $flags);
      $this->m_prefix = $prefix;
      $this->m_postfix = $postfix;
    }

    function display($record, $mode)
    {
      $node = &$this->m_ownerInstance;
      $descriptor = $this->wikiName($record["name"]);
      return '<a href="'.atkConfig::get('wiki','wiki_server').atkConfig::get('wiki','wiki_prefix').'/'.$descriptor.'" target="_new">'.$this->text("wiki").'</a>';
    }
     
    function wikiName($name)
    {
      return $this->wikiEncode($this->m_prefix.$name.$this->m_postfix);
    }

    function wikiEncode($string)
    {
      $string = str_replace("/", "_", $string);
      $string = str_replace(" ", "_", $string);
      return rawurlencode($string);
    }
  }
?>
