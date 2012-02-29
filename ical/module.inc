<?php

  $modifiers[] = "employee.userprefs";
  
  class mod_ical extends achievoModule
  {
    function getNodes()
    {
      registerNode("ical.ical_preferences", array("add","delete","edit"));
    }

    function userprefs_modifier(&$node)
    {
      // Language override is only nessacery with achievo 1.2 and higher
      if (function_exists(atkimport)) {   
        atkimport("atk.atklanguage");
        $al = &atkLanguage::getInstance();
        $al->addOverrideModule("ical");
      }
      
      userelation("atkOneToManyRelation");
      useattrib("atkDummyAttribute");      
      $user = getUser();
      
      $icalnode = &getNode("ical.ical_preferences");      
      $token = $icalnode->encrypt($user['id'].":".str_replace(" ",":",$user['firstname'].$user['lastname']),atkconfig('auth_accountenableexpression'));
      $link = $icalnode->makeLink($token);
      
      $node->add(new atkDummyAttribute("ical_explain_url_",$icalnode->atk_text("ical_explain_url","ical").$link.$icalnode->atk_text("ical_explain_url_after","ical"),AF_BLANK_LABEL),"ical");
      
      if ($icalnode->allowed('add'))
      {
      $node->add(new atkDummyAttribute("ical_explain_own_url",$icalnode->atk_text("ical_explain_own_url","ical"),AF_BLANK_LABEL),"ical");
      $node->add(new atkOneToManyRelation("ical","ical.ical_preferences","ical_user_id",AF_BLANK_LABEL),"ical"); 
      }
    }
  }

?>
