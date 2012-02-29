var divStat = new Array();
var active;
var user = 0;//what user are we working with
var calendar;//the calendar object
var compId;//competency id
var lng;//language of the user
var currentDate = new Date();//today
var scheduleDate = currentDate.getFullYear() + "-" + (currentDate.getMonth() + 4) + "-" + currentDate.getDate();//offset of date +4 months
var deleteMessage;//the message we want to give when we ask if the user realy wants to remove the competency
var infoBuffer = $H();//Buffer for the mouse over so we dont need to keep loading the data we just remeber it here
var modDir = "";//directory of the module
var allowed = "nothing"; //what is the user allowed todo
var allowedAdd = false;
var userhasviewall = false;//may the user see everything or not
var addMenuItems = "";//container to hold all the context menu items for the add menu
var deleteMenuItems = "";//container to hold all the context menu items for the delete menu
var collapsed= $H();//Var to keep track of what competency domainds we have collapsed
var  url = './dispatch.php?atknodetype=competency.competencymatch&atkaction=stats&atklevel=-1&atkprevlevel=0&uri=%2Fcompetency/competency';
var addCall = '';
var saveUrl = '';

//initializes the context menu used to add/deete competences and do the other right mousebutton actions
function addMenu(menuObtained, menuObtainedOn, menuSchedule, menuDelete)
{
  addMenuItems = [
  {
    name      : menuObtained,
    className : 'add',
    callback  : function(e)
    {
      sendData(e.target.id);
      initContextMenus();
    }
  },
  {
    name      : menuObtainedOn,
    className : 'addDate',
    callback  : function(e)
    {
      openAddCalendar(e);
      compId = e.target.id;
    }
  },
  {
    separator : true
  },
  {
    name      : menuSchedule,
    className : 'Schedule',
    callback  : function(e)
    {
      addScheduleDialog(e);
    }
  }];

  deleteMenuItems = [
  {
    name      : menuDelete,
    className : 'Delete',
    callback  : function(e)
    {
      confirmDelete(e.target.id, deleteMessage);
      initContextMenus();
    }
  }];
}

var timeout;
//shows a dialog with the given message
function showDialog(msg)
{
  Dialog.info(msg,
  {
    width        : 250,
    height       : 100,
    showProgress : true
  });
  timeout = 3;
  setTimeout(infoTimeout, 1000)
}

//loops until a certain time is passed
function infoTimeout(msg)
{
  timeout--;
  if (timeout > 0)
  {
    setTimeout(infoTimeout, 1000)
  } else
    Dialog.closeInfo()
}

/**
 * initializes all context menus
 */
function initContextMenus()
{
  new Proto.Menu(
      {
        selector   : '.level',
        className  : 'menu desktop',
        menuItems  : deleteMenuItems,
        // array of menu items
        beforeShow : function(e)
        {
          var paras = $A($$('div.menu'));
          paras.each(Element.hide);
        }
      });
  new Proto.Menu(
      {
        selector   : '.competency',
        className  : 'menu desktop',
        menuItems  : deleteMenuItems,
        // array of menu items
        beforeShow : function(e)
        {
          var paras = $A($$('div.menu'));
          paras.each(Element.hide);
        }
      });
  new Proto.Menu(
      {
        selector   : '.level_NA',
        className  : 'menu desktop',
        menuItems  : addMenuItems,
        // array of menu items
        beforeShow : function(e)
        {
          var paras = $A($$('div.menu'));
          paras.each(Element.hide);
        }
      });
  new Proto.Menu(
        {
          selector   : '.level_NA_PL',
          className  : 'menu desktop',
          menuItems  : addMenuItems,
          // array of menu items
          beforeShow : function(e)
          {
            var paras = $A($$('div.menu'));

            paras.each(Element.hide);
          }
        });
  new Proto.Menu(
      {
        selector   : '.competency_NA',
        className  : 'menu desktop',
        menuItems  : addMenuItems,
        // array of menu items
        beforeShow : function(e)
        {
          var paras = $A($$('div.menu'));
          paras.each(Element.hide);
        }
      });

  new Proto.Menu(
      {
        selector   : '.level_IP',
        className  : 'menu desktop',
        menuItems  : deleteMenuItems,
        // array of menu items
        beforeShow : function(e)
        {
          var paras = $A($$('div.menu'));
          paras.each(Element.hide);
        }
      });
  new Proto.Menu(
      {
        selector   : '.competency_IP',
        className  : 'menu desktop',
        menuItems  : deleteMenuItems,
        // array of menu items
        beforeShow : function(e)
        {
          var paras = $A($$('div.menu'));
          paras.each(Element.hide);
        }
      });
  new Proto.Menu(
      {
        selector   : '.level_NA_IP',
        className  : 'menu desktop',
        menuItems  : addMenuItems,
        // array of menu items
        beforeShow : function(e)
        {
          var paras = $A($$('div.menu'));

          paras.each(Element.hide);
        }
      });

  new Proto.Menu(
        {
          selector   : '.level_NA_PL_IP',
          className  : 'menu desktop',

          menuItems  : addMenuItems,
          // array of menu items
          beforeShow : function(e)
          {
            var paras = $A($$('div.menu'));
            paras.each(Element.hide);
          }
        });

  new Proto.Menu(
      {
        selector   : '.competency_NA_IP',
        className  : 'menu desktop',
        menuItems  : addMenuItems,
        // array of menu items
        beforeShow : function(e)
        {
          var paras = $A($$('div.menu'));
          paras.each(Element.hide);
        }
      });
}

/**
 * We set the user variable
 *
 * @param (integer) id of the user
 */
function setUser(id)
{
  user = id;
}

/**
 * Event listener for the user tabs it will change the user id call an ajax
 * function to reload data in the competences area and higlight the now chosen
 * user
 *
 * @param (event) e the mouse event
 */
function changeUser(e)
{
  var id = e.target.value;
  if (user != id)
  {
    user = id;
    updateCompetences();
    updateTodo();
  }
}

/**
 * Event listener for the profile tabs it will change the user id call an ajax
 * function to reload data in the competences area and higlight the now chosen
 * profile
 *
 * @param (event) e the mouse event
 */
function changeProfile(e)
{
  var id = e.target.value;
  updateCompetencesWithProfile(id);
}

/**
 * Event listener for the show extra checkbox if this is checked we will show
 * also the extra competences that we normaly dont show.
 *
 * @param (event) e the mouse event
 */
function changeShowExtra(e)
{
  if(e.target.checked){extra=1}else{extra=0};
  var pars = 'update=true&userId=' + user+'&showExtra='+extra;
  var myAjax = new Ajax.Updater('_loadCompetences', url + '/_loadCompetences', {
    method     : 'get',
    parameters : pars,
    onLoading  : showLoad,
    onComplete : showUpdate
  });
  return false;
}

/**
 * called in the beginning is used to set the beginning observers in this page
 */
function setObserv()
{
  var personChoser = $A(document.getElementsByClassName('personChoser'));
  personChoser.each(function(element)
  {
    element.observe("change", changeUser)
  });

  var showExtra = $A(document.getElementsByClassName('showextra'));
  showExtra.each(function(element)
  {
    element.observe("change", changeShowExtra)
  });

  Event.observe(window, "scroll", onScrollEvent);
  updateObservers();
}

/**
 * When we reload the competency data we will lose our observers So after we
 * change the competency data we will call this function to make observers of
 * the new tabs
 */
function updateObservers()
{
  var profileChoser = $A(document.getElementsByClassName('profileChoser'));
  profileChoser.each(function(element)
  {
    element.observe("change", changeProfile);
  });

  var competences = $A($$('.level_NA_IP',
                          '.level_NA_PL',
                          '.level_IP',
                          '.level_NA',
                          '.level',
                          '.level_NA_PL_IP'
                          ));

  competences.each(function(element)
  {
    element.observe("mouseover", onMouseOverComp);
    element.observe("mouseout", onMouseOverComp);
  });
}

// to see after a div refresh what the status is of the div
// 0 is normal state and 1 means its collapsed
function initDivStatus()
{
  // first we check the domains
  var domains = document.getElementsByClassName('domainborderless');
  for (var i = 0; i < domains.length; i++)
  {
    divStat[domains[i].id] = 0;
  }
}

/**
 * This is the methode that takes care of changing all the colors/styles of
 * competences on the fly this makes that we dont have to refresh the screen all
 * the time but all the operations on the database are executed in the
 * background while java takes care of the visual part in the foreground
 *
 * @param (integer) compId competency to change color of
 * @param (String) type the type of what we are going to do (choices ADD or REMOVE)
 */
function changeCompetencyColors(compId, type)
{
  var element = $(compId);

  infoBuffer.unset(compId);
  if (type == "ADD")
  {
    var sibArray = element.previousSiblings();

    sibArray.each(function(n)
    {
      infoBuffer.unset(n.id);
      if (n.className != 'level' || n.className != 'level_IP')
      {
       //In case of add change from level not acquired to level aquired
        if (n.className == 'level_NA')
        {
          n.removeClassName('level_NA');
          n.addClassName('level');
        }
        //In case of add change from level not acuired in profile to acquired in profile
        if (n.className == 'level_NA_IP')
        {
          n.removeClassName('level_NA_IP');
          n.addClassName('level_IP');
        }
        //In case of add change from level not acquired but in profile and planned to acuired in profile
        if (n.className == 'level_NA_PL_IP')
        {
          n.removeClassName('level_NA_PL_IP');
          n.addClassName('level_IP');
        }
        //In case of add change from  level acquired and planned to level acquired
        if (n.className == 'level_NA_PL')
        {
          n.removeClassName('level_NA_PL');
          n.addClassName('level');
        }
      }
    });

    if (element.className == 'level_NA')
    {//In case of add change from level not acquired to level aquired
      element.removeClassName('level_NA');
      element.addClassName('level');
    } else if (element.className == 'competency_NA')
    {//In case of add change from competency not acquired to competency acquired
      element.removeClassName('competency_NA');
      element.addClassName('competency');
    } else if (element.className == 'competency_NA_PL')
    {//In case of add change from competency not acquired planned to acquired
        element.removeClassName('competency_NA_PL');
        element.addClassName('competency');
    } else if (element.className == 'level_NA_IP')
    {//In case of add change from level not acuired in profile to acquired in profile
      element.removeClassName('level_NA_IP');
      element.addClassName('level_IP');
    } else if (element.className == 'level_NA_PL')
    {//In case of add change from  level acquired and planned to level acquired
      element.removeClassName('level_NA_PL');
      element.addClassName('level');
    }else if (element.className == 'level_NA_PL_IP')
    {//In case of add change from level not acquired but in profile and planned to acuired in profile
      element.removeClassName('level_NA_PL_IP');
      element.addClassName('level_IP');
    }
    else
    {//In case of add change from competency not acquired in profile to acquired in proifle
      element.removeClassName('competency_NA_IP');
      element.addClassName('competency_IP');
    }
  } else if (type == "REMOVE")
  {
    var sibArray = element.nextSiblings();
    sibArray.each(function(n)
    {
      // we run trough all higherlaying siblings and set them on nonactive
      if (n.className != 'level_NA' || n.className == 'level_NA_IP')
      {
        infoBuffer.unset(n.id);
        //In case of remove change from level aquired to level not acquired
        if (n.className == 'level')
        {
          n.removeClassName('level');
          n.addClassName('level_NA');
        }
        //In case of remove change from level aquired in profile to level not acquired in profile
        if (n.className == 'level_IP')
        {
          n.removeClassName('level_IP');
          n.addClassName('level_NA_IP');
        }
      }
    });
    // as last we set the original(deleted) element on nonactive
    if (element.className == 'level')
    {//In case of remove change from level aquired to level not acquired
      element.removeClassName('level');
      element.addClassName('level_NA');
    } else if (element.className == 'competency')
    {//In case of remove change from competency aquired to competency not acquired
      element.removeClassName('competency');
      element.addClassName('competency_NA');
    } else if (element.className == 'level_IP')
    {//In case of remove change from level aquired in profile to level not acquired in profile
      element.removeClassName('level_IP');
      element.addClassName('level_NA_IP');
    } else if (element.className == 'competency_IP')
    {//In case of remove change from competency in profile aquired to competency in profile not acquired
      element.removeClassName('competency_IP');
      element.addClassName('competency_NA_IP');
    }
  } else if(type == "SCHEDULE")
  {
    if (element.className == 'level_NA')
    {//In case of remove change from level aquired to level not acquired
      element.removeClassName('level_NA');
      element.addClassName('level_NA_PL');
    } else if (element.className == 'level_NA_IP')
    {//In case of remove change from competency aquired to competency not acquired
      element.removeClassName('level_NA_IP');
      element.addClassName('level_NA_PL_IP');
    }
  }
}

/**
 * This function expands or collapses the divs when there is pushed on the
 * plus/minus icon and changes the icon
 *
 * @param (string) div id of the div we want to collapse/expand
 * @param (img)  picture id of the icon we want to change
 */
function expand(div, img, domain)
{
  if ($(img).src == ("http://" + location.hostname + "/atk/images/plus.gif"))
  {
    $(img).src = "./atk/images/minus.gif";
  } else
  {
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
function confirmDelete(comp, message)
{
  var return_value = confirm(message);

  if (return_value == true)
  {
    deleteData(comp, user);
  }
}

// **********************************************************
// calendar functions
// *************************************************************
/**
 * We open the calendar to chose a date for adding a competency
 *
 * @param (object) object we call the calendar from
 */
function openAddCalendar(e)
{
  compId = e.target.id;
  if (calendar != null)
  {
    // we already have some calendar created
    calendar.hide(); // so we hide it first.
    calendar = null;
  }

  cal = new Calendar(false, null, getDate, closeCalendar);

  calendar = cal;

  calendar.create();
  calendar.showAtElement(e.target);
}

/**
 * When we chose to add a cmpetency with a differnt date than today we will get the date here
 * @param cal the calendar
 * @param date the date
 * @return void
 */
function getDate(cal, date)
{
  sendData(compId, date);
  closeCalendar();
}

/**
 * We open the calendar to chose a date for the scheduler
 *
 * @param (object)  object we call the calendar from
 */
function openCalendar(e)
{
  if (calendar != null)
  {
    // we already have some calendar created
    calendar.hide(); // so we hide it first.
    calendar = null;
  }

  cal = new Calendar(false, null, setDate, closeCalendar);
  calendar = cal;
  calendar.create();

  calendar.parseDate(scheduleDate);
  calendar.showAtElement(e.target);
}

/**
 * we add the date to the database
 *
 * @param (calendar) cal the calendar object
 * @param (date) date the chosee date
 */
function addDate(cal, date)
{
  scheduleDate = date;
  scheduleData(compId, date);
  updateTodo();
  changeCompetencyColors(compId, "SCHEDULE");
  closeCalendar();
}

/**
 * @param cal The calendar object.
 * @param date The selected date
 */
function setDate(cal, date) {
    scheduleDate = date;

    var form = $('scheduleForm');
    var dateField = form['schedule_plandate'];

    dateField.setValue(date);
    closeCalendar();
}

/**
 * Closes the calendar we use to chose dates for as well shedules as to chose
 * on what date a competence was acquired
 * @return
 */
function closeCalendar()
{
  initContextMenus();
  calendar.hide();
}

// *****************************************
// responses
// **********************************************************
/**
 * 'When mouse is over a competency show the mouseover data param (object)e the
 * object the mouse is over
 */
function onMouseOverComp(e)
{
  // when we have an empty id it means we have our mouse over the legenda
  if (e.target.id != "")
  {

    if (e.type == "mouseover")
    {
      $('compover').show();
      var scrOfX = 0, scrOfY = 0;
      if (typeof(window.pageYOffset) == 'number')
      {
        // Netscape compliant
        scrOfY = window.pageYOffset;
        scrOfX = window.pageXOffset;
      } else if (document.body
          && (document.body.scrollLeft || document.body.scrollTop))
      {
        // DOM compliant
        scrOfY = document.body.scrollTop;
        scrOfX = document.body.scrollLeft;
      } else if (document.documentElement
          && (document.documentElement.scrollLeft || document.documentElement.scrollTop))
      {
        // IE6 standards compliant mode
        scrOfY = document.documentElement.scrollTop;
        scrOfX = document.documentElement.scrollLeft;
      }

      $('compover').style.top = (e.clientY + scrOfY + 10) + "px";
      $('compover').style.left = (e.clientX + scrOfX + 10) + "px";

      if (infoBuffer.get(e.target.id) == undefined)
      {
        showMouseOver(e.target.id);
      } else
      {
        $('compover').innerHTML = infoBuffer.get(e.target.id);
      }
    } else
    {
      $('compover').hide();
    }
  }
}

/**
 * shows the html in the mousover box
 *
 * @param (request) orignal request
 */
function showOver(originalRequest)
{
  $('loading').style.display = "none";
  infoBuffer.set(originalRequest.request.parameters["compId"],
      originalRequest.responseText);
  $('compover').innerHTML = originalRequest.responseText;
}

/**
 * Shows when ajax is busy loading data
 */
function showLoad()
{
  $('loading').style.display = "block";
  $('loading').style.top = (document.documentElement.scrollTop + 300) + "px";
}

/**
 * Shows a spinner when the mouseover is waitning to recieve its data
 * @return
 */
function showMouseOverLoad()
{
//   $('loading').style.display = "block";
}

/**
 * We set the loading spinner to no show
 * @param originalRequest
 * @return
 */
function showResponse(originalRequest)
{
  $('loading').style.display = "none";
}

/**
 * The actions we do after an update
 *
 * @param originalRequest
 * @return
 */
function showUpdate(originalRequest)
{
  $('loading').style.display = "none";
  var infoBuffer = $H();
  updateObservers();
  toggleDivs();
  setTimeout(function() {
    if (allowedAdd)
    {
      initContextMenus();
    }
  }, 0);
}

/**
 * Expands or collapse the divs
 * @return
 */
function toggleDivs()
{
  tempCollapsed = collapsed;
  collapsed = $H();
  tempCollapsed.each(function(pair){
  expand(pair.key,"img_"+pair.key)
  } )
}
// ************************************
// Ajax requests
// ************************************
/**
 * we send an ajax request to reload all data of competencies this will usualy
 * happen when we change of person
 */
function updateCompetences()
{
  $('showextra').checked=false;
  var pars = 'update=true&userId=' + user;
  var myAjax = new Ajax.Updater('_loadCompetences', url + '/_loadCompetences', {
      method     : 'get',
      parameters : pars,
      evalScripts: true,
      onLoading  : showLoad,
      onComplete : showUpdate
    });
 return false;
}

/**
 * When we change the profile in the select we update the screen with the
 * profile data and then we save the change to the database so the next time we
 * load this screen we get the same profile
 *
 * @param (integer) profileId id of the profile to save
 */
function updateCompetencesWithProfile(profile)
{
  $('showextra').checked=false;

  var pars = 'profile=' + profile + '&update=true&userId=' + user;

  var myAjax = new Ajax.Updater('_loadCompetences', url + '/_loadCompetences', {
      method     : 'get',
      parameters : pars,
      onLoading  : showLoad,
      onComplete : showUpdate
    });
    return false;
}

/**
 * Removes a competency from the list and updates the screens accordingly
 *
 * @param item id of the competency we want to remove
 * @return
 */
function deleteData(item)
{
  changeCompetencyColors(item, "REMOVE");

  var pars = 'clearProduct=true&compId=' + item + '&userId=' + user;
  var myAjax = new Ajax.Updater('_updateActions', url + '/_updateActions', {
      method     : 'get',
      parameters : pars
    });
  updateTodo();
return false;
}

/**
 * We add a new competency to the list and then update all the views
 * we have
 *
 * @param item id of the competency we need to update
 * @param date date the date for when the competency is added (when empty the current date is used)
 * @return
 */
function sendData(item, date)
{
  changeCompetencyColors(item, "ADD");
  var pars = 'compId=' + item + '&userId=' + user + '&date=' + date;

  var myAjax = new Ajax.Updater('_updateActions', url + '/_updateActions', {
      method     : 'get',
      parameters : pars
    });
  updateTodo();
return false;
}

/**
 * adds the data to the scheduler
 *
 * @param (integer)compId id of the competence
 * @param (date)date to be scheduled
 */
function scheduleData(compId, date)
{
  var pars = 'schedule=' + date + '&compId=' + compId + '&userId=' + user;

  var myAjax = new Ajax.Updater('scheduleData', url + '/_updateActions', {
      method     : 'get',
      parameters : pars
    });
   updateTodo();
}

/**
 * Is loaded when a mouse is over a div that needs to show information
 *
 * @param (integer)compId id of the competence we want data from
 */
function showMouseOver(compId)
{
  var pars = 'over=over&compId=' + compId + '&userId=' + user + '&lng=' + lng;
  var myAjax = new Ajax.Updater('_updateActions', url + '/_updateActions', {
      method     : 'get',
      parameters : pars,
      onLoading  : showMouseOverLoad,
      onComplete : showOver
    });
}

/**
 * Is loaded when a mouse is over a div that needs to show information
 *
 * @param (integer) compId id of the competence we want data from
 */
function updateProfile(msg)
{
  profile = $('profileChoser').options[$('profileChoser').selectedIndex].value
  var pars = 'type=profile&profile=' + profile + '&userId=' + user;
  var myAjax = new Ajax.Updater('_updateActions', url + '/_updateActions', {
      method     : 'get',
      parameters : pars,
      onLoading  : showMouseOverLoad,
      onComplete : showDialog(msg)
    });

}

/**
 * When we scroll our window we want to move the legenda as well
 * @param e the action event
 * @return void
 */
function onScrollEvent(e)
{
  positionX = document.viewport.getDimensions().width - 450;

  offsets = document.viewport.getScrollOffsets();
  $('legenda').style.top = (offsets[1] + 100) + "px";
  $('legenda').style.left = (positionX) + "px";
}

/**
 * Shows or hides the legenda (depending on the state)
 * @return
 */
function toggleLegenda()
{
  $('legenda').toggle();
}

/**
 * closes the comment screen.
 * @return void
 */
function closeComment()
{
  $("comments").clear();
  $('comment').hide();
}

/**
 * Send our new comment to the database and when this completes calls the
 * function to show our changes
 * @param competency
 * @return void
 */
function sendComment(competency)
{
  comment = $('comments').value;
  competency = $('comp').value;
  var pars = 'type=comment&comment=' + comment + '&userId=' + user
      + '&competency=' + competency;

  var myAjax = new Ajax.Updater('_updateActions', url + '/_updateActions', {
      method     : 'get',
      parameters : pars,
      onLoading  : showMouseOverLoad,
      onComplete : updateTodo
    });
}

/**
 * This function will do an ajax request to get the new htnl we want to show in our
 * todo div when we just updated a planning or a comment for the competency we selected
 * @return void
 */
function updateTodo()
{
  competency = $('comp').value;
  var pars = 'type=gettodo&userId=' + user + '&competency=' + competency;
  var myAjax = new Ajax.Updater('_updateActions', url + '/_updateActions', {
      method     : 'get',
      parameters : pars,
      onLoading  : showMouseOverLoad,
      onComplete : updateTodoDiv
    });
}

/**
 * When we change antything in the planning or comments we need to load this data into the todo div
 * @param originalRequest Responsetext of the ajaxcall containig our new html
 * @return void
 */
function updateTodoDiv(originalRequest)
{
  $("_getTodo").innerHTML = originalRequest.responseText;
  $("comments").clear();
  $('comment').hide();
}

/**
 * Sends an ajax request to the server that will load teh already existing comment and
 * send it back
 * @param competency integer The id of the competency
 * @return void
 */
function getComment(competency)
{
  comment = $('comments').value;
  competency = $('comp').value;
  var pars = 'type=getcomment&userId=' + user + '&competency=' + competency;

  var myAjax = new Ajax.Updater('_updateActions', url + '/_updateActions', {
      method     : 'get',
      parameters : pars,
      onComplete : fillComment
    });
}

/**
 * Fills the comment popup with the data we have in the database
 * @param originalRequest the orignial request we got back
 * @return void
 */
function fillComment(originalRequest)
{
  $("comments").value = originalRequest.responseText;
}

/**
 * Shows or hides the comment screen where users can fill in some remarkt conserning the employee/competence
 * @param compId integer that is the id of the competency
 * @return void
 */
function toggleComment(compId)
{
  positionX = document.viewport.getDimensions().width;
  positionY = document.viewport.getDimensions().height;
  offsets = document.viewport.getScrollOffsets();
  $("comp").value = compId;

  // we load existing data
  getComment();

  $('comment').style.top = (((positionY / 2) + offsets[1]) - 100) + "px";
  $('comment').style.left = ((positionX / 2) - 200) + "px";
  $('comment').show();
}

/**
 * Initializes the legenda that shows what colors and icons are what in the list
 * @return
 */
function initLegenda()
{
  positionX = document.viewport.getDimensions().width - 400;
  offsets = document.viewport.getScrollOffsets();
  $('legenda').style.top = (offsets[1] + 100) + "px";
  $('legenda').style.left = (positionX) + "px";
}

/**
 * Opens a new window and ads the content of our todo div to it
 * as soon as its done loading we will give a order to start printing
 * the user will get a print dialog where they can select what printer etc
 * @return void
 */
function printTodo()
{
  var disp_setting="toolbar=yes,location=no,directories=yes,menubar=yes,";
      disp_setting+="scrollbars=yes,width=650, height=600, left=100, top=25";
  var content_vlue =$("_getTodo").innerHTML;
 //we need to give a bit of css to the print window so it has a bit better makeup
  var css = '<style type="text/css">.todotable{ border: 1px solid;width:100%;}.todotable THEAD{border-collapse: collapse;background-color: #F0F0F0;} .todotable .domaincell{border-top: 1px solid;}</style>';

  var docprint=window.open("","",disp_setting);
  docprint.document.open();
  docprint.document.write('<html><head><title>Competences</title>');
  docprint.document.write(css);
  docprint.document.write('</head><body onLoad="self.print()">');
  docprint.document.write(content_vlue);
  docprint.document.write('</body></html>');
  docprint.document.close();
  docprint.focus();
}

/**
 * Show Schedule dialog.
 */
function addScheduleDialog(e)
{
  compId = e.target.id;
  this.addCall.call(null, {competence_id: compId, user: user});
}

function saveScheduleDialog()
{
  var form = $('scheduleForm');
  var remark = form['schedule_remark'];
  var date = form['schedule_plandate'];
  var person = form['schedule_person_id'];
  var competence = form['schedule_competence_id'];
  var params = $H();
  
  params.set('remark', $(remark).getValue());
  params.set('date', $(date).getValue());
  params.set('person', $(person).getValue());
  params.set('competence', $(competence).getValue());

  if ($(date).getValue() != '')
  {
    new Ajax.Request(saveUrl, {parameters: params, onComplete: function(request) {request.responseText.evalScripts();}});
    ATK.Dialog.getCurrent().close();
    updateCompetences();
    updateTodo();
  }
  else
  {
    // Date is empty, just close the Dialog.
    ATK.Dialog.getCurrent().close();
  }
}

function setScheduleURLs(add, save) {
    addCall = add;
    saveUrl = save;
}

