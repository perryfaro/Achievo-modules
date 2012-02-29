var modDir;
var  url = './dispatch.php?atknodetype=competency.competencymatch&atkaction=stats&atklevel=-1&atkprevlevel=0&uri=%2Fcompetency/holes'
var targetValue = 0;

/**
 * sets observers on the domain and profile select.
 */
function setObservers() {
	$('profileChoser').observe("change", profileChangeListener);
	$('domainChoser').observe("change", domainChangeListener);
}

/**
 * executed when the profile changed it will update the domain box depening of the
 * profile chosen.
 * @param e
 */
function profileChangeListener(e) {
	targetValue = e.target.value;
	fillDomainBox(e.target.value);
}

/**
 * will be executed after the domain changes and will update the grapg for domain
 * holes depening on the profile/domain chosen
 * @param e
 */
function domainChangeListener(e) {
	changeGraph($('profileChoser').value, e.target.value);
}

/**
 * will get the new graph and updates the div with this new graph
 * @param profileId
 * @param selected
 */
function changeGraph(profileId, selected) {
	if (selected != undefined) {
		var pars = 'profileId=' + profileId + "&domainId=" + selected
				+ "&type=graph";;
	} else {
		var pars = 'profileId=' + profileId + "&domainId="
				+ $('domainChoser').value + "&type=graph";
	}
	  var myAjax = new Ajax.Updater('container', url + '/_showGraph', {
		    method     : 'get',
		    parameters : pars
		  });

}

/**
 * shows a graph in the grapg container div
 * @param originalRequest
 */
function showGraph(originalRequest) {
	$('container').innerHTML = originalRequest.responseText;
}

/**
 * populates the domain select depending on a profile id
 * @param profileId the id of the profile
 */
function fillDomainBox(profileId) {
	var pars = 'profileId=' + profileId + "&type=domainBox";
	  var myAjax = new Ajax.Updater('fillBox', url + '/_fillBox', {
		    method     : 'get',
		    parameters : pars,
		    onComplete : showDomainBox
		  });
}

/**
 * shows the domain select and then changes the graph
 * @param originalRequest
 */
function showDomainBox(originalRequest) {
	la = originalRequest.responseText.evalJSON();
	i = 0;
	options = new Array();
	// we first clear the select of any options it has at the moment
	l = $('domainChoser').options.length;
	for (x = 0; x < l; x++) {
		$('domainChoser').options[0] = null;
	}

	la.each(function(e) {
		selected = e["id"];
		options.push(new Option(e["name"], e["id"]));
		$('domainChoser').options[i++] = new Option(e["name"], e["id"]);
	});
	// $('domainChoser').options = options;
	changeGraph(targetValue);
}