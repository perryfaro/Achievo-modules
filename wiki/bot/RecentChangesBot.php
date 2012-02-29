<?php
// I'm a DEMO bot. You don't have to use this file unless you want to.

// Runs recently changed summaries through some filters.
// Just run me once each day or so--less often on less busy sites.

// define these BEFORE including BasicBot.php if you want to override the settings in BasicBot.php:
//define('USERID','2');
//define('USERNAME','RCBot');
//define('PASSWORD','RCBotPassword');

require_once('BasicBot.php');

// our callback. We'll run all pages that have been changed recently (i.e. since our last check) through these functions.
function RecentChangesPatrol($content){
	if (0===stripos( $content, '#REDIRECT [[') ) // don't do anything if the page is a redirect
		return $content;
	if (strlen($content) < 100) // very short articles. Don't run through additional filters.
		return addTemplate( $content, array( 'template'=>'{{Template:Speedy_Deletion}}' ) );
	if (strlen($content) < 500) // always run this check first, since other checks lengthen $content by adding templates and categories to it
		$content = addTemplate( $content, array( 'template'=>'{{Template:Needs_Expansion}}' ) );
	if (checkBadWords($content)) // check for profanity
		$content = addCategory( $content, array( 'cat'=>'Bot_Flag/Possible_Vandalism' ) );
	if (!isWikified($content))	// check for wikification
		$content = addTemplate( $content, array( 'template'=>'{{Template:Needs_Wikification}}' ) );
	return $content;
}


#########################################################
################ THE ACTION IS RIGHT AFTER THIS ###################
#########################################################

// Our settings
$callback = 'RecentChangesPatrol';
$editSummary = '';

// The action
$recentBot = new BasicBot();
$recentBot->FilterRecentChanges($callback,$editSummary);

?>