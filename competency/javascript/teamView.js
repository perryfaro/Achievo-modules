var infoBuffer = $H();
var modDir = "";
var url = './dispatch.php?atknodetype=competency.competencymatch&atkaction=stats&atklevel=-1&atkprevlevel=0&uri=%2Fcompetency/team';

function showMouseOver(id, level, user, comp)
{
  var pars = 'id=' + id + '&level=' + level + '&user=' + user + '&comp=' + comp;
//  var myAjax = new Ajax.Request(url,
//  {
//    method     : 'get',
//    parameters : pars,
//    onComplete : showDiv
//  });
  var myAjax = new Ajax.Updater('getText', url + '/_getText', {
      method     : 'get',
      parameters : pars,
      onComplete : showDiv,
    });
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
 * inititalizes the position of the legenda
 */
function initLegenda()
{
  positionX = document.viewport.getDimensions().width - 400;
  offsets = document.viewport.getScrollOffsets();
  $('legenda').style.top = (offsets[1] + 100) + "px";
  $('legenda').style.left = (positionX) + "px";
  $('legenda').hide();
}

/**
 * observe the scroll event
 */
function setObserv()
{
  Event.observe(window, "scroll", onScrollEvent);

}

/**
 * hides the mouseover when we go out of a cel
 */
function mouseOutCell()
{
  $('compover').hide();
}

/**
 * chows the mouseover when we are over a cell
 * @param originalRequest
 */
function showDiv(originalRequest)
{
  infoBuffer.set(originalRequest.request.parameters["id"] + " "
      + originalRequest.request.parameters["level"] + " "
      + originalRequest.request.parameters["user"],
      originalRequest.responseText);

  $('compover').innerHTML = originalRequest.responseText;
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
 * When we are over a cell we show teh mouseover that contains information about
 * the competency and person
 * @param e
 * @param id
 * @param level
 * @param user
 * @param comp
 * @return
 */
function mouseOverCell(e, id, level, user, comp)
{

  $('compover').show();
  if (e.type == "mouseover")
  {
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

    if (infoBuffer.get(id + " " + level + " " + user) == undefined)
    {
      showMouseOver(id, level, user, comp);
    } else
    {
      $('compover').innerHTML = infoBuffer.get(id + " " + level + " " + user);
    }

  }
}

/**
 * add listeners to the team select
 */
function setListeners()
{
   $("teams").observe("change", changeTeam)
}

/**
 * updates the table when the team select changes
 * @param e
 */
function changeTeam(e)
{
  var id = e.target.value;
  var pars = 'type=updateTeam&id=' + id;

  var myAjax = new Ajax.Updater('table', url + '/_table', {
      method     : 'get',
      parameters : pars,
    });
}