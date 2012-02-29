<?php
  require_once(moduleDir('wiki').'bot/BasicBot.php');

  class AchievoBot
  {
    private $_bot;

    public function __construct()
    {
      $this->_bot = new BasicBot();
    }

    public function updatePage($pagename, $callbackfunction, $comment, $params)
    {
      $params['callback'] = $callbackfunction;
      $this->_bot->wikiFilter( $pagename, 'achievobot_processText', $comment, $params );
    }
  }

  function achievobot_processText($content, $params)
  {
    echo "|\n";
    $func = $params['callback'];
    $result = $func($params);
    return achievobot_injectText($content, $result);
  }
  
  function achievobot_injectText($str, $injectable)
  {
  	if (strpos($str, "<BOT")===false)
  	{
  		return $str."\n\n<!--<BOT MARKER>; don't change anything from here -->\n".$injectable."\n<!--</BOT MARKER> .. to here, or I'll damage it. -->\n";
  	}
  	else
  	{
	  	$result = "";
	  	$lines =  explode("\n", $str);
	  	foreach($lines as $line)
	  	{
	  	  if (strpos($line, "<BOT")!==false)
	  	  {
	  	  	$result.=$line."\n";
	  	  	$bot_managed = true;
	  	  }
	  	  if (strpos($line, "</BOT")!==false)
	  	  {
	  	  	$bot_managed = false;
	  	  	
	  	  	// end of bot managed stuff. Insert fluff first
	  	  	$result.="$injectable\n";
	  	  }
	  	  
	  	  if (!$bot_managed)
	  	  {
	  	  	$result.=$line."\n";
	  	  }
	  	}
  	}
  	
     return $result;
  }
