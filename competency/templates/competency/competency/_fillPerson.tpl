<Select name="persons" class="personChoser">
{foreach from=$persons item=person}
      {if $person.id==$pers}
        <option selected="selected" value="{$person.id}">{$person.firstname} {$person.lastname}</option>
      {else}
        <option value="{$person.id}">{$person.firstname} {$person.lastname}</option>
   {/if}
{/foreach}
</select>