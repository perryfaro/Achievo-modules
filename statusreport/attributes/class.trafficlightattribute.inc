<?php

  define('IMG_TRAFFIC_GREEN',   moduleDir('statusreport').'/images/traffic_green.png');
  define('IMG_TRAFFIC_YELLOW',  moduleDir('statusreport').'/images/traffic_yellow.png');
  define('IMG_TRAFFIC_RED',     moduleDir('statusreport').'/images/traffic_red.png');
  define('IMG_TRAFFIC_UNKNOWN', moduleDir('statusreport').'/images/traffic_unknown.png');

  define('IMG_BULLET_GREEN',   moduleDir('statusreport').'/images/bullet_green.png');
  define('IMG_BULLET_YELLOW',  moduleDir('statusreport').'/images/bullet_yellow.png');
  define('IMG_BULLET_RED',     moduleDir('statusreport').'/images/bullet_red.png');
  define('IMG_BULLET_UNKNOWN', moduleDir('statusreport').'/images/bullet_unknown.png');


  class trafficLightAttribute extends atkAttribute
  {

  	/**
  	 * Constructor
  	 *
  	 * @param String $name
  	 * @param Integer $flags
  	 */
    public function __construct($name, $flags=0)
    {
      $this->atkAttribute($name, $flags);
    }


    /**
     * Display override for traffic light
     *
     * @param Array $record
     * @param String $mode The display mode ("view" for viewpages, or "list"
     *                     for displaying in recordlists, "edit" for
     *                     displaying in editscreens, "add" for displaying in
     *                     add screens. "csv" for csv files. Applications can
     *                     use additional modes.
     * @return String html
     */
    public function display( $record, $mode = '')
    {
      switch($mode)
      {
        case "view":
        case "edit":
        case "add":
          $res = $this->getViewImage($record, $img, $text);
          break;
        case "list":
          $res = $this->getListImage($record);
          break;
        default:
          $res = $record[$this->fieldName()];

      }
      return $res;
    }

    private function getViewImage($record, &$img, &$text)
    {
    	switch($record[$this->fieldName()])
    	{
    	  case 'red':
    	    $img = IMG_TRAFFIC_RED;
    	    $text = atktext('trafficlight_red');
    	    break;
    	  case 'yellow':
    	    $img = IMG_TRAFFIC_YELLOW;
    	    $text = atktext('trafficlight_yellow');
    	    break;
    	  case 'green':
    	    $img = IMG_TRAFFIC_GREEN;
    	    $text = atktext('trafficlight_green');
    	    break;
    	  default:
    	    $img = IMG_TRAFFIC_UNKNOWN;
    	    $text = atktext('trafficlight_unknown');
    	     break;
    	}
    	return '<img src="'.$img.'" alt="'.$text.'" />';
    }

    private function getListImage($record)
    {
    	switch($record[$this->fieldName()])
    	{
    	  case 'red':
    	    $img = IMG_BULLET_RED;
    	    $text = atktext('trafficlight_red');
    	    break;
    	  case 'yellow':
    	    $img = IMG_BULLET_YELLOW;
    	    $text = atktext('trafficlight_yellow');
    	    break;
    	  case 'green':
    	    $img = IMG_BULLET_GREEN;
    	    $text = atktext('trafficlight_green');
    	    break;
    	  default:
    	    $img = IMG_BULLET_UNKNOWN;
    	    $text = atktext('trafficlight_unknown');
    	     break;
    	}
    	return '<img src="'.$img.'" alt="'.$text.'" />';
    }

    /**
     * Ensure the attribute will be updated
     *
     * @return unknown
     */
    public function needsUpdate()
    {
      return true;
    }


    /**
     * Generate trafficlight based on configuration options
     *
     * @param Array $record
     * @param String $mode
     * @return String html
     */
    public function edit($record="", $fieldprefix="", $mode="")
    {
    	$page = &$this->getOwnerInstance()->getPage();
    	$name = $this->m_name;
    	$page->register_scriptcode("function updatelight(code,id,imgname)
    	{
    	 $(id).value = code;
    	 $(id+'_img').src=imgname;
    	}");

    	$img = $text="";
      $this->getViewImage($record, $img, $text);

    	return $this->hide($record, $fieldprefix).
    	       '<img id="'.$name.'_img" src="'.$img.'" alt="'.$text.'" width="80" height="32" usemap="#'.$name.'_map"/>
                <map id="'.$name.'_map" name="'.$name.'_map">
                  <area shape="circle" coords="16,16,11" alt="Red" onClick="updatelight(\'red\',\''.$name.'\',\''.IMG_TRAFFIC_RED.'\')">
                  <area shape="circle" coords="40,16,11" alt="Yellow" onClick="updatelight(\'yellow\',\''.$name.'\',\''.IMG_TRAFFIC_YELLOW.'\')">
                  <area shape="circle" coords="64,16,11" alt="Green" onClick="updatelight(\'green\',\''.$name.'\',\''.IMG_TRAFFIC_GREEN.'\')">
                </map>';
    }


  }