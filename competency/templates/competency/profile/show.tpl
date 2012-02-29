<div id="loading"><img src="{$mythemedir}Images/spinner.gif" height= "100" width ="100" alt="Loading..."></div>
<div style="float:right"><a href="javascript:void(0)" onclick="toggleLegenda()">Help</a></div>
<br>
<br>
{atktext module='competency' id='_profile'}{_partial "fillProfile"}

{_partial "loadCompetences"}

<div id=legenda class=legenda>
<table>
<tr>
    <td>
      Legenda
   </td>
    <td width=80%>
    </td>
    <td>
      <A href="javascript:void(0)" onclick="toggleLegenda()"><img alt="close" border=0 src="{$mythemedir}Images/cross.png"></a>
    </td>
  </tr>
</table>

  <br clear="all">
    <div class=level_L>{atktext id="in_profile" module="competency"}</div>
    <div class=level_NA_L>{atktext id="not_in_profile" module="competency"}</div>
</div>

<div class="compover" id="compover"><div class="loadingcomp" id="loadingcomp"><img src="{$mythemedir}Images//spinner.gif" height="100" width ="100" alt="Loading..."></div>