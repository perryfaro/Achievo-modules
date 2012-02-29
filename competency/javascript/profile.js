var divStat = new Array();
var active;
var profile = 0
var calendar;
var compId;
var currentDate = new Date();
var scheduleDate = currentDate.getFullYear() + "-"
    + (currentDate.getMonth() + 4) + "-" + currentDate.getDate();
var deleteMessage;
var infoBuffer = $H();
var lng = "";
var modDir= "";
var collapsed = $H();
var addMenuItems;
var deleteMenuItems;
var url = './dispatch.php?atknodetype=competency.competencymatch&atkaction=stats&atklevel=-1&atkprevlevel=0&uri=%2Fcompetency/profile'
var allowedAdd = false;

/**
 * adds the context menu used to add/delete competences to a profile
 * @param menuAdd
 * @param menuDelete
 */
function addMenu(menuAdd, menuDelete) {
  addMenuItems = [{
    name : menuAdd,
    className : 'add',
    callback : function(e) {
      sendData(e.target.id);
      initContextMenus();
    }
  }]
  deleteMenuItems = [{
    name : menuDelete,
    className : 'Delete',
    callback : function(e) {
      confirmDelete(e.target.id, deleteMessage);
      initContextMenus();
    }
  }]
}
/**
 * turns divs on or off
 */
function toggleDivs()
{
  tempCollapsed = collapsed;
  collapsed = $H();
  tempCollapsed.each(function(pair){
  expand(pair.key,"img_"+pair.key)
  } )
}

/**
 * initializes all context menus
 * Uses protomenu
 */
function initContextMenus() {
 if(!allowedAdd)
  {
   return false;
  }
  new Proto.Menu({
    selector : '.level',
    className : 'menu desktop',
    menuItems : deleteMenuItems,
    beforeShow : function(e) {
      var paras = $A($$('div.menu'));
      paras.each(Element.hide);
    }
  });
  new Proto.Menu({
    selector : '.competency',
    className : 'menu desktop',
    menuItems : deleteMenuItems,
    beforeShow : function(e) {
      var paras = $A($$('div.menu'));
      paras.each(Element.hide);
    }
  });
  new Proto.Menu({
    selector : '.level_NA',
    className : 'menu desktop',
    menuItems : addMenuItems,
    beforeShow : function(e) {
      var paras = $A($$('div.menu'));
      paras.each(Element.hide);
    }
  });
  new Proto.Menu({
    selector : '.competency_NA',
    className : 'menu desktop',
    menuItems : addMenuItems,
    beforeShow : function(e) {
      var paras = $A($$('div.menu'));
      paras.each(Element.hide);
    }
  });

  new Proto.Menu({
    selector : '.level_IP',
    className : 'menu desktop',
    menuItems : deleteMenuItems,
    beforeShow : function(e) {
      var paras = $A($$('div.menu'));
      paras.each(Element.hide);
    }
  });
  new Proto.Menu({
    selector : '.competency_IP',
    className : 'menu desktop',
    menuItems : deleteMenuItems,
    beforeShow : function(e) {
      var paras = $A($$('div.menu'));
      paras.each(Element.hide);
    }
  });
  new Proto.Menu({
    selector : '.level_NA_IP',
    className : 'menu desktop',
    menuItems : addMenuItems,
    beforeShow : function(e) {
      var paras = $A($$('div.menu'));
      paras.each(Element.hide);
    }
  });
  new Proto.Menu({
    selector : '.competency_NA_IP',
    className : 'menu desktop',
    menuItems : addMenuItems,
    beforeShow : function(e) {
      var paras = $A($$('div.menu'));
      paras.each(Element.hide);
    }
  });
}

/**
 * We use this to hide certain things we we dont need when javascrips is
 * enabled but we do need when teher is no javascript present
 */
function noScript() {
  arr = $A($$('div.noScript'));
  arr.each(Element.hide);
}

/**
 * Event listener for the profile tabs it will change the user id call an ajax
 * function to reload data in the competences area and higlight the now chosen
 * profile
 *
 * @param (event)
 *            e the mouse event
 */
function changeProfile(e) {
  var id = e.target.value;
  profile = id;
  updateCompetencesWithProfile(id);
}

/**
 * called in the beginning is used to set the beginning observers in this page
 */
function setObserv() {
  document.observe("scroll", onScrollEvent);
  updateObservers();
}

/**
 * On change of the always show checkbox this function will check if we need to remove the
 * always show or add it.
 * @param e observer
 * @return void
 */
function changeCheck(e)
{
  if(e.target.checked)
  {
    e.target.next().disabled = true;
    addChecked(e.target.value);
  }
  else
  {
    e.target.next().disabled = false;
    removeChecked(e.target.value);
  }
}

/**
 * On change of the extra checbox we will ad a competency that normaly doesnt show in the list but still belongs to the
 * profile
 * @param e observer
 * @return void
 */
function changeExtra(e)
{
  if(e.target.checked)
  {
    e.target.previous().disabled = true;
    addExtra(e.target.value);
  }
  else
  {
    e.target.previous().disabled = false;
    removeExtra(e.target.value);
  }
}

/**
 * Adds an always show to a competency
 * @param competency the competency we want to add a always show for
 * @return void
 */
function addChecked(competency)
{
  var pars = 'type=addAlwaysShow&compId=' + competency + '&profile=' + $("profileChoser").value;
   var myAjax = new Ajax.Updater('_updateActions', url + '/_updateActions', {
        method     : 'get',
        parameters : pars,
      });
}

/**
 * Removes an always show for a competency
 * @param competency the competency to remove the always show for
 * @return
 */
function removeChecked(competency)
{
  var pars = 'type=removeAlwaysShow&compId=' + competency + '&profile=' + $("profileChoser").value;
   var myAjax = new Ajax.Updater('_updateActions', url + '/_updateActions', {
        method     : 'get',
        parameters : pars,
      });
}

/**
 * Adds an extra competency to the profile
 * @param competency the extra competency we want to add to the profile
 * @return void
 */
function addExtra(competency)
{
  var pars = 'type=addExtra&compId=' + competency + '&profile=' + $("profileChoser").value;
   var myAjax = new Ajax.Updater('_updateActions', url + '/_updateActions', {
        method     : 'get',
        parameters : pars,
      });
}

/**
 * Removes an extra competency for a profile
 * @param competency the extra competency to remove for the profile
 * @return void
 */
function removeExtra(competency)
{
  var pars = 'type=removeExtra&compId=' + competency + '&profile=' + $("profileChoser").value;
   var myAjax = new Ajax.Updater('_updateActions', url + '/_updateActions', {
        method     : 'get',
        parameters : pars,
      });
}

/**
 * When we reload the competency data we will lose our observers So after we
 * change the competency data we will call this function to make observers of
 * the new tabs
 */
function updateObservers() {
  var check = $A(document.getElementsByClassName('check'));
  check.each(function(element) {
    element.observe("change", changeCheck);
  });

  var check = $A(document.getElementsByClassName('extra'));
  check.each(function(element) {
    element.observe("change", changeExtra);
  });

  var profileChoser = $A(document.getElementsByClassName('profileChoser'));
  profileChoser.each(function(element) {
    element.observe("change", changeProfile);
  });

  var competences = $A($$('.competency',
          '.competency_NA',
          '.level',
          '.level_NA',
          '.level',
          '.competency_IP',
          '.competency_NA_IP',
          '.level_IP',
          '.level_NA_IP'
              ));

  if(allowedAdd)
  {
    competences.each(function(element)
    {
      element.observe("mouseover", onMouseOverComp);
      element.observe("mouseout", onMouseOverComp);
    });
  }
}

/**
 * Highlight the selected person tab so we can easaly see who we are working
 * with
 *
 * @param (integer) id of the div we want to higlight
 */
function highlightSelectedProfile(id) {
  // we select the now active person and set it to non active mode
  var profilesActive = $A(document.getElementsByClassName('profileActive'));
  for (var i = 0; i < profilesActive.length; i++) {
    profilesActive[i].removeClassName('profileActive');
    profilesActive[i].addClassName('profile');
  }
  // we get the element we wan to higlight
  var element = document.getElementById(id); // get the element
  element.removeClassName('profile');
  element.addClassName('profileActive');

}

// to see after a div refresh what the status is of the div
// 0 is normal state and 1 means its collapsed
function initDivStatus() {
  // first we check the domains
  var domains = document.getElementsByClassName('domainborderless');
  for (var i = 0; i < domains.length; i++) {
    divStat[domains[i].id] = 0;
  }
}

/**
 * This is the methode that takes care of changing all the colors/styles of
 * competences on the fly this makes that we dont have to refresh teh screen all
 * the time but all the operations on teh database are executed in the
 * bacjground while java takes care of teh visual part in the foreground
 *
 * @param (integer) compId competency to change color of
 * @param (String) type the type of what we are going to do (choices ADD or REMOVE)
 */
function changeCompetencyColors(compId, type) {
  var element = $(compId);
  if (type == "ADD") {
    var sibArray = element.previousSiblings();

    sibArray.each(function(n) {
      if (n.className != 'level' || n.className != 'level_IP') {
        if (n.className == 'level_NA') {
          n.removeClassName('level_NA');
          n.addClassName('level');
        }
        if (n.className == 'level_NA_IP') {
          n.removeClassName('level_NA_IP');
          n.addClassName('level_IP');
        }
      }
    });

    if (element.className == 'level_NA') {
      element.removeClassName('level_NA');
      element.addClassName('level');

    } else if (element.className == 'competency_NA') {
      element.removeClassName('competency_NA');
      element.addClassName('competency');
    } else if (element.className == 'level_NA_IP') {
      element.removeClassName('level_NA_IP');
      element.addClassName('level_IP');

    } else {
      element.removeClassName('competency_NA_IP');
      element.addClassName('competency_IP');
    }
  } else if (type == "REMOVE") {
    var sibArray = element.nextSiblings();
    sibArray.each(function(n) {
      // we run trough all higherlaying siblings and set them on nonactive
      if (n.className != 'level_NA' || n.className == 'level_NA_IP') {
        if (n.className == 'level') {
          n.removeClassName('level');
          n.addClassName('level_NA');
        }
        if (n.className == 'level_IP') {
          n.removeClassName('level_IP');
          n.addClassName('level_NA_IP');
        }
      }
    });
    // as last we set the original(deleted) element on nonactive
    if (element.className == 'level') {
      element.removeClassName('level');
      element.addClassName('level_NA');

    } else if (element.className == 'competency') {
      element.removeClassName('competency');
      element.addClassName('competency_NA');
    } else if (element.className == 'level_IP') {
      element.removeClassName('level_IP');
      element.addClassName('level_NA_IP');

    } else if (element.className == 'competency_IP') {
      element.removeClassName('competency_IP');
      element.addClassName('competency_NA_IP');
    }
  }

}

/**
 * This function expands or collapses the divs when there is pushed on the
 * plus/minus icon and changes the icon
 *
 * @param (string)  div id of the div we want to collapse/expand
 * @param (img)  picture id of the icon we want to change
 */
function expand(div, img) {
  if ($(img).src == ("http://" + location.hostname + "/atk/images/plus.gif")) {
    $(img).src = "./atk/images/minus.gif";
  } else {
    $(img).src = "./atk/images/plus.gif";
  }
  $(div).toggle();
    if (collapsed.get(div) )
  {
    collapsed.unset(div);
  }
  else
  {
    collapsed.set(div,1);
  }
}

/**
 * We want a confirmation if we realy want to delete the competency
 *
 * @param (integer) comp the id of the competency
 */
function confirmDelete(comp, message) {
  var return_value = confirm(message);

  // TEST TO SEE IF TRUE|FALSE RETURNED
  if (return_value == true) {
    deleteData(comp);
  }
}

// **********************************************************
// calendar functions
// *************************************************************
/**
 * We open the calendar to chose a date for adding a competency
 *
 * @param (object) object we call teh calendar from
 */
function openAddCalendar(e) {
  compId = e.target.id;
  if (calendar != null) {
    // we already have some calendar created
    calendar.hide(); // so we hide it first.
    calendar = null;
  }

  cal = new Calendar(false, null, getDate, closeCalendar);

  calendar = cal;

  calendar.create();
  calendar.showAtElement(e.target);
}

function getDate(cal, date) {
  sendData(compId, date);
  closeCalendar();
}

/**
 * We open the calendar to chose a date for the scheduler
 *
 * @param (object) object we call teh calendar from
 */
function openCalendar(e) {
  compId = e.target.id;
  if (calendar != null) {
    // we already have some calendar created
    calendar.hide(); // so we hide it first.
    calendar = null;
  }

  cal = new Calendar(false, null, addDate, closeCalendar);
  calendar = cal;
  calendar.create();

  calendar.parseDate(scheduleDate);
  calendar.showAtElement(e.target);
}

/**
 * we add the date to teh database
 *
 * @param (calendar) cal the calendar object
 * @param (date) date the chosee date
 */
function addDate(cal, date) {
  scheduleDate = date;
  scheduleData(compId, date);
  changeCompetencyColors(compId, "SCHEDULE");
  closeCalendar();
}
function closeCalendar() {
  initContextMenus();
  calendar.hide();
}

// *****************************************
// responses
// ******************************************
/**
 *  When mouse is over a competency show the mouseover data param (object) the
 *  object the mouse is over
 */
function onMouseOverComp(e) {

  if (e.type == "mouseover") {
    $('compover').show();
    var scrOfX = 0, scrOfY = 0;
    if (typeof(window.pageYOffset) == 'number') {
      // Netscape compliant
      scrOfY = window.pageYOffset;
      scrOfX = window.pageXOffset;
    } else if (document.body
        && (document.body.scrollLeft || document.body.scrollTop)) {
      // DOM compliant
      scrOfY = document.body.scrollTop;
      scrOfX = document.body.scrollLeft;
    } else if (document.documentElement
        && (document.documentElement.scrollLeft || document.documentElement.scrollTop)) {
      // IE6 standards compliant mode
      scrOfY = document.documentElement.scrollTop;
      scrOfX = document.documentElement.scrollLeft;
    }

    $('compover').style.top = (e.clientY + scrOfY + 10) + "px";
    $('compover').style.left = (e.clientX + scrOfX + 10) + "px";
    if (infoBuffer.get(e.target.id) == undefined) {
      showMouseOver(e.target.id);
    } else {
      $('compover').innerHTML = infoBuffer.get(e.target.id);
    }
  }
  else
  {
    $('compover').hide();
  }
}

/**
 * shows the html in the mousover box
 *
 * @param (request) orignal request
 */
function showOver(originalRequest) {
  // $('loading').style.display = "none";
  infoBuffer.set(originalRequest.request.parameters["compId"],
      originalRequest.responseText);
  $('compover').innerHTML = originalRequest.responseText;
}

/**
 * Shows when ajax is busy loading data
 */
function showLoad() {
  $('loading').style.display = "block";
  $('loading').style.top = (document.documentElement.scrollTop + 300) + "px";
}

/**
 * activeded when the mouse is over a div
 */
function showMouseOverLoad() {
  // $('loading').style.display = "block";
}

/**
 * disables the loading screen when loading is done
 * @param originalRequest
 */
function showResponse(originalRequest) {
  $('loading').style.display = "none";
}

/**
 *
 * @param originalRequest
 */
function showUpdate(originalRequest) {
  $('competences').innerHTML = originalRequest.responseText;
  $('loading').style.display = "none";
toggleDivs();
  updateObservers();
  initContextMenus();
}

// ************************************
// Ajax requests
// ************************************

/**
 * When we change the profile in the select we update the screen with the
 * profile data and then we save the change to the database so the next time we
 * load this screen we get the same profile
 *
 * @param (integer) profileId id of the profile to save
 */
function updateCompetencesWithProfile(profile) {
  var pars = 'profile=' + profile + '&update=true';
  var myAjax = new Ajax.Updater('competences', url + '/_loadCompetences', {
      method     : 'get',
      parameters : pars,
      onLoading : showLoad,
      onComplete : showUpdate
      });
}

/**
 * removes a competence from the profile
 * @param item
 */
function deleteData(item) {
  changeCompetencyColors(item, "REMOVE");

  var rand = Math.random(9999);
  var pars = 'clearProduct=true&compId=' + item + '&profile='+ $("profileChoser").value;
  var myAjax = new Ajax.Updater('_updateActions', url + '/_updateActions', {
        method     : 'get',
        parameters : pars,
      });
}

/**
 * adds a competence to a profile
 * @param item
 * @param date
 */
function sendData(item, date) {
  changeCompetencyColors(item, "ADD");
  var pars = 'compId=' + item + '&profile=' + $("profileChoser").value;
   var myAjax = new Ajax.Updater('_updateActions', url + '/_updateActions', {
        method     : 'get',
        parameters : pars,
      });
}

/**
 * Is loaded when a mouse is over a div that needs to show information
 *
 * @param (integer) compId id of the competence we want data from
 */
function showMouseOver(compId) {
  var pars = 'over=over&compId=' + compId +"&lng="+lng;

   var myAjax = new Ajax.Updater('_updateActions', url + '/_updateActions', {
        method     : 'get',
        parameters : pars,
      onLoading : showMouseOverLoad,
      onComplete : showOver
      });
}

/**
 * exectuted ona page scroll so the legenda will scroll with the page
 * @param e
 */
function onScrollEvent(e) {
  positionX = document.viewport.getDimensions().width - 350;

  offsets = document.viewport.getScrollOffsets();
  $('legenda').style.top = (offsets[1] + 100) + "px";
  $('legenda').style.left = (positionX) + "px";
}

/**
 * Turns the legenda on or off depending on the state
 */
function toggleLegenda() {
  $('legenda').toggle();
}

/**
 * initialzes the position of the legenda
 */
function initLegenda() {
  positionX = document.viewport.getDimensions().width - 350;
  offsets = document.viewport.getScrollOffsets();
  $('legenda').style.top = (offsets[1] + 100) + "px";
  $('legenda').style.left = (positionX) + "px";
}
