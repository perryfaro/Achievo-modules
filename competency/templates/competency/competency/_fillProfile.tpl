<Select name="profileChoser" class="profileChoser" id="profileChoser">
<option value='0'>{atktext module='competency' id='no profile'}</option>
    {foreach from=$listprofiles item=profile}

      {if $profile.id==$profileId ||$profile==$profileId }
        <option selected="selected" value="{$profile.id}">{$profile.name}</option>
      {else}
        <option value="{$profile.id}">{$profile.name}</option>
   {/if}
   {/foreach}
 </select>
