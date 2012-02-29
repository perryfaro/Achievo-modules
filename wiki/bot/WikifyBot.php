<?php
// I'm a DEMO. You can use me if you want, but you'll probably want to modify me.

// Harvests links from Special:Deadendpages. (If there's no links, then we have some evidence already that it's not wikified, right?)
// Checks each link for wikification. Adds "Needs Wikification" template if necessary.


// define these BEFORE including basicBot.php if you want to override the settings in BasicBot
//define('USERID','1');
//define('USERNAME','YourBot');
//define('PASSWORD','Password'); // password in plain text. No md5 or anything.

require_once('BasicBot.php');


// our primary callback. Checks for any wiki markup. If none found, passes $content through secondary filter 'addTemplate' (defined in BasicBot.php)
function checkWikify($content){
	if (isWikified($content))	// defined in BasicBot.php
		return $content;
	else
		return addTemplate( $content, array( 'template'=>'{{Template:Needs_Wikification}}' ) );
}


#########################################################
################ THE ACTION IS RIGHT AFTER THIS ###################
#########################################################

$wikifyBot = new BasicBot();
// Our settings
$source = 'Special:Deadendpages&limit=5000&offset=0';
$callback = 'checkWikify';
$editSummary = 'Add notice: Needs wikification';
$params = null; // don't need to pass additional parameters to our callback
$delay = 15; // seconds to wait between editing each summary
// The action
$wikifyBot->SpecialFilterAll($source,$callback,$editSummary,$params,$delay);

?>