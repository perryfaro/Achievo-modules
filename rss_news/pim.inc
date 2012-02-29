<?php
include_once moduleDir("rss_news")."utils/magpierss/rss_fetch.inc";
define("MAGPIE_CACHE_ON",true);
define("MAGPIE_CACHE_DIR",atkconfig("atkroot")."achievotmp/rss_cache");
define("MAGPIE_CACHE_FRESH_ONLY",false);

function _error ($errormsg, $lvl=E_USER_WARNING) {
		global $MAGPIE_ERROR;
		
		// append PHP's error message if track_errors enabled
		if ( isset($php_errormsg) ) { 
			$errormsg .= " ($php_errormsg)";
		}
		if ( $errormsg ) {
			$errormsg = text("rss_error").": $errormsg";
			$MAGPIE_ERROR = $errormsg;
			trigger_error( $errormsg, $lvl);				
		}
}





function pimRSSNews()
{
	$db = &atkGetDb();
  $userid = atkArrayNvl(atkGetUser(), "id");
  $sql = "SELECT rss_news_personal.*,
                 rss_news_url.name as pre_name,
                 rss_news_url.url as pre_url
          FROM rss_news_personal
          LEFT JOIN rss_news_url ON (rss_news_personal.url_id=rss_news_url.id)
          WHERE user_id=".$userid;
  $nrows = $db->getRows($sql);
  if(count($nrows)==0) return "";
  
  $ret = '<table border="'.$border.'" cellspacing="0" cellpadding="2" width="100%">' . "\n";
  $ret.='<tr>';
  
  for($i=0;$i<count($nrows);$i++)
  {
    if($i%2==0 && $i!=0) $ret.="</tr><tr>";
    $url = ($nrows[$i]["url"]!=""?$nrows[$i]["url"]:$nrows[$i]["pre_url"]);
    if($nrows[$i]["refresh_time"]) define('MAGPIE_CACHE_AGE', $nrows[$i]["refresh_time"]);
    $rss = fetch_rss( $url );
    $ret.='<td valign="top" width="50%">'.display_rss($rss,$nrows[$i]).'</td>';
    if($i%2!=0) $ret.='<td valign="top" width="50%">&nbsp;</td>';
  }
  $ret.="</tr>";
  $ret.="</table>";
  return $ret;
}

function format_string($string,$max,$decodeutf8=false)
{
  if($decodeutf8) $string = utf8_decode($string);
  
	if(strlen($string)<=$max or $max<=0) {
		return($string);
	}
	
	$lastspace=strrpos($string,' ');
	$string=substr($string,0,$lastspace).'...';
	return $string;
}


function display_rss($rss,$rec)
{
  $layout = &atkInstance("atk.layout");
  $ret=$layout->data_top();
  $ret.='<tr>';
  
  if(!$rss)
  {
    $ret.=$layout->ret_td_datatitle(($rec["name"]!=""?$rec["name"]:$rec["pre_name"]));
    $ret.='</tr><tr class="row1">';
    
    $ret.=$layout->ret_td(magpie_error());
  }
  else 
  {
    $head="";
    
    if($rec["show_feed_image"] && $rss->image)
    {
      if($rss->image["link"]!="")
      {
         $head.='<a href="'.$rss->image["link"].'" target="_blank">'; 
      }
      $head.='<img src="'.$rss->image["url"].'" title="'.$rss->image["title"].'" height="'.$rss->image['height'].'" width="'.$rss->image['width'].'" border=0>';
      if($rss->image["link"]!="") $head.='</a>';
    }
    
    if($rec["show_feed_image"] && $rec["show_feed_title"]) $head.="<br>";
    
    if($rec["show_feed_title"]) $head.='<h3><a href="'.$rss->channel["link"].'" target="_blank">'.format_string($rss->channel['title'],0,$rec["decode_utf8"]).'</a></h3>';
    
    if($head!="") $ret.=$layout->ret_td_datatitle($head);
    if($rec["show_input"] && $rss->textinput)
    {
      $ret.='</tr><tr class="row1">';
      
      $inputbox=$rss->textinput["description"].'<br>
                <form action="'.$rss->textinput['link'].'" method="post">
                <input type="text" name="'.$rss->textinput['name'].'">
                <input type="submit" value="'.$rss->textinput['title'].'">
                </form>';
      
      $ret.=$layout->ret_td($inputbox);
    }
    $ret.='</tr><tr class="row1">';
    if($rec["max_items"]==0)
    {
      $items = $rss->items;
    }
    else 
    {
      $items = array_slice($rss->items, 0, $rec["max_items"]);
    }
    $items_content='';
    foreach ($items as $item) 
    {
		  $items_content.='<a href="'.$item["link"].'" target="_blank">'.format_string($item["title"],$rec["limit_item_title"],$rec["decode_utf8"]).'</a><br>';
		  if($rec["show_descriptions"]) $items_content.=format_string($item["description"],$rec["limit_description"],$rec["decode_utf8"])."<br>";
		  
	  }
    
    $ret.=$layout->ret_td($items_content);
  }
  $ret.='</tr>';
  $ret.=$layout->data_bottom();
  $ret.="<br>";
  return $ret;
}

?>