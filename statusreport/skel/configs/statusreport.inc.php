<?php
/*
 * The combined ‘status’ of the project, based on PM trafficlight and Achievo trafficlight.
 * This array has the following structure (one element):
 * array['PM trafficlight']['Achievo trafficlight'] = "combined ‘status’ of the project";
 *
*/

$config["traffic_light"] = array('green'=>array('green'=>'','yellow'=>'','red'=>'yellow'),
                          'yellow'=>array('green'=>'yellow','yellow'=>'yellow','red'=>'yellow'),
                          'red'=>array('green'=>'red','yellow'=>'red','red'=>'red'));
/*
 * Configuration for using traffic_light colors
*/
$config["traffic_color"] = array('green'=>'greenlight','yellow'=>'yellowlight','red'=>'redlight');