<div class='_loadCompetences' name='_loadCompetences' id='_loadCompetences'>
{atktext module='competency' id='_profile'}{_partial "fillProfile"}
{if $allowSetProfile}
  <a href="javascript:updateProfile('{atktext module="competency" id="profile_for_person_has_changed"}')">        {atktext module="competency" id="set as employee's Profile"}
{/if}
<div class='competences' name='competences'>
{foreach from=$domain key=dKey item=dValue}
  <div class="domain" id="domain{$dKey}">
    <A href="javascript:void(0); onclick=expand('bin{$dKey}','img_bin{$dKey}','domain{$dKey}');"> <img style="margin:3px" border=0 id ="img_bin{$dKey}" align="left" src="./atk/images/minus.gif"> {$dKey}</A>
      <div class="domainborderless" id="bin{$dKey}">
        {foreach from=$dValue key=gKey item=gValue}
          {if $gValue.id != ""}
          {assign var=comp value="!"|explode:$gKey}
            <div class="group">{if $allowed=='team'}<a href="javascript:void(0);" onclick=toggleComment({$comp[1]})>{$comp[0]}</a>{else}{$comp[0]}{/if}
              <div class="groupborderless" id="bin{$gKey}">
                <div class={if $gValue.id|in_array:$competences.comp}level{else}level_NA{if $gValue.id|in_array:$planned.comp}_PL{/if}{/if}{if $gValue.id|in_array:$profiles.comp}_IP{/if} onMouseDown="clicked = this.id" id="competency!{$gValue.id}">
                  {$comp[0]}
                </div>
              </div>
            </div>
          {else}
          <div class="group">
            {assign var=comp value="!"|explode:$gKey}
            {if $allowed=='team'}<a href="javascript:void(0);" onclick=toggleComment({$comp[1]})>{$comp[0]}</a>{else}{$comp[0]}{/if}
             <div class="groupborderless" id="bin{$gKey}">
              {foreach from=$gValue key=cKey item=cValue}
                <div class={if $cValue|in_array:$competences.level}level{else}level_NA{if $cValue|in_array:$planned.level}_PL{/if}{/if}{if $cValue|in_array:$profiles.level}_IP{/if} id="niveau!{$cValue}">
                 {$cKey}
               </div>
              {/foreach}
             </div>
           </div>
          {/if}
        {/foreach}
       </div>
    </div>
{/foreach}
</div>
</div>
<script type="text/javascript">
window.allowedAdd = {if $allowAdd}1{else}0{/if};
</script>
