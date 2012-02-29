<div id="loading"><img src="{$mythemedir}Images/spinner.gif" height= "100" width ="100" alt="Loading..."></div>
{atktext module='competency' id='person'}{_partial "fillPerson"}
<div style="float:right"><a href="javascript:void(0)" onclick="toggleLegenda()">Help</a></div>
<br>
<br>
<div style="float:right">{atktext module=competency id="show_extra_competences"}<input type='checkbox' class='showextra' id='showextra'></div>
<br>
<br>



{_partial "loadCompetences"}

<a href="javascript:printTodo();">Print Todo lijst !</a>

<div id='_getTodo' class='_getTodo' name='_getTodo'>{_partial "getTodo"}</div>

{$comment}

<div class="isset" id="isset">"{atktext module=competency id="profile_for_person_has_changed"}"</div>

<div id=legenda class=legenda>
<table>
<tr>
   <td>
     Legenda
  </td>
   <td width=80%>
   </td>
   <td>
     <A style="float:top" href="javascript:void(0)" onclick="toggleLegenda()"><img alt="close" border=0 src="{$mythemedir}Images/cross.png"></a>
   </td>
 </tr>
</table>

<br clear="all">

<div class=level_L>{atktext id="acquired_but_not_in_profile" module="competency"}</div>
<div class=level_IP_L>{atktext id="acquired_and_in_profile" module="competency"}</div>

 <br clear="all">

<div class=level_NA_L>{atktext id="not_acquired" module="competency"}</div>
<div class=level_NA_IP_L>{atktext id="in_profile_but_not_acquired" module="competency"}</div>
<br clear="all">

<div class=level_NA_PL_L>{atktext id="not_acquired_planned" module="competency"}</div>
<div class=level_NA_IP_PL_L>{atktext id="in_profile_but_not_acquired_planned" module="competency"}</div>
</div>

<div class="compover" id="compover"><div class="loadingcomp" id="loadingcomp"><img src="{$mythemedir}Images/spinner.gif" height="100" width ="100" alt="Loading..."></div>
