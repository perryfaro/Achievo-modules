<div name='teams'>
  {$teams}
</div>
<div style="float:right"><a href="javascript:void(0)" onclick="toggleLegenda()">Help</a></div>
<br>
<div name='table' id='table' class='table'>
{_partial "table"}
</div>

<div class=compover id=compover></div>

  <script type="text/javascript">
  {$script}
  </script>

  <div class=legenda id=legenda>
  Legenda
    <div id=green class=green>{atktext id="acquired_and_in_profile" module="competency"}</div>
    <div id=blue class=blue>{atktext id="acquired_but_not_in_profile" module="competency"}</div>
    <div id=geel class=geel>{atktext id="in_profile_but_not_acquired" module="competency"}</div>
 </div>
