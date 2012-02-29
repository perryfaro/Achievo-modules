<div id='competences' name='competeces'>
{foreach from=$domain key=dKey item=dValue}
  <div class="domain">
    <A href="javascript:void(0); onclick=expand('bin{$dKey}','img_bin{$dKey}');"><img style="margin:3px" border=0 id ="img_bin{$dKey}" align="left" src="./atk/images/minus.gif"> {$dKey}</A>
      <div class="domainborderless" id="bin{$dKey}">
        {foreach from=$dValue key=gKey item=gValue}
          {if $gValue.id != "" && count($gValue) == 1}
            <div class="group">{$gKey}
              <div class="groupborderless" id="bin{$gKey}">
              <div class='alwaysshow'>
                 {if $allowAdd}
                 <input type='checkbox' {if $gValue.id|in_array:$alwaysShow}checked="checked" {/if} {if $gValue.id|in_array:$extra}disabled="disabled" {/if}value='{$gValue.id}' class='check'> {atktext module='competency' id='always_show_for_this_profile'}
                 <input type='checkbox'  {if $gValue.id|in_array:$extra}checked="checked" {/if} {if $gValue.id|in_array:$alwaysShow}disabled="disabled" {/if} value='{$gValue.id}' class='extra'>{atktext module='competency' id='extra_competence'}
				{/if}
                <div class={if $gValue.id|in_array:$competences.comp} "level" {else} "level_NA" {/if} onMouseDown="clicked = this.id" id="competency!{$gValue.id}">
                 {$gKey}
                </div>
              </div>
            </div>
          {else}
          <div class="group">
            {$gKey}
              <div class='alwaysshow'>
			    {if $allowAdd}
                <input type='checkbox'  {if $gValue.id|in_array:$alwaysShow}checked="checked" {/if} {if $gValue.id|in_array:$extra}disabled="disabled" {/if} value='{$gValue.id}' class='check'> {atktext module='competency' id='always_show_for_this_profile'}
                <input type='checkbox'  {if $gValue.id|in_array:$extra}checked="checked" {/if} {if $gValue.id|in_array:$alwaysShow}disabled="disabled" {/if} value='{$gValue.id}' class='extra'>{atktext module='competency' id='extra_competence'}
				{/if}
              </div>
             <div class="groupborderless" id="bin{$gKey}">
              {foreach from=$gValue key=cKey item=cValue}
              {if $cKey != 'id'}
                <div class={if $cValue|in_array:$competences.level}"level"{else}"level_NA"{/if}" id="niveau!{$cValue}">
                 {$cKey}
               </div>
               {/if}
              {/foreach}

             </div>
           </div>
          {/if}
        {/foreach}
       </div>
    </div>
{/foreach}
</div>

