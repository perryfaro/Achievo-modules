var modDir;
var domain;
var competency;
var niveau;
var  url = './dispatch.php?atknodetype=competency.competencycompanyview&atkaction=admin&atklevel=-1&atkprevlevel=0&uri=%2Fcompetency/company'

/**
 * We set all the observers we need to fire our actions
 * @return
 */
function setObservers() {
  selects = $A($$('.atkManyToOneRelation'));
  selects.each(function(element)
  {
    switch (element.name)
    {
      case "atksearch_AE_competency_id_AE_competency_id[]":
      element.observe("change", competencyChangeListener);
      competency = element;
      break;
      case "atksearch_AE_competency_id_AE_domain_id_AE_domain_id[]":
      element.observe("change", domainChangeListener);
      domain = element;
      break;
      case "atksearch_AE_niveau_id_AE_niveau_id[]":
      niveau = element;
      break;
    }
  });
}

/**
 * We update our list of competences after we select a domain
 * @param domain the domain we selected
 * @return
 */
function updateCompetencesList(domain) {
  var pars = 'domain=' + domain + "&type=competency";

  var myAjax = new Ajax.Updater('_updateselect', url + '/_updateSelect', {
      method     : 'get',
      parameters : pars,
    onComplete : showDomainBox
    });
}

/**
 * We update our list of levels after we selected a domain and or competency
 * @param domain the selected domain
 * @param competency the selected competency
 * @return
 */
function updateNiveauList(domain, competency) {
  var pars = 'domain=' + domain + "&competency=" + competency
      + "&type=niveau";

  var myAjax = new Ajax.Updater('_updateselect', url + '/_updateSelect', {
      method     : 'get',
      parameters : pars,
      onComplete : showNiveauList
    });
}

/**
 * We transform our json string to a dorpdown box for the niveaus
 * @param originalRequest teh json we recieved
 * @return
 */
function showNiveauList(originalRequest) {
  la = originalRequest.responseText.evalJSON();
  i = 0;
  options = new Array();
  // we first clear the select of any options it has at the moment
  l = niveau.options.length;
  for (x = 0; x < l; x++) {
    niveau.options[0] = null;
  }
  niveau.options[i++] = new Option("Search all", "");
  la.each(function(e) {
    selected = e["id"];
    options.push(new Option(e["name"], e["id"]));
    niveau.options[i++] = new Option(e["name"], e["id"]);
  });

}

/**
 * We transform our json string to a dorpdown box for the domains
 * @param originalRequest teh json we recieved
 * @return
 */
function showDomainBox(originalRequest) {
  la = originalRequest.responseText.evalJSON();
  i = 0;
  options = new Array();
  // we first clear the select of any options it has at the moment
  l = competency.options.length;
  for (x = 0; x < l; x++) {
    competency.options[0] = null;
  }

  competency.options[i++] = new Option("Search all", "");
  la.each(function(e) {
    selected = e["id"];
    options.push(new Option(e["name"], e["name"]));
    competency.options[i++] = new Option(e["name"], e["name"]);
  });
}

/**
 * What do we do when the domain select changes
 * @param e the event
 * @return
 */
function domainChangeListener(e) {
  updateCompetencesList(e.target.value);
  updateNiveauList(e.target.value);
}

/**
 * what do we do when the competency select changed
 * @param e the event
 * @return
 */
function competencyChangeListener(e) {
  updateNiveauList(domain.value, e.target.value);
}

/**
 *
 * @param e
 * @return
 */
function showChangeListener(e) {
  $("hiddenShow").value=e.target.value;
  $("hiddenOffset").value=0;
    document.navigator.submit();
}

/**
 * MIght not ne used anymore check this
 * @param id
 */
function nav(id)
{
    $("hiddenOffset").value=id;
      document.navigator.submit();
}