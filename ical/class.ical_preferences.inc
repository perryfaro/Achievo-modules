<?php
useattrib("atkDummyAttribute");
useattrib("atkNumberAttribute");
useattrib("atkBoolattribute");
useattrib("atkListattribute");

class ical_preferences extends atkNode 
{
  function ical_preferences()
  {
    $this->atkNode("ical_preferences");
    $this->add(new atkAttribute("id",AF_AUTOKEY,10));
    
    $this->add(new atkDummyAttribute("ical_comments_1",$this->atk_text("ical_explain_edit","ical"),AF_BLANK_LABEL|AF_HIDE_LIST));
    $this->add(new atkDummyAttribute("ical_comments_hr","<hr noshade>",AF_BLANK_LABEL|AF_HIDE_LIST));
    $this->add(new atkAttribute("ical_user_id",AF_HIDE,10));
    $this->add(new atkAttribute("ical_user",AF_HIDE,200));
    $this->add(new atkAttribute("ical_user_url",AF_READONLY|AF_UNIQUE));
    $this->add(new atkDummyAttribute("ical_comments_hr_2","<hr noshade>",AF_BLANK_LABEL|AF_HIDE_LIST));
        
    $this->add(new atkBoolAttribute("ical_events"));
    $this->add(new atkBoolAttribute("ical_events_privat",AF_HIDE_LIST));
    $this->add(new atkBoolAttribute("ical_only_events_privat",AF_HIDE_LIST));
    $this->add(new atkBoolAttribute("ical_todo"));
    
    $this->add(new atkDummyAttribute("ical_comments_alarm","<br>".$this->atk_text("ical_comment_alarm","ical"),AF_BLANK_LABEL|AF_HIDE_LIST));
    $this->add(new atkBoolAttribute("ical_alarm",AF_HIDE_LIST));
    $this->add(new atkListAttribute("ical_alarm_time",array("1","2","3","4","5","6","7","8","9","10","15","20","25","30","35","40","50","55","60"),'',AF_HIDE_LIST));
    $this->add(new atkNumberAttribute("ical_alarm_repeat",AF_HIDE_LIST,'2'));
    
    $this->add(new atkDummyAttribute("ical_format_comment","<br>".$this->atk_text("ical_format_comment","ical"),AF_BLANK_LABEL|AF_HIDE_LIST));
    $this->add(new atkListAttribute("ical_format",array("iCal","vCal (only events)","xCal"),array("ics","vcs","xcs"),AF_HIDE_LIST|AF_OBLIGATORY));
    
    $this->setTable("ical_preferences");
    
    
  }
  
  function initial_values()
  {
    $user = getUser();
    return array("ical_user_id"=>$user["id"],
                 "ical_user"=>$user["userid"],
                 "ical_user_url"=>urldecode($this->encrypt($this->randomString(16),$this->randomString(5))),
                 "ical_events"=>1,
                 "ical_events_privat"=>1,
                 "ical_format"=>"iCal"); 
  }

  function ical_user_url_display(&$rec)
  {
    return $this->makeLink(urlencode($rec["ical_user_url"]));
  }
  
  /**
  * Function which makes a random string with the given lenght
  *
  * @param $len Lenght of the string which will be returned
  * @return String Randomize string
  */
  function randomString($len) {
   srand(date("s"));
   $possible="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890";
   $str="";
   while(strlen($str)<$len) {
      $str.=substr($possible,(rand()%(strlen($possible))),1);
   }
   return($str);
  } 
  
  /**
  * Encrypt the given string with the key which is given and encode it base64 and url friendly
  *
  * @param $string The string which will be encoded
  * @param $key Key which will be used to encode the given string
  * @return String Encrypted string url friendly encoded
  */
  function encrypt($string, $key) {
   $result = '';
   for($i=0; $i<strlen($string); $i++) {
     $char = substr($string, $i, 1);
     $keychar = substr($key, ($i % strlen($key))-1, 1);
     $char = chr(ord($char)+ord($keychar));
     $result.=$char;
   }
   return urlencode(base64_encode($result));
  }

 /**
  * Makes a link for requesting a ical file
  *
  * @param $token Token of a user
  * @return String Link to the ical file for the user with the given token
  */
  function makeLink($token)
  {
    global $PHP_SELF;
    $hostname = str_replace(":".$_SERVER["SERVER_PORT"], "", $_SERVER["HTTP_HOST"]);
    $path = str_replace("dispatch.php","",$PHP_SELF);
    $ical_link = "http://".$hostname.$path.moduleDir("ical")."ical.php?".$token;
    return '<a href="'.$ical_link.'" target="_blank">'.$ical_link.'</a>';
  }
  
  /**
  * Translates strings with compatibility mode
  * Checks which achievo is used and then makes the right call
  *
  * @param $string String which will be searched for in language file
  * @return String The right string for the language
  */
  function atk_text($string,$module)
  {
      if (function_exists('atktext')) {
      return atktext($string,$module);    
      } else {
      return text($string);
      }
  }
  
}
?>
