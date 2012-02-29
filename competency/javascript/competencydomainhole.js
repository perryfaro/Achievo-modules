var modDir = "";
var  url = './dispatch.php?atknodetype=competency.competencymatch&atkaction=stats&atklevel=-1&atkprevlevel=0&uri=%2Fcompetency/domainholes'

/**
 * set the observes on the dom elements we have that need a onchange handler
 */
function setObservers()
{
    //handler for the profile change.
  $('profileChoser').observe("change", profileChangeListener);
}

/**
 * Wil be extecuted of the profile changes and loads the graph for the new
 * proile chosen
 * @param e
 */
function profileChangeListener(e)
{
  changeGraph(e.target.value);
}

/**
 * changes the competency domain hole graph depending on what profile changed
 * @param profileId
 */
function changeGraph(profileId)
{
  var pars = 'profileId=' + profileId;
  var myAjax = new Ajax.Updater('container', url + '/_showGraph', {
	    method     : 'get',
	    parameters : pars,

	  });
}

/**
 * chows a graph in the container div
 * @param originalRequest
 */
function showGraph(originalRequest)
{
  $('container').innerHTML = originalRequest.responseText;
}

/**
 * changes the url to an url with the chosen domain and profile
 * @param domain
 * @param profile
 */
function changeUrl(domain,profile)
{
	url = 'dispatch.php?atknodetype=competency.competencyholes&atkaction=stats&atklevel=1&atkprevlevel=0&domain='+domain+"&profile="+profile;
//	alert(domain + " " + profile);
	top.frames["main"].location = url;
}

/**
 * fills the domain selector with the domains available
 * @param originalRequest
 */
function showDomainBox(originalRequest)
{
  la = originalRequest.responseText.evalJSON();
  i = 0;
  options = new Array();
  //we first clear the select of any options it has at the moment
  l = $('domainChoser').options.length;
  for (x = 0; x < l; x++)
  {
    $('domainChoser').options[0] = null;
  }

  la.each(function(e)
  {
    selected = e["id"];
    options.push(new Option(e["name"], e["id"]));
    $('domainChoser').options[i++] = new Option(e["name"], e["id"]);
  });
  // $('domainChoser').options = options;
  changeGraph(targetValue);
}