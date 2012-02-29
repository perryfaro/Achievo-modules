<?php

   /**
    * Function which returns a unix timestamp
    * 
    * @param $sqldate is the normal date from the database [like 2005-12-31]
    * @param $sqltime is the normal time from the database [like 12:00:59]
    * @param $adjust  array with numbers to modify the date/time [hour,min,sec,month,day,year]
    * @return String The unix timestamp which was given
    */
    function timestamp($sqldate="",$sqltime="",$adjust=array("0","0","0","0","0","0"))
    {
      // return nothing when date empty
      if ($sqldate=="") {return;}
      // make pieces of the date/time
      $normaldate = str_replace("-","",$sqldate);
      $year=substr($normaldate,0,4);
      $month=substr($normaldate,4,2);
      $day=substr($normaldate,6,2);
      $normaltime = str_replace(":","",$sqltime);
      $hours=substr($normaltime,0,2);
      $minutes=substr($normaltime,2,2);
      $seconds=substr($normaltime,4,2);
      // use default time when empty
      if ($sqltime=="") { $hours=0; $minutes=0; $seconds=0;}
      // return mktime with adjustments
      return mktime($hours + $adjust[0],
                    $minutes + $adjust[1],
                    $seconds + $adjust[2],
                    $month + $adjust[3],
                    $day + $adjust[4],
                    $year+ $adjust[5]);
    }
    
    /**
    * Function which returns a number which represents a day 
    * (0=sunday | 1 = monday | 2 = tuesday | 3 = wednesday | 4 = thursday | 5 = friday | 6 = saterday)
    *
    * @param $timestamp must be a unix timestamp
    * @return Int The number which represents the day
    */
    function giveDayNumber($timestamp)
    {
    $month = date("m",$timestamp);
    $year  = date("Y",$timestamp);
    $day   = date("d",$timestamp);
    $a=(int)((14-$month) / 12); $y=$year-$a; $m=$month + (12*$a) - 2;
    $numberedday=($day + $y + (int)($y/4) - (int)($y/100) + (int)($y/400) + (int)((31*$m)/12)) % 7;
    return intval($numberedday);
    }

    /**
    * Function which returns an array with all the links that are in the specified text
    *
    * @param $text String with text
    * @return Array with all links
    */
    function extractLinks($text)
    {
      // Make the link as if in HTML so we can detect it faster!!
      $text = ereg_replace("[[:alpha:]]+://[^<>[:space:]]+[[:alnum:]/]","<a href=\"\\0\">\\0</a>",$text);
    // Now go check the text for a <A and extract the link
    $pos = 0;
    while (!(($pos = strpos($text,"<",$pos)) === false)) {
    $pos++;
    $endpos = strpos($text,">",$pos);
    $tag = substr($text,$pos,$endpos-$pos);
    $tag = trim($tag);
      if (!strcasecmp(strtok($tag," "),"A")) {
        if (eregi("HREF[ tnrv]*=[ tnrv]*\"([^\"]*)\"",$tag,$regs));
        else if (eregi("HREF[ tnrv]*=[ tnrv]*([^ tnrv]*)",$tag,$regs));
        else $regs[1] = "";
        if ($regs[1]) {
          $hrefs[] = $regs[1];
        }
        $pos = $endpos+1; $linkpos = $pos;
      } else {
        $pos = $endpos+1;
      }
    }
    return $hrefs;
    }
    
    /**
    * Decrypts the given string with the key which is given so it will be in its original form 
    * if it used encrypt function and the same key
    * 
    * @param $string The string which will be encoded
    * @param $key Key which will be used to encode the given string
    * @return String Encrypted string url friendly encoded
    */
    function decrypt($string, $key) {
    $result = '';
    $string = urldecode(base64_decode($string));

    for($i=0; $i<strlen($string); $i++) {
      $char = substr($string, $i, 1);
      $keychar = substr($key, ($i % strlen($key))-1, 1);
      $char = chr(ord($char)-ord($keychar));
      $result.=$char;
    }
    return $result;
   }
?>
