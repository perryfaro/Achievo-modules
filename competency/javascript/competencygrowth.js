/**
 * This function expands or collapses the divs when there is pushed on the
 * plus/minus icon and changes the icon
 *
 * @param (string)
 *            div id of the div we want to collapse/expand
 * @param (img)
 *            picture id of the icon we want to change
 */
function expand(div, img) {
	if ($(img).src == ("http://" + location.hostname + "/atk/images/plus.gif")) {
		$(img).src = "./atk/images/minus.gif";
	} else {
		$(img).src = "./atk/images/plus.gif";
	}
	$(div).toggle();
}

/**
 *  sets observers on the scroll of the page so it will scroll the legenda window
 */
function setObservers() {
	document.observe("scroll", onScrollEvent);
}

/**
 * IS executed after the document is scrolled and will move the legenda window
 * @param e
 */
function onScrollEvent(e) {
	positionX = document.viewport.getDimensions().width - 350;

	offsets = document.viewport.getScrollOffsets();
	$('legenda').style.top = (offsets[1] + 100) + "px";
	$('legenda').style.left = (positionX) + "px";
}

/**
 * turns the legenda on or off depening on its current state
 */
function toggleLegenda() {
	$('legenda').toggle();
}

/**
 * initializes the position of the legenda
 */
function initLegenda() {
	positionX = document.viewport.getDimensions().width - 350;
	offsets = document.viewport.getScrollOffsets();
	$('legenda').style.top = (offsets[1] + 100) + "px";
	$('legenda').style.left = (positionX) + "px";
}